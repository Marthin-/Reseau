#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SIZE_TAB 4
#define SIZE_IP 12
#define max_char 500

void catch ();
int serveur();
void tableau_ip();

typedef void (*sighandler_t)(int);

struct sockaddr_in  nom_sdemande;
struct sockaddr_in from;

void catch()
{
	exit(2);
}

int *tableau_sock(int *tableau_sock, int sdemande, int compteur_sock)
{
	int j;
	tableau_sock[compteur_sock]=sdemande;
	for (j = 0;  j<= compteur_sock; j++)
	{
		printf("adresse joueur %d: %d\n", compteur_sock, tableau_sock[compteur_sock]);
	}
	return tableau_sock[compteur_sock];
}

int serveur()
{
	int i;
	char buffer[max_char];
	int taille_buff;
	int listen_socket, status;
	struct sockaddr_in client_address;
	struct addrinfo hints, *servinfo;
	socklen_t client_address_length = sizeof client_address;
	char * port_listen = "2842";

	memset(&client_address, 0, sizeof client_address);
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = INADDR_ANY;
	client_address.sin_port = htons(6667);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port_listen, &hints, &servinfo)) != 0)
	{
    	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    	exit(EXIT_FAILURE);
    }

	if ((listen_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0)
	{
		perror("socket");
		return -1;
	}

	if (bind(listen_socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
    	close(listen_socket);
    	perror("bind:");
    	exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);
  	printf("Attente de requête sur le port %d\n", client_address.sin_port);

while(1)
{
  	memset(buffer, 0, sizeof buffer);
  	if(recvfrom(listen_socket, buffer, sizeof buffer, 0,(struct sockaddr *) &client_address, &client_address_length) == -1)
  	{
  		perror("recvfrom");
  		close(listen_socket);
  		exit(EXIT_FAILURE);
  	}

  	taille_buff = strlen(buffer);
  	if (taille_buff > 0)
  	{
  	  printf(">>  depuis %s", inet_ntoa(client_address.sin_addr));

      // Affichage du numéro de port du client.
      printf(":%hu\n", ntohs(client_address.sin_port));

      // Affichage de la ligne reçue
      printf("  Message reçu : %s\n", buffer);

      // Conversion de cette ligne en majuscules.
      for (i = 0; i < taille_buff; i++)
        buffer[i] = toupper(buffer[i]);

      // Renvoi de la ligne convertie au client.
      if (sendto(listen_socket, buffer, taille_buff, 0,
                 (struct sockaddr *) &client_address, sizeof(client_address)) == -1)
    	{
        	perror("sendto:");
        	close(listen_socket);
    		exit(EXIT_FAILURE);	
  		}
  	}
}
return 0;
}

void serveurbrodcast()
{
int sdemande;
int i;
char buf[256];
u_char *p;
int nc, fromlen;
struct hostent * hp;
char * ip_j;


signal(SIGINT,(sighandler_t)catch);

if ( (sdemande = socket(AF_INET, SOCK_DGRAM, 0) ) <0){
	perror("socket de demande");
	exit(1);
}

nom_sdemande.sin_family = AF_INET;
nom_sdemande.sin_addr.s_addr= htonl(INADDR_ANY);
nom_sdemande.sin_port=htons(5011);

if (bind ( sdemande, (const struct sockaddr *)&nom_sdemande, sizeof(nom_sdemande))<0){
	perror("bind ");
	exit(1);
}

while (1)
{
	fromlen= sizeof (from);
	nc=recvfrom(sdemande, buf, sizeof ( buf), 0,(struct sockaddr *)&from, &fromlen);
	if (nc <= 0){
  	perror("recvfrom");
  	exit(1);
	}

	printf("serveur recoit:");
	buf[nc]='\0';printf("%s\n",buf);
	hp=gethostbyaddr( &from.sin_addr,sizeof(from.sin_addr) , AF_INET);
	printf("...de la part de %s(%d)\n",inet_ntoa(from.sin_addr),htons(from.sin_port));
}	
}


int main()
{

	int val = 0;
	printf("1 ou 2\n");
	scanf("%d", &val);
	if(val == 1)
	{
		serveurbrodcast();
	}
	if(val == 2)
	{
		serveur();
	}
}

