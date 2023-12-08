/* Fichier qui contient le programme principal du client. Le rôle de celui-ci est de se connecter à un serveur et de lui communiquer des requêtes que celui-ci 
devra satisfaire. Le client reçoit alors des réponses diverses en fonction de sa requête et saura les interpréter */

// Include des headers
#include "utilitaires.h"
#include "init.h"

/*
*   processus client : Se connecte à un serveur, envoie une requête et attend une réponse qu'il pourra interpréter et afficher.
*       @arguments : int Port number, char* Name of the host you want to connect to
*       @return : int 0 if success
*                 int 1 if something goes wrong
*/
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments : 2 arguments needed (int : Port, char* : hostname) \n");
        exit(1);
    } else if (atoi(argv[1]) < 9000 || atoi(argv[1]) > 9010) {
        printf("Wrong argument value : First argument must be an integer between 9000 and 9010 is expected\n");
        exit(1);
    }

    int socket = client_socket_init(atoi(argv[1]), argv[2]);
    if (communication_to_server(socket) != 0) {
        printf("Erreur de communication avec le serveur, la requête n'a pas pu être complétée.");
        close(socket);
        exit(1);
    }

    // Fermeture de la socket
    close(socket);

    return 0;
}