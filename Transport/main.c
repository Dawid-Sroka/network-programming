// Dawid Sroka, 317239

#include <sys/mman.h>

#include <sys/queue.h>

#include "header.h"

// #define BUF_SIZE 100
#define SWS 10
#define MAX_PACKET_SIZE 1000

char buffer[1500];	// buffer for received replies






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
    	CIRCLEQ_INSERT_TAIL(&head, p, link);
	}



// 	MAIN ROUTINE

	struct packet* winfst = CIRCLEQ_FIRST(&head); // first packet in the window

	print_q(&head);


	int expecting = 0;

	int size = atoi(argv[4]);
	// int npackets = size / (MAX_PACKET_SIZE + 1); 
	// (void)npackets;
    int outfd = open(argv[3] ,O_WRONLY);

	int break1 = 1;
	int break2 = 0;

	while(expecting < size) {
		fprintf(stdout, "outer loop\n");

		if(break1 == 1) {
			fprintf(stdout, "break1 == 1\n");

			struct packet* np;

			int position = 0;
			CIRCLEQ_FOREACH(np, &head, link){
				if (np->state == 0 ) {
					// char* mes = "GET 0 1000\n";
					// strcat(mes, itoa(position));
					int messlen = 11 + how_long(position); 
					char mes[messlen];
					snprintf(mes, messlen, "GET %d 1000\n", position);
					printf("%s", mes); // outputs so you can see it
					send(sockfd, mes, strlen(mes), 0);
				}
					position+=1000;
			}	
		}

	// preparing for select
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (sockfd, &descriptors);
		struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 1000000;

		while (1)
		{
			// fprintf(stdout, "inner loop\n");

			// select + recvfrom -> wait max 1s
			// fprintf(stdout, "time left = %ld\n", tv.tv_usec);

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

			ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET,
							MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
			if (packet_len < 0) {
				fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
				return EXIT_FAILURE;				
			}
			if (packet_len > 0) {

				int res_start = 0, res_len = 0;
				char p[10];
				sscanf(buffer, "%s %d %d", p, &res_start, &res_len);
				fprintf(stdout, "got %d %d\n", res_start, res_len);

				if (expecting <= res_start && res_start < expecting + SWS * MAX_PACKET_SIZE) {
					// fprintf(stdout, "res %d in the window\n", res_start);

					int index = (res_start - expecting) / MAX_PACKET_SIZE;
					struct packet* iter = winfst;
					for (int i = 0; i < index; i++) {
						iter = CIRCLEQ_NEXT(iter, link);
					}
					iter->state = 1;

					if (res_start == expecting + MAX_PACKET_SIZE) {
						break2 = 1;
					}
				}

			}


		}

		if(break2 == 1) {
			// struct packet* iter = winfst;
			// struct packet* next;
			// while (iter->state == 1) {
			// 	next = CIRCLEQ_NEXT(iter, link);
			// 	if ( next == winfst) {

			// 	free(iter);
			// 	iter = next;
			// 	}
			// }

			// find the last packet that received answer (state == 1)
			struct packet* iter = winfst;
			struct packet* next = CIRCLEQ_NEXT(iter, link);
			while (next != (void *)&winfst && next->state == 1) {
				iter->state = 0;	// reset state
				// free(iter);
				iter = next;
				next = CIRCLEQ_NEXT(iter, link);
			}

			// take the one after, which is the first packet that didn't receive answer (state == 0)
			iter->state = 0;
			winfst = CIRCLEQ_NEXT(iter, link);
			break1 = 1;

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