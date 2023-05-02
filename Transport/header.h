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
#include <sys/param.h>

#define SWS 1000
#define RTT_us 80000
#define MAX_PACKET_SIZE 1000
#define MAX_IP_LEN 15	// valid ip address is max 15 characters long

// variables

// I will use CIRCLEQ implementations from C queue library
struct packet;

typedef struct packet {
	uint8_t state;
	int pstart;
	CIRCLEQ_ENTRY(packet) link;           /* Queue */
} packet_t ;

struct circlehead;
typedef CIRCLEQ_HEAD(circlehead, packet) circlehead_t;

void print_q(circlehead_t* headp, packet_t* p);
void initiailze_q(circlehead_t* headp);
void free_q(circlehead_t* headp);
struct packet* shift_window_q(circlehead_t* headp, struct packet* fst, int expects, int* idx);

// helper functions
void input_validate(int argc, char* argv[]);
int establish_connection(char* addr, char* port);
int how_long(int a);


#endif // HEADER_H
