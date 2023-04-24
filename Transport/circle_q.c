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