// Dawid Sroka, 317239

#include <sys/mman.h>

#include <sys/queue.h>

#include "header.h"

// #define BUF_SIZE 100
#define MAX_PACKET_SIZE 1000

char buffer[SWS][MAX_PACKET_SIZE];	// buffer for received replies
char header_buffer[40 + MAX_PACKET_SIZE];






int main(int argc, char* argv[]) {
// INPUT VALIDATION

	input_validate(argc, argv);

// ESTABLISH CONNECTION

	int sockfd = -1;
	sockfd = establish_connection(argv[1], argv[2]);

// INITIALIZE CIRCLE QUEUE 


	circlehead_t head;                 /* Queue head */
	CIRCLEQ_INIT(&head);                    /* Initialize the queue */

	for (int i = 0; i < SWS; i++)
	{	
		struct packet* p = malloc(sizeof(struct packet));
		p->state = 0;
		p->pstart = i * MAX_PACKET_SIZE;
    	CIRCLEQ_INSERT_TAIL(&head, p, link);
	}



// 	MAIN ROUTINE

	struct packet* winfst = CIRCLEQ_FIRST(&head); // first packet in the window



	int expecting = 0;

	int size = atoi(argv[4]);
	// int npackets = size / (MAX_PACKET_SIZE + 1); 
	// (void)npackets;
    int outfd = open(argv[3] ,O_WRONLY);

	int break1 = 1;
	int break2 = 0;

	while(expecting < size) {
		// fprintf(stdout, "outer loop\n");
		print_q(&head, winfst);

		if(break2 == 1) {
			// find the last packet that received answer (state == 1)
			struct packet* iter = winfst;
			// struct packet* next = CIRCLEQ_NEXT(iter, link);

			for (int i = 0; i < SWS; i++)
			{
				if ( iter->state == 1) {
					dprintf(outfd, buffer[i]);
					iter->state = 0;	// reset state
					iter->pstart += SWS * MAX_PACKET_SIZE;  
					iter = CIRCLEQ_LOOP_NEXT(&head, iter, link);
				} else {
					break;
				}
			}
			
			winfst = iter;
			print_q(&head, winfst);
			expecting = winfst->pstart;
			fprintf(stdout, "expecting = %d\n", expecting);

			break1 = 1;

		}

		if(break1 == 1) {

			struct packet* np = winfst;

			int position = expecting;
			for(int i = 0; i < SWS; i++){
				if (np->state == 0 && np->pstart < size) {
					// char* mes = "GET 0 1000\n";
					// strcat(mes, itoa(position));
					int messlen = 11 + how_long(position); 
					char mes[messlen];
					snprintf(mes, messlen, "GET %d 1000\n", position);
					printf("%s", mes); // outputs so you can see it
					send(sockfd, mes, strlen(mes), 0);
				}
				position += MAX_PACKET_SIZE;
				np = CIRCLEQ_LOOP_NEXT(&head, np, link);
			}
	
		}


	// preparing for select
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (sockfd, &descriptors);
		struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 100000;

		while (1)
		{
			// select + recvfrom -> wait max 1s

			int ready = select (sockfd+1, &descriptors, NULL, NULL, &tv);
			if(ready == 0){	// timeout expired and no packet came
				fprintf(stdout, "time expired\n");
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
			if (packet_len > 0) {

				int res_start = 0, res_len = 0;
				char p[10];
				sscanf(header_buffer, "%s %d %d", p, &res_start, &res_len);
				// fprintf(stdout, "got %d %d\n", res_start, res_len);

				if (expecting <= res_start && res_start < expecting + SWS * MAX_PACKET_SIZE) {

					int index = (res_start - expecting) / MAX_PACKET_SIZE;
					struct packet* iter = winfst;
					for (int i = 0; i < index; i++)
						iter = CIRCLEQ_LOOP_NEXT(&head, iter, link);

					if(iter->state == 0) {
						fprintf(stdout, "res %d in the window\n", res_start);
						int start_len = how_long(res_start);
						int len_len = how_long(res_len);
						memcpy(buffer[index], header_buffer + 4 + 1 + start_len + 1 + len_len + 1, MAX_PACKET_SIZE);
						iter->state = 1;
						iter->pstart = expecting + index * MAX_PACKET_SIZE;
						// write(outfd, header_buffer, MAX_PACKET_SIZE);
					}


					if (index == 0) {
						fprintf(stdout, "index = 0\n");

						break2 = 1;
						break;
					}
				}

			}

		}

	}




	close(sockfd);
    close(outfd);


// CLEAN

	struct packet *next, *first = CIRCLEQ_FIRST(&head);

	while (first != (void *)&head) {
		next = CIRCLEQ_NEXT(first, link);
		free(first);
		first = next;
	}

	sleep(1);

	return EXIT_SUCCESS;

}