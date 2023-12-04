#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>




/*
    processus client : envoie une demande de connexion au serveur et qui receveras un entier
                       et l'afficheras à l'écrant
*/
int main(void){
    //creation de la socket
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //specifier une adresse pour la socket
    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(9000);   
    server_adress.sin_addr.s_addr = INADDR_ANY;

    /*   entier qui recevras l'etat de la connexion connect() renvoie 0 s'il réussit, ou -1 s'il échoue, auquel  cas  errno
         contient le code d'erreur.
    */
    int connection_status = connect(network_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));

    /*
    tester en cas d'erreurs de connexion
    */

   /*
   reception des données du serveur
   */
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);
    char buffer[256] = "";
    read(network_socket, buffer, sizeof(buffer));
    printf("%s", buffer);
    

    /*
    afficher la réponse du serveur
    */

    printf("le serveur renvoie : %s\n", server_response);

    /*
    fermer la socket    
    */    
   close(network_socket);

   return 0;
}