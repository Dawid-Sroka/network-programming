// Dawid Sroka, 317239

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include "header.h"

int decode(u_int8_t* buffer, int id, int ttl){

    struct ip* 	ip_header = (struct ip*) buffer;
    ssize_t		ip_header_len = 4 * ip_header->ip_hl;

    u_int8_t* icmp_packet = buffer + ip_header_len;
    struct icmp* icmp_header = (struct icmp*) icmp_packet;
    
    if(icmp_header->icmp_type != 11 && icmp_header->icmp_type != 0) {
        return -1;
    }

    if(icmp_header->icmp_type == 0) {
        int reply_id = icmp_header->icmp_hun.ih_idseq.icd_id;
        if (reply_id == id){
            return 0;
        } else {
            return -4;
        }
    }

    u_int8_t* icmp_inner = icmp_packet + 8;

    struct ip* 	orig_ip_header = (struct ip*) icmp_inner;
    ssize_t		orig_ip_header_len = 4 * orig_ip_header->ip_hl;


    u_int8_t* orig_icmp_packet = (u_int8_t*)orig_ip_header + orig_ip_header_len;
    struct icmp* orig_icmp_header = (struct icmp*) orig_icmp_packet;

    int orig_icmp_header_id = orig_icmp_header->icmp_hun.ih_idseq.icd_id;

    if(orig_icmp_header_id != id) {
        return -2;
    }

    int orig_icmp_header_seq = orig_icmp_header->icmp_hun.ih_idseq.icd_seq;
    
    int orig_ttl = orig_icmp_header_seq / 100; 

    if(orig_ttl != ttl) {
        return -3;
    } else {
        return 0;
    }

}