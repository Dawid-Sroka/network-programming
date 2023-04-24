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


void print_q(circlehead_t* head) {
    packet_t* np;
    int i = 0;
    CIRCLEQ_FOREACH(np, head, link) {

        printf("%d %d\n", i, np->state);

        usleep(200000);
        i++;
    }


}

// void print_q(packet_t* p) {
//     printf("the queue looks like this:\n");

//     packet_t* np = p;
//     int i = 0;
//     while (CIRCLEQ_NEXT(np, link) != p){
//         printf("%d %d\n", i, np->state);
//         np = CIRCLEQ_NEXT(np, link);
//         usleep(200000);
//         i++;
//     }
//     printf("end of the queue\n");

// }