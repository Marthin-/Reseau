#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

#define MAXIMUM_PORT 5
#define PORT_ARRAY_SIZE (MAXIMUM_PORT+1)
#define MAX_MSG 80
#define MAX 500
#define MSG_ARRAY_SIZE (MAX_MSG+1)

int client()
{
  int socketDescriptor, status;
  unsigned int msgLength;
  struct addrinfo hints, *servinfo;
  struct timeval timeVal;
  fd_set readSet;
  char msg[MSG_ARRAY_SIZE], serverPort[PORT_ARRAY_SIZE]; 

  puts("Entrez le nom du serveur ou son adresse IP : ");
  memset(msg, 0, sizeof msg);  // Mise à zéro du tampon
  scanf("%s", msg);hahahha

  printf("%s\n", msg);

  memset(serverPort, 0, sizeof serverPort);  // Mise à zéro du tampon
  serverPort[0]=2;
  serverPort[1]=8;
  serverPort[2]=4;
  serverPort[3]=2;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  if ((status = getaddrinfo(msg, serverPort, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  if ((socketDescriptor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
    perror("socket:");
    exit(EXIT_FAILURE);
  }

  puts("\nEntrez quelques caractères au clavier.");
  puts("Le serveur les modifiera et les renverra.");
  puts("Pour sortir, entrez une ligne avec le caractère '.' uniquement.");

  // Invite de commande pour l'utilisateur et lecture des caractères jusqu'à la
  // limite MAX_MSG. Puis suppression du saut de ligne en mémoire tampon.
  puts("Saisie du message : ");
  memset(msg, 0, sizeof msg);  // Mise à zéro du tampon
  scanf("%s", msg);

  // Arrêt lorsque l'utilisateur saisit une ligne ne contenant qu'un point
  while (strcmp(msg, ".")) {
    if ((msgLength = strlen(msg)) > 0) {
      // Envoi de la ligne au serveur
      if (sendto(socketDescriptor, msg, msgLength, 0,
                 servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        perror("sendto:");
        close(socketDescriptor);
        exit(EXIT_FAILURE);
      }

      // Attente de la réponse pendant une seconde.
      FD_ZERO(&readSet);
      FD_SET(socketDescriptor, &readSet);
      timeVal.tv_sec = 1;
      timeVal.tv_usec = 0;

      if (select(socketDescriptor+1, &readSet, NULL, NULL, &timeVal)) {
        // Lecture de la ligne modifiée par le serveur.
        memset(msg, 0, sizeof msg);  // Mise à zéro du tampon
        if (recv(socketDescriptor, msg, sizeof msg, 0) == -1)
        {
          perror("recv:");
          close(socketDescriptor);
          exit(EXIT_FAILURE);
        }

        printf("Message traité : %s\n", msg);
      }
      else {
        puts("Pas de réponse dans la seconde.");
      }
    }
    // Invite de commande pour l'utilisateur et lecture des caractères jusqu'à la
    // limite MAX_MSG. Puis suppression du saut de ligne en mémoire tampon.
    // Comme ci-dessus.
    puts("Saisie du message : ");
    memset(msg, 0, sizeof msg);  // Mise à zéro du tampon
    scanf("%s", msg);
  }

  close(socketDescriptor);

  return EXIT_SUCCESS;
}

int main(int argc, char const *argv[])
{
  client();
  return 0;
}
