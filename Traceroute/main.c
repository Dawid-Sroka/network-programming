// Dawid Sroka, 317239

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#include "header.h"


uint16_t compute_icmp_checksum (const void *buff, int length);
int decode(u_int8_t* buffer, int id, int ttl);
void print_reply_addrs(uint32_t senders_ip_raw[], int senders_bitmap[]);

u_int8_t reps_buffer[3][IP_MAXPACKET];	// buffer for received replies


int main(int argc, char* argv[]) {

	if ( argc != 2 ) {
		fprintf(stderr, "usage: %s host_address\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// create recipient
	struct sockaddr_in recipient;
	socklen_t recipient_len = sizeof(recipient);
	bzero (&recipient, recipient_len);
	recipient.sin_family = AF_INET;
	if ( inet_pton(AF_INET, argv[1], &(recipient.sin_addr.s_addr)) == 0) {
		fprintf(stderr, "'%s' is not a valid ip address\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	// identify self
	int pid = getpid();

	// create raw socket
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}


	// -- MAIN ROUTINE --
	int ttl_rounds = 30;
	int end_flag = 0;

	for (int i = 0; i < ttl_rounds; i++)
	{
		printf("%2d. ", i+1);

		// inject ttl
		int ttl = i+1;
		if ( setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0) {
			fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		// prepare time measurements
		struct timeval starts[3];
		struct timeval ends[3];
		double rtt[3] = {-1,-1,-1};	//         round trip time
		double avrtt;				// average rount trip time
		
		// -- SENDING --
		for (int j = 0; j < 3; j++)
		{
			// create ICMP message
			struct icmp header = {
				.icmp_type = ICMP_ECHO,
				.icmp_code = 0,
				.icmp_cksum = 0,
				.icmp_hun.ih_idseq.icd_id = pid,
				.icmp_hun.ih_idseq.icd_seq = 100*ttl + j,
			};
				
			uint16_t cksum = compute_icmp_checksum( (uint16_t*)&header, sizeof(header));
			header.icmp_cksum = cksum;
			
			// time stamp start
			if (gettimeofday(&starts[j], NULL) < 0) {
				fprintf(stderr, "gettimeofday error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}

			// send message
			ssize_t bytes_sent = sendto(sockfd, &header, sizeof(header),
										0, (struct sockaddr*)&recipient, sizeof(recipient));
			if(bytes_sent < 0) {
				fprintf(stderr, "sendto error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			} else if(bytes_sent != sizeof(header)) {
				fprintf(stderr, "sendto error: not all bytes sent\n");
				return EXIT_FAILURE;
			}

		}
	
		// response addresses buffer
		uint32_t senders_ip_raw[3];
		int senders_bitmap[3] = {0,0,0};	// auxillary
		
		// preparing for select
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (sockfd, &descriptors);
		struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;

		// -- RECEIVING --
		int cnt = 0;	// how many responses for current ttl came so far
		while (cnt < 3)
		{

			// select + recvfrom -> wait max 1s
			int ready = select (sockfd+1, &descriptors, NULL, NULL, &tv);
			if(ready == 0){	// timeout expired and no packet came
				break;
			} else if (ready < 0) {
				fprintf(stderr, "select error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}

			struct sockaddr_in sender;
			socklen_t sender_len = sizeof(sender);

			ssize_t packet_len = recvfrom (sockfd, reps_buffer[cnt], IP_MAXPACKET,
							MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
			if (packet_len < 0) {
				fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
				return EXIT_FAILURE;				
			}


			// check if the packet maches our pid and current ttl
			if(decode(reps_buffer[cnt], pid, ttl) == 0) {

				// note down the addresses 
				senders_ip_raw[cnt] = sender.sin_addr.s_addr;
				senders_bitmap[cnt] = 1;

				// check if it's a final echo reply
				if(sender.sin_addr.s_addr == recipient.sin_addr.s_addr) {
					end_flag = 1;
				}

				// time stamp end
				if (gettimeofday(&ends[cnt], NULL) < 0) {
					fprintf(stderr, "gettimeofday error: %s\n", strerror(errno));
					return EXIT_FAILURE;
				}
				rtt[cnt] = (ends[cnt].tv_sec - starts[cnt].tv_sec) * 1000.0;      // sec to ms
				rtt[cnt] += (ends[cnt].tv_usec - starts[cnt].tv_usec) / 1000.0;   // us to ms
				
				// update
				cnt += 1;
			}
			else {
				continue;
			}

		}

		// -- PRINT OUTPUT ACCORDINGLY --
		if(cnt > 0) {
			print_reply_addrs(senders_ip_raw, senders_bitmap);
		} else{
			printf("*\n");
			continue;
		}

		if(rtt[0] > 0 && rtt[1] > 0 && rtt[2] > 0) {	// calc avrtt only if all replies came
			avrtt = ( rtt[0] + rtt[1] + rtt[2] ) / 3;
			printf("%.2fms\n", avrtt);
		} else {
			printf("???\n");
		}

		if (end_flag == 1)
			break;
	}

	return EXIT_SUCCESS;
}
