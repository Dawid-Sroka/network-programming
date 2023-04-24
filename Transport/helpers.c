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


int establish_connection(char* addr, char* port) {
	
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sockfd = -1;

	/* Obtain address(es) matching host/port. */

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(addr, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
		Try each address until we successfully connect(2).
		If socket(2) (or connect(2)) fails, we (close the socket
		and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype,
					rp->ai_protocol);
		if (sockfd == -1)
			continue;

		if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

		close(sockfd);
	}

	freeaddrinfo(result);           /* No longer needed */

	if (rp == NULL || sockfd == -1) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	
	return sockfd;
}


int how_long(int a) {
	char no[11];
    snprintf(no, 11, "%d", a);
	return strlen(no);
}


// int establish_connection(addr, port) {

// }