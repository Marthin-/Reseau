#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define max_char 500


int main(int argc, char const *argv[])
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