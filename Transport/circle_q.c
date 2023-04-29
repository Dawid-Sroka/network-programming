#include <sys/queue.h>

#include "header.h"


// struct packet;

// // I will use CIRCLEQ implementations from C queue library
// typedef struct packet {
// 	uint8_t state;
// 	int pstart;
// 	CIRCLEQ_ENTRY(packet) link;           /* Queue */
// } packet_t ;

// struct circlehead;
// typedef CIRCLEQ_HEAD(circlehead, packet) circlehead_t;


// void print_q(circlehead_t* head) {
//     packet_t* np;
//     int i = 0;
//     CIRCLEQ_FOREACH(np, head, link) {
//         printf("%d %d\n", i, np->state);
//         i++;
//     }
// }

void initiailze_q(circlehead_t* headp){

	CIRCLEQ_INIT(headp);                    /* Initialize the queue */

	for (int i = 0; i < SWS; i++)
	{	
		struct packet* p = malloc(sizeof(struct packet));
		p->state = 0;
		p->pstart = i * MAX_PACKET_SIZE;
    	CIRCLEQ_INSERT_TAIL(headp, p, link);
	}
}


void print_q(circlehead_t* headp, packet_t* p) {
    printf("the queue looks like this:\n");

    packet_t* np = p;
    // int i = 0;
    // while (CIRCLEQ_NEXT(np, link) != p){
    //     printf("%d %d\n", i, np->state);
    //     knp = CIRCLEQ_NEXT(np, link);
    //     usleep(200000);
    //     i++;
    // }
    for (int i = 0; i < SWS; i++)
    {
        printf("%d %d %d\n", i, np->state, np->pstart);
        np = CIRCLEQ_LOOP_NEXT(headp, np, link);
        
    }
    
    printf("end of the queue\n");

}

void free_q(circlehead_t* headp){
  
	struct packet *next, *first = CIRCLEQ_FIRST(headp);

	while (first != (void *)headp) {
		next = CIRCLEQ_NEXT(first, link);
		free(first);
		first = next;
	}
}
