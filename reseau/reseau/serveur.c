#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


int cree_socket_stream (const char * nom_hote, const char * nom_service, const char * nom_proto)

{
	int sock;
	struct sockaddr_in adresse;
	struct hostent * hostent;
	struct servent * servent;
	struct protoent * protoent;

	if ((hostent = gethostbyname(nom_hote)) == NULL)
	{
		perror ("gethostbyname");
		return -1;
	}
	if ((protoent = getprotobyname(nom_proto)) == NULL)
	{
		perror("getprotobyname");
		return -1;
	}
	if ((servent = getservbyname(nom_service, protoent->p_name)) == NULL)
	{
		perror("getprotobyname");
		return -1;
	}
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}
	memset(& adresse, 0, sizeof (struct sockaddr_in));
	adresse.sin_family = AF_INET;
	adresse.sin_port = servent->s_port;
	adresse.sin_addr.s_addr = ((struct in_addr *) (hostent->h_addr))->s_addr;
	if (bind(sock, (struct sockaddr *) & adresse,sizeof(struct sockaddr_in)) < 0)
	{
		close(sock);
		perror("bind");
		return -1;
	}
	return sock;
}

int affiche_adresse_socket (int sock)
{
	struct sockaddr_in adresse;
	socklen_t longueur;

	longueur = sizeof(struct sockaddr_in);
	if(getsockname(sock, & adresse, & longueur) < 0)
	{
		perror("getsockname");
		return -1;
	}
	fprintf(stdout, "IP = %s, PORT = %u \n", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
	return 0;
}
int serveur (void)
{
	int sock_contact;
	int sock_connectee;
	struct sockaddr_in adresse;
	socklen_t longueur;

	sock_contact = cree_socket_stream(NULL, NULL, "UDP");
	if (sock_contact < 0)
		return -1;
	listen(sock_contact, 5);
	fprintf(stdout, "Mon adresse >>\n");
	affiche_adresse_socket(sock_contact);
	while(! quitter())
	{
		longueur = sizeof(struct sockaddr_in);
		sock_connectee = accept(sock_contact, (struct sockadrr *)& adresse, & longueur);
		if(sock_connectee < 0)
		{
			perror ("accpet");
			return -1;
		}
		switch (fork())
		{
			case 0:
			close(sock_contact);
			connexion(sock_connectee);
			exit(EXIT_SUCCESS);
			case -1:
			perror("fork");
			return -1;
			default:
			close(sock_connectee);
		}
	}
	return 0;
}

void connexion (int sock)
{
	struct sockaddr_in adresse;
	socklen_t longueur;
	char	buffer [256];

	longueur = sizeof(struct sockaddr_in);
	if (getpeername(sock, (struct sockaddr *) & adresse, & longueur) < 0)
	{
		perror("getpeername");
		return;
		sprintf(buffer, "IP = %s, Port = %u \n", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
		fprintf(stdout, "	distante %s", buffer);
		write(sock, "votre adresse :", 16);
		write(sock, buffer, strlen(buffer));
		close(sock);
	}
}

int main(int argc, char const *argv[])
{
	
	return serveur();
}