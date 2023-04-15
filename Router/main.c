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

#define DEBUG
#undef DEBUG
#ifdef DEBUG
#define debug(...) fprintf(__VA_ARGS__)
#else
#define debug(...)
#endif

// #include "header.h"

void read_input() {
    int n;
    scanf("%d",&n);
    debug(stderr, "[%p] %d\n", &n, n);

    for (int i = 0; i < n; i++)
    {
        char *save_ptr = NULL;

        char net[20];
        scanf("%s", net);

        char *addr_tok = strtok_r(net, "/", &save_ptr);
        char *mask_tok = strtok_r(NULL, "/", &save_ptr);

        char addr[16];
        strcpy(addr, addr_tok);
        char mask[2];
        strcpy(mask, mask_tok);

        debug(stdout, "[%p] %s\n", addr, addr);
        debug(stdout, "[%p] %s\n", mask, mask);

        char pass[8];
        int dist;
        scanf("%s %d", pass, &dist);

        debug(stdout, "[%p] %d\n", &dist, dist);
    }
}



int main() {

    read_input();
    

	return EXIT_SUCCESS;
}