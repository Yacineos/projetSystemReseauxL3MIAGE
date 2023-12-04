#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>




/*
    processus client : envoie une demande de connexion au serveur et qui receveras un entier
                       et l'afficheras à l'écrant
*/
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments : 2 arguments needed (int : Port, any : Request) \n");
        exit(1);
    } else if (atoi(argv[1]) < 9000 || atoi(argv[1]) > 9010) {
        printf("Wrong argument value : Second argument must be an integer between 9000 and 9010 is expected\n");
        exit(1);
    }

    char buffer[256] = "";

    //creation de la socket
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1) {
        printf("First socket creation error on server side");
        exit(1);
    } 

    //specifier une adresse pour la socket
    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(atoi(argv[1]));
    server_adress.sin_addr.s_addr = INADDR_ANY;

    /*   entier qui recevra l'etat de la connexion connect() renvoie 0 s'il réussit, ou -1 s'il échoue, auquel  cas  errno
         contient le code d'erreur.
    */
    int connection_status = connect(network_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));
    
    if (connection_status == -1) {
        printf("Connection to server error on client side");
        exit(1);
    }

   /*
   reception des données du serveur
   */
    memset(buffer, 0, 256);
    read(network_socket, buffer, sizeof(buffer));
    printf("le serveur renvoie : %s\n", buffer);
    memset(buffer, 0, 256);
    strcpy(buffer, argv[2]);
    write(network_socket, buffer, sizeof(buffer));
    memset(buffer, 0, 256);
    read(network_socket, buffer, sizeof(buffer));
    printf("%s\n", buffer);
    

    /*
    afficher la réponse du serveur
    */

    /*
    fermer la socket    
    */    
   close(network_socket);

   return 0;
}