#define YACINE inet_addr("152.77.82.209")
// Include du header
#include "utilitaires.h"
#include "init.h"

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