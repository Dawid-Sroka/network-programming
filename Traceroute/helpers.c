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

void print_reply_addrs(char senders_ip_str[]){
	char reply_addrs[60];
	reply_addrs[60] = '\0';
	if(senders_ip_str[0] != '\0') {
		strcat(reply_addrs, senders_ip_str);
	}
	if(senders_ip_str[20] != '\0' && senders_ip_str[20] != senders_ip_str[0] ) {
		strcat(reply_addrs, ", ");
		strcat(reply_addrs, senders_ip_str + 20);
	}
	if(senders_ip_str[40] != '\0' 			   && 
	   senders_ip_str[40] != senders_ip_str[0] &&
	   senders_ip_str[40] != senders_ip_str[20]  ) {
		strcat(reply_addrs, ", ");
		strcat(reply_addrs, senders_ip_str + 40);
	}
	strcat(reply_addrs, " ");
	printf("%s", reply_addrs);
}


