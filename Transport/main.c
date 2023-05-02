// Dawid Sroka, 317239

#include <sys/mman.h>
#include <sys/queue.h>

#include "header.h"

extern int fsize;
extern int outfd;
extern char buffer[SWS][MAX_PACKET_SIZE];	// buffer for received replies
char header_buffer[40 + MAX_PACKET_SIZE];


int main(int argc, char* argv[]) {
  // INPUT VALIDATION
  input_validate(argc, argv);

  // ESTABLISH CONNECTION
  int sockfd = establish_connection(argv[1], argv[2]);

  // INITIALIZE CIRCLE QUEUE 
  circlehead_t head;                 /* Queue head */
  initiailze_q(&head);

  // 	MAIN ROUTINE
  struct packet* winfst = CIRCLEQ_FIRST(&head); // first packet in the window

  int expecting = 0;
  int buf_start_index = 0;
  fsize = atoi(argv[4]);
  uint16_t port = atoi(argv[2]);
  port = (port >> 8) | (port << 8);

  outfd = open(argv[3] ,O_WRONLY | O_TRUNC | O_APPEND);

  int break1 = 1;
  int break2 = 0;

  while(expecting < fsize) {

    // break2 means we can shift the window
    if(break2 == 1) {
      winfst = shift_window_q(&head, winfst, expecting, &buf_start_index); 
      expecting = winfst->pstart;
      break1 = 1;
    }

    // break1 means we have to send all packets in the window again
    if(break1 == 1) {

      struct packet* np = winfst;

      int position = expecting;
      for(int i = 0; i < SWS; i++){
        if (np->state == 0 && np->pstart < fsize) {
          int messlen = 11 + how_long(position); 
          char mes[messlen];
          snprintf(mes, messlen, "GET %d 1000\n", position);
          
          int senderr = send(sockfd, mes, strlen(mes), 0);
          if(senderr == -1){
            fprintf(stderr, "send error: %s\n", strerror(errno));
            return EXIT_FAILURE;				
          }
        }
        position += MAX_PACKET_SIZE;
        np = CIRCLEQ_LOOP_NEXT(&head, np, link);
      }

    }


    // preparing for select
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    struct timeval tv; tv.tv_sec = 0; tv.tv_usec = RTT_us;

    // select + recvfrom
    while (1) {
      int ready = select (sockfd+1, &descriptors, NULL, NULL, &tv);
      if(ready == 0){	// timeout expired and no packet came
        break1 = 1;
        break2 = 0;
        break;
      } else if (ready < 0) {
        fprintf(stderr, "select error: %s\n", strerror(errno));
        return EXIT_FAILURE;
      }

      struct sockaddr_in sender;
      socklen_t sender_len = sizeof(sender);


      ssize_t packet_len = recvfrom (sockfd, header_buffer, 40 + IP_MAXPACKET,
          MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
      if (packet_len < 0) {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        return EXIT_FAILURE;				
      }

      uint32_t addr;
      inet_pton(AF_INET, argv[1], &addr);

      // if packet is valid, analyze it 
      if (packet_len > 0 && sender.sin_addr.s_addr == addr  && sender.sin_port == port) {

        int res_start = 0;
        int res_len = 0;
        char p[10];
        sscanf(header_buffer, "%s %d %d", p, &res_start, &res_len);

        // if packet is in the current window
        if (expecting <= res_start && res_start < expecting + SWS * MAX_PACKET_SIZE) {

          int index = (res_start - expecting) / MAX_PACKET_SIZE;
          struct packet* iter = winfst;
          for (int i = 0; i < index; i++)
            iter = CIRCLEQ_LOOP_NEXT(&head, iter, link);

          // if got new message, update its state and copy the data to buffer
          if(iter->state == 0) {
            int start_len = how_long(res_start);
            int len_len = how_long(res_len);
            memcpy(buffer[(buf_start_index + index) % SWS], header_buffer + 4 + 1 + start_len + 1 + len_len + 1, MIN(MAX_PACKET_SIZE, fsize - res_start));
            iter->state = 1;
            iter->pstart = expecting + index * MAX_PACKET_SIZE;
          }

          //if we hit first packet in the window, we have to shift it
          if (index == 0) {
            break2 = 1;
            break;
          }
        }

      }

    }

  }

  // CLEAN
  close(sockfd);
  close(outfd);

  free_q(&head);

  return EXIT_SUCCESS;
}
