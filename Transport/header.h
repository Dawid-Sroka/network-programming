#ifndef HEADER_H
#define HEADER_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/queue.h>


struct packet;

// I will use CIRCLEQ implementations from C queue library
typedef struct packet {
	uint8_t state;
	int pstart;
	CIRCLEQ_ENTRY(packet) link;           /* Queue */
} packet_t ;

struct circlehead;
typedef CIRCLEQ_HEAD(circlehead, packet) circlehead_t;

// void print_q(packet_t* p);
void print_q(circlehead_t* head);





void input_validate(int argc, char* argv[]);
int establish_connection(char* addr, char* port);
int how_long(int a);


#endif // HEADER_H