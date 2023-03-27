#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>


uint16_t compute_icmp_checksum (const void *buff, int length);
int decode(u_int8_t* buffer, int id, int ttl, char ip_str[]);
