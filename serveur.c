// Les include génériques
 #include "stdio.h"
 #include "stdlib.h"
 #include "unistd.h"

 // Include des fonctions et types fondamentaux  
 #include "sys/socket.h"
 #include "sys/types.h"

 // Include des macros
 #include "netinet/in.h"


// Serveur qui se met à l'écoute, reçoit une demande client, 
// et y répond par l'envoi du message de réponse.
int main(void){
    // Création du socket d'écoute (socket())
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Création de la structure adresse du serveur (struct sockaddr_in)
    struct sockaddr_in sockaddr_server;
    sockaddr_server.sin_family = AF_INET;
    sockaddr_server.sin_port = htons(9000);
    sockaddr_server.sin_addr.s_addr = INADDR_ANY;

    // Association de la structure d'adresse et du socket d'écoute (bind())
    bind(server_socket, (struct sockaddr*) &sockaddr_server, sizeof(sockaddr_server));

    // Attribution du rôle de socket d'écoute à notre socket (listen())
    // 5 => Taille max. de la file d'attente de connexion
    listen(server_socket, 5);

    // Création d'un socket de service commun client/serveur (accept())
    // Valeur de retour = numéro du descripteur de fichier du socket de service
    int serv_adress_size = sizeof(sockaddr_server);
    int client_socket = accept(server_socket,(struct sockaddr*) &sockaddr_server, &serv_adress_size);

    // Envoi du résultat de la requête client grâce au socket de service (send())
    char answer_msg[256];
    send(client_socket, answer_msg, sizeof(answer_msg), 0);

    // Fermeture de la socket de service (close())
    close(client_socket);
    
    // Fermeture du socket d'écoute (close())
    close(server_socket);

    // Fin du programme serveur 
    return 0;
}