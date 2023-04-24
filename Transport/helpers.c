#include "header.h"


#define MAX_IP_LEN 15	// valid ip address is max 15 characters long


void input_validate(int argc, char* argv[]) {
    if ( argc != 5 ) {
		fprintf(stderr, "usage: %s IPaddr port filename size\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    int outfd = open(argv[3] ,O_WRONLY);
    if (outfd == -1) {
		fprintf(stderr, "open error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
    }

	close(outfd);

	char addrbuf[MAX_IP_LEN];

	if ( strlen(argv[1]) > MAX_IP_LEN || inet_pton(AF_INET, argv[1], addrbuf) == 0) {
		fprintf(stderr, "'%s' is not a valid ip address\n", argv[1]);
		exit(EXIT_FAILURE);
	}
}

int how_long(int a) {
	char no[11];
    snprintf(no, 11, "%d", a);
	return strlen(no);
}


// int establish_connection(addr, port) {

// }