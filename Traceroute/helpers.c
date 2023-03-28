// Dawid Sroka, 317239

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "header.h"

uint16_t compute_icmp_checksum (const void *buff, int length) {
	uint32_t sum;
	const uint16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (uint16_t)(~(sum + (sum >> 16)));
}

int safe_ntop(uint32_t* raw, char* dst) {
	if ( inet_ntop(AF_INET, raw, dst, 20) == NULL){
		fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
		return EXIT_FAILURE;	
	} else
		return 1;
}

void print_reply_addrs(uint32_t senders_ip_raw[], int senders_bitmap[]) {

	// char *str0, *str1, *str2;
	char str0[20], str1[20], str2[20];


	if (senders_bitmap[0] == 1)
		safe_ntop(&senders_ip_raw[0],str0);
	if (senders_bitmap[1] == 1)
		safe_ntop(&senders_ip_raw[1],str1);
	if (senders_bitmap[2] == 1)
		safe_ntop(&senders_ip_raw[2],str2);

	char reply_addrs[63];
	reply_addrs[0] = '\0';


	if(senders_bitmap[1] == 0) {		// only one came
		strcat(reply_addrs, str0);
	}
	else if (senders_bitmap[2] == 0) {	// two came
		if(senders_ip_raw[0] == senders_ip_raw[1])
			strcat(reply_addrs, str0);
		else {
			strcat(reply_addrs, str0);
			strcat(reply_addrs, " ");
			strcat(reply_addrs, str1);
		}
	}
	else {
		if(senders_ip_raw[0] == senders_ip_raw[1]){
			strcat(reply_addrs, str0);
			if(senders_ip_raw[1] != senders_ip_raw[2]) {
				strcat(reply_addrs, " ");
				strcat(reply_addrs, str2);
			}
		}
		else {
			strcat(reply_addrs, str0);
			strcat(reply_addrs, " ");
			strcat(reply_addrs, str1);
			if(senders_ip_raw[0] != senders_ip_raw[2] && senders_ip_raw[1] != senders_ip_raw[2]) {
				strcat(reply_addrs, " ");
				strcat(reply_addrs, str2);
			}
		}
	}

	strcat(reply_addrs, "  ");
	printf("%s", reply_addrs);
}


