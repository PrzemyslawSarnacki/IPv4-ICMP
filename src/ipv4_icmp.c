
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
#include "biblioteka.h"

typedef unsigned char u8;
typedef unsigned short int u16;
void *Biblioteka; // wskaznik do bilbioteki

unsigned short chsum(unsigned short *ptr, int nbytes);

int main(int argc, char **argv) {

	Biblioteka = dlopen("/biblioteka.so", RTLD_NOW);
	  	if (!Biblioteka) {
	  		printf("Error otwarcia: %s\n", dlerror());
	  		return (1);
	  	}

	  	typedef void (*Funkcja1)();
	  	Funkcja1 start = (Funkcja1)dlsym(Biblioteka, "start");
	  	start();




	unsigned long daddr = inet_addr(argv[2]);
	unsigned long saddr = inet_addr(argv[1]);

	int payload_size = 0, sent, sent_size, sent_total = 1000000;

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	//calkowita wielkosc pakietu
	int packet_size = sizeof(struct iphdr) + sizeof(struct icmphdr)
			+ payload_size;
	char *packet = (char *) malloc(packet_size);

	if (!packet) {
		perror("błąd alokowania pamięci");
		close(sockfd);
		return (0);
	}

	struct ifreq ifr;



	//struktura naglowka ip
	struct iphdr *ip = (struct iphdr *) packet;
	struct icmphdr *icmp = (struct icmphdr *) (packet + sizeof(struct iphdr));

	//wypełnia pamięć bajtem
	memset(packet, 0, packet_size);


	ip->version = 4;
	ip->id = rand();
	ip->protocol = IPPROTO_ICMP;
	ip->saddr = saddr;
	ip->daddr = daddr;
	ip->tot_len = htons(packet_size);
	ip->ihl = 5;
	ip->check = chsum((u16 *) ip, sizeof(struct iphdr));
	icmp->un.echo.sequence = rand();
	icmp->checksum = 0;

	return (0);
}

