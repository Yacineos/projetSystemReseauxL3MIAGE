#include <stdio.h> 
#include "init.h"
#include "utilitaires.h"

/* @param : compteur_vdepart = nombre d'éléments dans le tableau, v_depart = chaîne de caractères à tester, ville_existe = true si la ville est déjà dans le tableau, false sinon,
            villes_depart = tableau contenant les villes de départ (uniques)

    Cette fonction permet de remplir un tableau de villes de départ. S'il est vide, on y introduit la ville sans tests. Sinon, on teste l'existence au sein du tableau.
    Si elle n'est pas déjà présente, on l'y introduit. Sinon, on ne fait rien.

    @return : Renvoie le compteur_vdepart qui est le nombre d'éléments dans le tableau (peut avoir changé par rapport à celui passé en paramètres)
*/ 
int init_v_depart(int compteur_vdepart, char v_depart[MAX_SIZE_STRING], bool ville_existe, char villes_depart[100][MAX_SIZE_STRING]){
    // On ne peut pas regarder à l'indice 0 car il n'y a rien (segfault)
    if (compteur_vdepart == 0) {
        strcpy(villes_depart[compteur_vdepart], v_depart);
        ville_existe = true;
        compteur_vdepart++;
    } else {
        ville_existe = est_dans_tableau(compteur_vdepart, villes_depart, v_depart);
    }
    if (ville_existe == false) {
        strcpy(villes_depart[compteur_vdepart], v_depart);
        compteur_vdepart++;
    }
    // On retourne la taille du tableau
    return compteur_vdepart;
}

/* @param : compteur_varrivee = nombre d'éléments dans le tableau, v_arrivee = chaîne de caractères à tester, ville_existe = true si la ville est déjà dans le tableau, false sinon,
            villes_arrivee = tableau contenant les villes d'arrivée' (uniques)

    Cette fonction permet de remplir un tableau de villes d'arrivée. S'il est vide, on y introduit la ville sans tests. Sinon, on teste l'existence au sein du tableau.
    Si elle n'est pas déjà présente, on l'y introduit. Sinon, on ne fait rien.

    @return : Renvoie le compteur_varrivee qui est le nombre d'éléments dans le tableau (peut avoir changé par rapport à celui passé en paramètres)
*/ 
int init_v_arrivee(int compteur_varrivee, char v_arrivee[MAX_SIZE_STRING], bool ville_existe, char villes_arrivee[100][MAX_SIZE_STRING]) {
    if (compteur_varrivee == 0) {
        strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
        ville_existe = true;
        compteur_varrivee++;
    } else {
        ville_existe = est_dans_tableau(compteur_varrivee, villes_arrivee, v_arrivee);
    }
    if (ville_existe == false) {
        strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
        compteur_varrivee++;
    }
    // On retourne la taille du tableau
    return compteur_varrivee;
}

// Initialise 2 tableaux comportant respectivement les villes de départ et d'arrivée
struct tableaux server_data_init(FILE* file) {
    char string[1000];
    char v_depart[MAX_SIZE_STRING];
    char v_arrivee[MAX_SIZE_STRING];
    char villes_depart[100][MAX_SIZE_STRING] = { 0 };
    char villes_arrivee[100][MAX_SIZE_STRING] = { 0 };
    int compteur_vdepart = 0;
    int compteur_varrivee = 0;
    bool ville_existe = false;
    // Parcours tout le fichier jusqu'à la fin
    while(!feof(file)) {
        fgets(string,sizeof(string), file);
        sscanf(string, "%*[^;];%127[^;]", v_depart);
        // Boucle départs
        compteur_vdepart = init_v_depart(compteur_vdepart, v_depart, ville_existe, villes_depart);
        // Reset du booléen pour pouvoir analyser les arrivées
        ville_existe = false;
        sscanf(string, "%*[^;];%*[^;];%127[^;]", v_arrivee);
        // Boucle arrivées
        compteur_varrivee = init_v_arrivee(compteur_varrivee, v_arrivee, ville_existe, villes_arrivee);
        // Reset du booléen pour pouvoir analyser les départs
        ville_existe = false;
    }
    struct tableau tableau_vdepart = {0};
    memcpy(tableau_vdepart.tab, villes_depart, sizeof(villes_depart));
    tableau_vdepart.n = compteur_vdepart;
    struct tableau tableau_varrivee = {0};
    memcpy(tableau_varrivee.tab, villes_arrivee, sizeof(villes_arrivee));
    tableau_varrivee.n = compteur_varrivee;
    struct tableaux tableaux_villes;
    tableaux_villes.tabVilleArrivee = tableau_varrivee;
    tableaux_villes.tabVilleDepart = tableau_vdepart;
    return tableaux_villes;
}

int server_socket_init(int port){

    // Création du socket d'écoute (socket())
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("First socket creation error on server side");
        exit(1);
    } 
    
    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1){
        perror("setsockopt");
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

// Fonction qui initialise le traitement du signal SIGCHLD 
int signal_init(){
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

// Boucle d'exécution du serveur stoppable avec Ctrl + C
// [A REFACTORISER AU MIEUX...]
void server_loop(int server_socket, struct tableaux tableaux_ville) {
        
    // Message qui signale au client la réussite de la connexion 
    char answer_msg[MAX_SIZE_STRING] = "You have reached the server";

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
                char buffer[MAX_SIZE_STRING] = "";
                struct trajet struc_buffer;
                int success = 1;
                while (success == 1) {
                    success = verif_des_villes(struc_buffer.ville_d, tableaux_ville.tabVilleDepart, client_socket); 
                }
                success = 1;
                while (success == 1) {
                    success = verif_des_villes(struc_buffer.ville_a, tableaux_ville.tabVilleArrivee, client_socket); 
                }
                memset(buffer, 0, MAX_SIZE_STRING);
                strcpy(buffer, "10:30");
                strcpy(struc_buffer.heure_d, buffer);
                memset(buffer, 0, MAX_SIZE_STRING);
                strcpy(buffer, "12:30");
                strcpy(struc_buffer.heure_a, buffer);
                struc_buffer.prix = 10.9;

                envoie_trajet(&struc_buffer , client_socket);
                // Envoi du résultat de la requête client grâce au socket de service (write())
                // write(client_socket, &struc_buffer, sizeof(struc_buffer));

                // Fermeture de la socket de service (close())
                close(client_socket);
                exit(0);
            default:
        }
        // Fermeture de la socket de service (close())
        close(client_socket);
    }
}

/* Creation de la socket de service côté client et attribution de son adresse */
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
    // Peut être changé avec une macro
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

    char buffer[MAX_SIZE_STRING] = "";
    struct trajet struc_buffer;

    // Reception des données du serveur
    memset(buffer, 0, MAX_SIZE_STRING);
    read(socket, buffer, MAX_SIZE_STRING);
    printf("%s\n", buffer);
    // Définition du paramètre pour la fonction suivante
    strcpy(buffer, "départ");
    // Saisie, envoi et vérification de la ville de départ
    ville_existe(socket, buffer);

    // Définition du paramètre pour la fonction suivante
    strcpy(buffer, "arrivée");
    // Saisie, envoi et vérification de la ville d'arrivée
    ville_existe(socket, buffer);
    lecture_trajet(&struc_buffer, socket);
    affiche_trajet(struc_buffer);

    return 0;
}