#ifndef BIBLOTEKA_H_
#define BIBLITEKA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/if.h>
#include <unistd.h>
#include <dlfcn.h>
#include <byteswap.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <ifaddrs.h>

typedef unsigned char u8;
typedef unsigned short int u16;
unsigned short chsum(unsigned short *ptr, int nbytes);
int set_ip(struct iphdr *ip, int packet_size, struct icmphdr *icmp);
void create_linked(struct iphdr *ip, struct icmphdr *icmp);
void start();
struct pakiet{
	struct pakiet *pierwszy;
	struct pakiet *nastepny;
	struct pakiet *poprzedni;

	struct icmphdr icmp;
	struct iphdr ip;


};



#endif

