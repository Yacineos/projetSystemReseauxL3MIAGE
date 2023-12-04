#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

/* Creation de la socket, attribution de son adresse */
int client_socket_init(int port) {
    //creation de la socket
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1) {
        printf("First socket creation error on server side");
        exit(1);
    } 

    //specifier une adresse pour la socket
    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(port);
    server_adress.sin_addr.s_addr = INADDR_ANY;

    /*   entier qui recevra l'etat de la connexion connect() renvoie 0 s'il réussit, ou -1 s'il échoue, auquel  cas  errno
         contient le code d'erreur. */
    int connection_status = connect(network_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));
    
    if (connection_status == -1) {
        printf("Connection to server error on client side");
        exit(1);
    }
    return network_socket;
}


// Gestion de la communication avec le serveur
int communication_to_server(int socket, char *request){

    char buffer[256] = "";

    // Reception des données du serveur
    memset(buffer, 0, 256);
    read(socket, buffer, sizeof(buffer));
    printf("le serveur renvoie : %s\n", buffer);
    memset(buffer, 0, 256);
    strcpy(buffer, request);
    write(socket, buffer, sizeof(buffer));
    memset(buffer, 0, 256);
    read(socket, buffer, sizeof(buffer));
    printf("%s\n", buffer);

    return 0;
}


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
    int socket = client_socket_init(atoi(argv[1]));
    if (communication_to_server(socket, argv[2]) !=0) {
        printf("Erreur de communication avec le serveur.");
        exit(1);
    }
    /*
    fermer la socket    
    */    
   close(socket);

   return 0;
}