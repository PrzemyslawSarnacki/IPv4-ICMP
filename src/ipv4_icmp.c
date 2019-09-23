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
#include "biblioteka.h"

typedef unsigned char u8;
typedef unsigned short int u16;
void *Biblioteka; // wskaznik do bilbioteki


int main(int argc, char **argv) {

	Biblioteka = dlopen("/media/sf_C/IPv4+ICMP/IPv4-ICMP/src/biblioteka.so", RTLD_NOW);
	  	if (!Biblioteka) {
	  		printf("Error otwarcia: %s\n", dlerror());
	  		return (1);
	  	}

	  	typedef void (*Funkcja1)();
	  	Funkcja1 start = (Funkcja1)dlsym(Biblioteka, "start");
	  	start();



	if (argc < 4) {
		printf(
				"\nUżycie: %s <źródłowy IP> <docelowy IP> [interfejs(0)] || [-a] [ilosc pakietow], spróbuj ponownie.\n",
				argv[0]);
		exit(0);
	}
	//printf("Argument %d", *argv[4]);

	unsigned long daddr = inet_addr(argv[2]);
	unsigned long saddr = inet_addr(argv[1]);

	int payload_size = 0, sent=0, sent_size, sent_total=0;

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);


	if (sockfd < 0) {
		perror("błąd otwarcia socketu, użyj sudo");
		return (0);
	}


	int on = 1;

	// dolaczenie naglowka IP
	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (const char*) &on,
			sizeof(on)) == -1) {
		perror("błąd generowania nagłówka ip");
		return (0);
	}

	//wysylanie do adresow broadcast
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char*) &on,
			sizeof(on)) == -1) {
		perror("błąd uprawnień");
		return (0);
	}

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

				memset(&ifr, 0, sizeof(ifr));
				snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), argv[3]);
				if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
					perror("błąd wybierania interfejsu, użyto domyślnego");
				}

	//struktura naglowka ip
	struct iphdr *ip = (struct iphdr *) packet;
	struct icmphdr *icmp = (struct icmphdr *) (packet + sizeof(struct iphdr));

	//wypełnia pamięć bajtem
	memset(packet, 0, packet_size);
	//printf("\nPodaj ilość pakietów do wysłania: ");
		//scanf("%d", &sent_total);
    ip->saddr = saddr;
	ip->daddr = daddr;
	set_ip(ip,packet_size,icmp);

	struct sockaddr_in servaddr;
	//servaddr.sin_addr.s_addr= inet_addr("10.0.2.15");
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = saddr;
	servaddr.sin_addr.s_addr = daddr;
	memset(&servaddr.sin_zero, 0, sizeof(servaddr.sin_zero));
    sent_total=*argv[4];
    sent_total=sent_total-48;
	printf("\n\tWYSYŁANIE...\n");
    create_linked(ip,icmp);

	while (sent < sent_total) {
		memset(packet + sizeof(struct iphdr) + sizeof(struct icmphdr),
				rand() % 255, payload_size);

		//przeliczenia sumy kontorlnej nag. icmp przy uzupenianiu payload randem
		icmp->checksum = 0;
		icmp->checksum = chsum((unsigned short *) icmp,
				sizeof(struct icmphdr) + payload_size);

		if ((sent_size = sendto(sockfd, packet, packet_size, 0,
				(struct sockaddr*) &servaddr, sizeof(servaddr))) < 1) {
			perror("błąd wysyłania\n");
			break;
		}
		++sent;
		printf("\t%d pakietów wysłanych\r", sent);
		printf("\n\t%d total\r", sent_total);
		fflush(stdout);

		usleep(1000000);  //mikrosekundy
	}

	printf("\tWYSŁANO %d PAKIET(Y/ÓW)\n\n", sent_total);

	free(packet);
	close(sockfd);

	return (0);
}

