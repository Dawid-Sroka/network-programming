// Dawid Sroka, 317239

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <byteswap.h>

#define DEBUG
#undef DEBUG
#ifdef DEBUG
#define debug(...) fprintf(__VA_ARGS__)
#else
#define debug(...)
#endif

// #include "header.h"

// Routing table entry
struct Entry {
    uint8_t used;
    char network[20];
    char brd[16];
    int dist;
    char via[16];
};

struct Entry empty_entry = {.used = 0, .network = "", .brd = "", .dist = 0, .via = "" };

// Routing table
struct Entry Rtab[10]; // assume there are no more than 10 routers in the world 


int safe_ntop(uint32_t* raw, char* dst) {
	if ( inet_ntop(AF_INET, raw, dst, 20) == NULL){
		fprintf(stdout, "inet_ntop error: %s\n", strerror(errno));
		return EXIT_FAILURE;	
	} else
		return 1;
}

void print_entries(){
    int free = 0;
    fprintf(stdout, "network             broadcast       dist via\n" );

    while (Rtab[free].used == 1) {
        fprintf(stdout, "%-19s ", Rtab[free].network);
        fprintf(stdout, "%-15s ", Rtab[free].brd);
        fprintf(stdout, "%-4d ", Rtab[free].dist);
        fprintf(stdout, "%s\n", Rtab[free].via);
        free++;
    }
}

void add_entry(char *net, int dist, char *via) {
    int free = 0;
    while (Rtab[free].used == 1)
        free++;

    Rtab[free].used = 1;
    strcpy(Rtab[free].network, net);
    Rtab[free].dist = dist;
    strcpy(Rtab[free].via, via);

    char *save_ptr = NULL;
    char *net_addr_p = strtok_r(net, "/", &save_ptr);
    uint32_t net_addr_n;

	if ( inet_pton(AF_INET, net_addr_p, &net_addr_n) == 0) {
		fprintf(stdout, "'%s' is not a valid ip address\n", net_addr_p);
		exit(EXIT_FAILURE);
	}

    char *mask_len = strtok_r(NULL, "/", &save_ptr);
    uint8_t k = strtol(mask_len, NULL, 10);
    uint32_t mask = ((1 << k) - 1 ) << (32 - k);
    net_addr_n = __bswap_32(net_addr_n);
    uint32_t brd_addr_n = net_addr_n | ( ~ mask );
    brd_addr_n = __bswap_32(brd_addr_n);
    // uint32_t brd_addr_n = (net_addr_n & mask) | ((1 << (32 - k)) - 1);

    safe_ntop(&brd_addr_n, Rtab[free].brd);
}

void read_input() {
    int n;  // number of interfaces
    scanf("%d",&n);
    debug(stdout, "[%p] %d\n", &n, n);

    for (int i = 0; i < n; i++)
    {
        char net[20];
        scanf("%s", net);

        char pass[20];
        int dist;
        scanf("%s %d", pass, &dist);
        debug(stdout, "%s\n", net);
        debug(stdout, "%d\n", dist);


        add_entry(net,dist,net);
        debug(stdout, "after add%d\n", dist);




        // char *save_ptr = NULL;
        // char *addr_tok = strtok_r(Rtab[i].network, "/", &save_ptr);
        // strcpy(Rtab[i].brd, addr_tok);
        // char *mask_tok = strtok_r(NULL, "/", &save_ptr);

        // char addr[16];
        // strcpy(addr, addr_tok);
        // char mask[2];
        // strcpy(mask, mask_tok);

        // debug(stdout, "[%p] %s\n", addr, addr);
        // debug(stdout, "[%p] %s\n", mask, mask);

        // char pass[8];
        // scanf("%s %d", pass, &(Rtab[i].dist));



        // debug(stdout, "[%p] %d\n", &(Rtab[i].dist), Rtab[i].dist);
    }

}



int main() {

    for (int i = 0; i < 10; i++)
        Rtab[i] = empty_entry;


    read_input();
    
    print_entries();

	return EXIT_SUCCESS;
}