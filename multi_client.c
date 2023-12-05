#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
// Les include pour traiter le signal
#include <sys/wait.h>
#include <newlib/sys/select.h>
#include <signal.h>
#include <arpa/inet.h>


void end_child(){
    wait(NULL);
}

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
    server_adress.sin_addr.s_addr = inet_addr("im2ag-204-09.u-ga.fr");

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

int signal_setup(){
    // déclaration d'une structure qui contient les actions à effectuer à la reception du signal
    struct sigaction ac;
    // affectation des valeurs
    ac.sa_handler = end_child;
    ac.sa_flags = SA_RESTART;

    // execution du handler au moment où on reçoit SIGCHLD
    if (sigaction(SIGCHLD, &ac, NULL) != 0) {
        perror("Sigaction failed");
        exit(1);
    }
    return 0;
}

// Client père principal qui handle les clients fils (requesters) sans emettre de requête serveur
int multiple_client_requests(int nb_clients, int port, char* request) {
    int i = 0;
    while (i < nb_clients) {
        switch (fork()) {
            case -1:
                perror("Fork failed");
                exit(1);
            case 0:
                int inner_socket = client_socket_init(port);
                if (communication_to_server(inner_socket, request) != 0) {
                    printf("Erreur de communication serveur avec le fils %d\n", i);
                    exit(1);
                }
                close(inner_socket);
                printf("Fils numéro %d s'est exécuté correctement\n", i);
                exit(0);
            default:
                i++;
        }
    }
    return 0;
}
 

/*
    processus client : envoie une demande de connexion au serveur et qui receveras un entier
                       et l'afficheras à l'écrant
*/
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Wrong number of arguments : 3 arguments needed (int : nb of clients, int : port, any : Request) \n");
        exit(1);
    }

    if (multiple_client_requests(atoi(argv[1]), atoi(argv[2]), argv[3]) != 0) {
        printf("Erreur lors de la mise en place des clients multiples.");
        exit(1);
    }

   return 0;
}