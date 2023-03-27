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

#define DEBUG
#undef DEBUG
#ifdef DEBUG
#define debug(...) fprintf(__VA_ARGS__)
#else
#define debug(...)
#endif

#define DEBUG_FLOW
#undef DEBUG_FLOW
#ifdef DEBUG_FLOW
#define debug_flow(...) fprintf(__VA_ARGS__)
#else
#define debug_flow(...)
#endif

int decode(u_int8_t* buffer, int id, int ttl, char ip_str[]){
    (void)ip_str;

    struct ip* 	ip_header = (struct ip*) buffer;
    ssize_t		ip_header_len = 4 * ip_header->ip_hl;
    debug(stderr, "DEBUG ip_header_len = %ld\n", ip_header_len);

    u_int8_t* icmp_packet = buffer + ip_header_len;
    struct icmp* icmp_header = (struct icmp*) icmp_packet;
    
    debug(stderr, "DEBUG icmp_type = %d\n", icmp_header->icmp_type);
    debug(stderr, "DEBUG icmp_code = %d\n", icmp_header->icmp_code);
    debug(stderr, "DEBUG icmp_cksum = %u\n", icmp_header->icmp_cksum);




    if(icmp_header->icmp_type != 11 && icmp_header->icmp_type != 0) {
        debug_flow(stderr, "from %s that's not 'ttl exceeded' reply! \n", ip_str);
        return -1;
    }

    if(icmp_header->icmp_type == 0) {
        int reply_id = icmp_header->icmp_hun.ih_idseq.icd_id;
        if (reply_id == id){
            return 0;
        } else {
            debug_flow(stderr, "from %s that's a reply, but not to our process!\n\n", ip_str);
            return -4;
        }
    }

    u_int8_t* icmp_inner = icmp_packet + 8;

    struct ip* 	orig_ip_header = (struct ip*) icmp_inner;
    ssize_t		orig_ip_header_len = 4 * orig_ip_header->ip_hl;

    debug(stderr, "DEBUG orig_ip_header_len = %ld\n", orig_ip_header_len);

    u_int8_t* orig_icmp_packet = (u_int8_t*)orig_ip_header + orig_ip_header_len;
    struct icmp* orig_icmp_header = (struct icmp*) orig_icmp_packet;

    int orig_icmp_header_id = orig_icmp_header->icmp_hun.ih_idseq.icd_id;
    debug(stderr, "DEBUG orig_icmp_id = %d\n", orig_icmp_header_id);

    if(orig_icmp_header_id != id) {
        debug_flow(stderr, "from %s that's a ttl exceeded message, but not to our process!\n\n", ip_str);
        return -2;
    }

    int orig_icmp_header_seq = orig_icmp_header->icmp_hun.ih_idseq.icd_seq;
    debug(stderr, "DEBUG orig_icmp_seq = %d\n", orig_icmp_header_seq);
    
    int orig_ttl = orig_icmp_header_seq / 100; 

    if(orig_ttl != ttl) {
        debug_flow(stderr, "from %s that reply came late!\n\n", ip_str);
        return -3;
    } else {
        return 0;
    }

    debug(stderr, "DEBUG orig_icmp_type = %d\n", orig_icmp_header->icmp_type);
    debug(stderr, "DEBUG orig_icmp_code = %d\n", orig_icmp_header->icmp_code);
    debug(stderr, "DEBUG orig_icmp_cksum = %u\n", orig_icmp_header->icmp_cksum);
}