// Dawid Sroka, 317239

#include "header.h"

#define BUF_SIZE 100


int main(int argc, char* argv[]) {


// INPUT VALIDATION

	in_validate( argc, argv);

// ESTABLISH CONNECTION

	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd = -1, s;

	/* Obtain address(es) matching host/port. */

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
		Try each address until we successfully connect(2).
		If socket(2) (or connect(2)) fails, we (close the socket
		and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
					rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

		close(sfd);
	}

	freeaddrinfo(result);           /* No longer needed */

	if (rp == NULL || sfd == -1) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}


// At this point we have a good sfd and we are connected

// SENDING 


	send(sfd, "GET 0 10\n", 9, 0);

// RECEIVING


    int outfd = open(argv[3] ,O_WRONLY);

	close(sfd);
    close(outfd);

	return EXIT_SUCCESS;

}