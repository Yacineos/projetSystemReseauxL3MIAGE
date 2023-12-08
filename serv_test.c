// Include du header
#include "utilitaires.h"
#include "init.h"

// Serveur qui se met à l'écoute, reçoit une demande client, 
// et y répond par l'envoi du message de réponse.
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments : 2 arguments needed (int : Port, char* : Filename \n");
        exit(1);
    } else if (atoi(argv[1]) < 9000 || atoi(argv[1]) > 9010) {
        printf("Wrong argument value : An integer between 9000 and 9010 is expected\n");
        exit(1);
    }
    FILE* trajets = fopen(argv[2], "r");
    // Initialisation d'une structure contenant le tableau de villes d'arrivée ainsi que celui des villes de départ avec le nombre de valeurs qu'ils contiennent
    struct tableaux tableaux_villes = server_data_init(trajets);

    int server_socket = server_socket_init(atoi(argv[1]));

    if(signal_init() != 0){
        printf("Erreur lors du traitement du signal");
        exit(1);
    }

    server_loop(server_socket, tableaux_villes, trajets);

    // Fermeture du socket d'écoute (close())
    close(server_socket);
    // Fermeture du fichier
    fclose(trajets);
    // Fin du programme serveur 
    return 0;
}