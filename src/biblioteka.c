

#include <string.h>
#include <stdio.h>
#include "biblioteka.h"
struct pakiet *adres_zakotwiczenia = NULL;
struct pakiet *obecny_adres = NULL;
struct pakiet *ostatni = NULL;
struct pakiet *temp;

void start(){
printf("######################################\n #\n#\Autor: Przemysław Sarnacki        #\n");
printf("######################################\n");
}
//Funkcja liczenia sumy kontrolnej
unsigned short chsum(unsigned short *ptr, int nbytes) {
	register long sum;
	u_short oddbyte;
	register u_short answer;

	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}

	if (nbytes == 1) {
		oddbyte = 0;
		*((u_char *) &oddbyte) = *(u_char *) ptr;
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;

	return (answer);
}

int set_ip(struct iphdr *ip,int packet_size, struct icmphdr *icmp){
	//if (argc < 5) {
		ip->tos = 0;
		ip->frag_off = 0;
		ip->ttl = 255;
		icmp->type = ICMP_ECHO;
		icmp->code = 0;
		icmp->un.echo.id = rand();
	//} else if (strcmp(argv[4], "-a") == 0) {
		/*printf("Podaj TOS: ");
		scanf("%d", &ip->tos);
		printf("Fragment offset: ");
		scanf("%d", &ip->frag_off);
		printf("Podaj TTL (0-255): ");
		scanf("%d", &ip->ttl);
		printf("Podaj typ ICMP: ");
		scanf("%d", &icmp->type);
		printf("Podaj kod ICMP: ");
		scanf("%d", &icmp->code);
		printf("Podaj id pakietu ICMP: ");
		scanf("%d", &icmp->un.echo.id);

	}
*/
	icmp->un.echo.sequence = rand();
	icmp->checksum = 0;
	ip->version = 4;
	ip->id = rand();
	ip->protocol = IPPROTO_ICMP;
	ip->tot_len = htons(packet_size);
	ip->ihl = 5;//domyślna długość nagłówka
	ip->check = chsum((u16 *) ip, sizeof(struct iphdr));
return(1);
}

void create_linked(struct iphdr *ip, struct icmphdr *icmp) {
	if(adres_zakotwiczenia == NULL){
		adres_zakotwiczenia = (struct pakiet*) malloc(sizeof(struct pakiet));
		obecny_adres = adres_zakotwiczenia;
		obecny_adres->pierwszy = adres_zakotwiczenia;
		obecny_adres->poprzedni = NULL;
		obecny_adres->nastepny = NULL;
		memcpy(&(obecny_adres->ip),ip,sizeof(struct iphdr));
		memcpy(&(obecny_adres->icmp),icmp,sizeof(struct icmphdr));
		obecny_adres = NULL;
	}else{
		ostatni = adres_zakotwiczenia;
		for(int n = 0 ; n < 10 ; ++n){
			if(ostatni->nastepny != NULL){
				ostatni = ostatni->nastepny;
			}else{
				break;
			}
		}
		obecny_adres = (struct pakiet*) malloc(sizeof(struct pakiet));
		ostatni->nastepny = obecny_adres;
		obecny_adres->pierwszy = adres_zakotwiczenia;
		obecny_adres->poprzedni = ostatni;
		obecny_adres->nastepny = NULL;

		memcpy(&(obecny_adres->ip),ip,sizeof(struct iphdr));
		memcpy(&(obecny_adres->icmp),icmp,sizeof(struct icmphdr));
	}
}


