// Les include génériques
 #include "stdio.h"
 #include "stdlib.h"
 #include "unistd.h"
 #include "stdbool.h"

// Les include pour traiter le signal
#include <sys/wait.h>
#include <newlib/sys/select.h>
#include <signal.h>

 // Include des fonctions et types fondamentaux  
 #include "sys/socket.h"
 #include "sys/types.h"

 // Include des macros
 #include "netinet/in.h"

// fonction qui sera appelé quand le signal SIGCHLD est reçu
// cette fonction enterine le fils 
void end_child(){
    wait(NULL);
}


// Serveur qui se met à l'écoute, reçoit une demande client, 
// et y répond par l'envoi du message de réponse.
int main(void){
    // Message qui signale au client la réussite de la connexion 
    char answer_msg[256] = "You have reached the server";

    // Création du socket d'écoute (socket())
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Création de la structure adresse du serveur (struct sockaddr_in)
    struct sockaddr_in sockaddr_server;
    sockaddr_server.sin_family = AF_INET;
    sockaddr_server.sin_port = htons(9002);
    sockaddr_server.sin_addr.s_addr = INADDR_ANY;


    // Association de la structure d'adresse et du socket d'écoute (bind())
    bind(server_socket, (struct sockaddr*) &sockaddr_server, sizeof(sockaddr_server));


    // déclaration d'une structure qui contient les 
    struct sigaction ac;
    // affectation des 
    ac.sa_handler = end_child;
    ac.sa_flags = SA_RESTART;

    // execution du handler au moment où on reçoit SIGCHLD
    sigaction(SIGCHLD, &ac, NULL);

    while (true) {
        // Attribution du rôle de socket d'écoute à notre socket (listen())
        // 5 => Taille max. de la file d'attente de connexion
        listen(server_socket, 5);

        // Création de la structure adresse du client (struct sockaddr_in)
        struct sockaddr_in sockaddr_client;

        // Création d'un socket de service commun client/serveur (accept())
        // Valeur de retour = numéro du descripteur de fichier du socket de service
        int serv_adress_size = sizeof(sockaddr_client);
        int client_socket = accept(server_socket,(struct sockaddr*) &sockaddr_client, &serv_adress_size);

        // Envoi d'un message confirmant la connexion au serveur
        write(client_socket, answer_msg, sizeof(answer_msg));
        
        switch(fork()) {
            case -1:
                perror("Fork failed.");
                exit(1);
            case 0:
                close(server_socket);
                char buffer[256] = "This is an answer to the request you sent";
                // Envoi du résultat de la requête client grâce au socket de service (send())
                write(client_socket, buffer, 256);

                // Fermeture de la socket de service (close())
                close(client_socket);
                exit(0);
            default:
        }
        // Fermeture de la socket de service (close())
        close(client_socket);
        printf("Request completed and child process is dead\n");
    }
    
    // Fermeture du socket d'écoute (close())
    close(server_socket);

    // Fin du programme serveur 
    return 0;
}