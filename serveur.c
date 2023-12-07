/* Fichier qui contient le programme principal du serveur. Celui-ci ne se charge que d'exécuter les différents protocoles et fonctions qui ont été définis dans d'autres fichiers attitrés */

// Les include génériques
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <stdbool.h>
 #include <string.h>

// Les include pour traiter le signal


#include <sys/wait.h>
#include <newlib/sys/select.h>
#include <signal.h>

 // Include des fonctions et types fondamentaux  
 #include <sys/socket.h>
 #include <sys/types.h>

 // Include des macros
 #include <netinet/in.h>

// fonction qui sera appelé quand le signal SIGCHLD est reçu
// cette fonction enterine le fils 
void end_child(){
    wait(NULL);
}

int server_socket_init(int port){

    // Création du socket d'écoute (socket())
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("First socket creation error on server side");
        exit(1);
    } 

    // Création de la structure adresse du serveur (struct sockaddr_in)
    struct sockaddr_in sockaddr_server;
    sockaddr_server.sin_family = AF_INET;
    sockaddr_server.sin_port = htons(port);
    sockaddr_server.sin_addr.s_addr = INADDR_ANY;


    // Association de la structure d'adresse et du socket d'écoute (bind())
    if (bind(server_socket, (struct sockaddr*) &sockaddr_server, sizeof(sockaddr_server)) != 0) {
        perror("Bind failed");
        exit(1);
    }

    return server_socket;
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

void server_loop(int server_socket){
        
    // Message qui signale au client la réussite de la connexion 
    char answer_msg[256] = "You have reached the server";

        while (true) {
        // Attribution du rôle de socket d'écoute à notre socket (listen())
        // 5 => Taille max. de la file d'attente de connexion
        if (listen(server_socket, 5) != 0) {
            perror("Listen failed");
            exit(1);
        }

        // Création de la structure adresse du client (struct sockaddr_in)
        struct sockaddr_in sockaddr_client;

        // Création d'un socket de service commun client/serveur (accept())
        // Valeur de retour = numéro du descripteur de fichier du socket de service
        int serv_adress_size = sizeof(sockaddr_client);
        int client_socket = accept(server_socket,(struct sockaddr*) &sockaddr_client, &serv_adress_size);
        
        if (client_socket == -1) {
            printf("Accept failed on server side.");
            exit(1);
        } 

        // Envoi d'un message confirmant la connexion au serveur
        write(client_socket, answer_msg, sizeof(answer_msg));
        
        switch(fork()) {
            case -1:
                perror("Fork failed");
                exit(1);
            case 0:
                close(server_socket);
                char buffer[256] = "";
                memset(buffer, 0, 256);
                read(client_socket, buffer, sizeof(buffer));
                printf("Request received : %s\n", buffer);
                strcpy(buffer, "This is an answer to the request you sent");
                // Envoi du résultat de la requête client grâce au socket de service (send())
                write(client_socket, buffer, 256);

                // Fermeture de la socket de service (close())
                close(client_socket);
                exit(0);
            default:
                break;
        }
        // Fermeture de la socket de service (close())
        close(client_socket);
        printf("Request completed and child process is dead\n");
    }

}


// Serveur qui se met à l'écoute, reçoit une demande client, 
// et y répond par l'envoi du message de réponse.
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong number of arguments : 1 argument needed\n");
        exit(1);
    } else if (atoi(argv[1]) < 9000 || atoi(argv[1]) > 9010) {
        printf("Wrong argument value : An integer between 9000 and 9010 is expected\n");
        exit(1);
    }

    int server_socket = server_socket_init(atoi(argv[1]));

    if(signal_setup() != 0){
        printf("Erreur lors du traitement du signal");
        exit(1);
    }

    server_loop(server_socket);

    // Fermeture du socket d'écoute (close())
    close(server_socket);

    // Fin du programme serveur 
    return 0;
}