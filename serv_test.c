// Les include génériques
 #include "stdio.h"
 #include "stdlib.h"
 #include "unistd.h"
 #include "stdbool.h"
 #include "string.h"

// Les include pour traiter le signal
#include <sys/wait.h>
#include <newlib/sys/select.h>
#include <signal.h>

 // Include des fonctions et types fondamentaux  
 #include "sys/socket.h"
 #include "sys/types.h"

 // Include des macros
 #include "netinet/in.h"

struct trajet {
    int num_train;
    char ville_d[100];
    char ville_a[100];
    char heure_d[5];
    char heure_a[5];
    float prix;
    char promotion;
};

// structure tableau qui contient le tableau d'une taille max
// avec le nombre d'element qui y a dans le tableau 
struct tableau{
    int n ; // taille du tableau 
    char tab[100][100] ; // tableau d'entier
};

struct tableaux{
    struct tableau tabVilleDepart;
    struct tableau tabVilleArrivee;
};

// Si la valeur est déjà dans le tableau renvoie true, sinon renvoie false
bool est_dans_tableau(int size, char tab_villes[100][100], char ville[100]) {
    for (int i = 0; i < size; i++) {
        if (strcmp(tab_villes[i], ville) == 0) {
            return true;
        }
    }
    return false;
}

/* @param : compteur_vdepart = nombre d'éléments dans le tableau, v_depart = chaîne de caractères à tester, ville_existe = true si la ville est déjà dans le tableau, false sinon,
            villes_depart = tableau contenant les villes de départ (uniques)

    Cette fonction permet de remplir un tableau de villes de départ. S'il est vide, on y introduit la ville sans tests. Sinon, on teste l'existence au sein du tableau.
    Si elle n'est pas déjà présente, on l'y introduit. Sinon, on ne fait rien.

    @return : Renvoie le compteur_vdepart qui est le nombre d'éléments dans le tableau (peut avoir changé par rapport à celui passé en paramètres)
*/ 
int init_v_depart(int compteur_vdepart, char v_depart[100], bool ville_existe, char villes_depart[100][100]){
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
int init_v_arrivee(int compteur_varrivee, char v_arrivee[100], bool ville_existe, char villes_arrivee[100][100]) {
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
    char v_depart[100];
    char v_arrivee[100];
    char villes_depart[100][100] = { 0 };
    char villes_arrivee[100][100] = { 0 };
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

// int verif_des_villes(struct trajet struc_buffer, struct tableaux tableaux_villes, int socket) {
//     char buffer[256] = "";
//     memset(buffer, 0, 256);
//     read(socket, buffer, sizeof(buffer));
//     strcpy(struc_buffer.ville_d, buffer);
//     for (int i = 0; i < tableaux_villes.tabVilleDepart.n; i++) {
//         printf("Départs : %s\n", tableaux_villes.tabVilleDepart.tab[i]);
//     }
//     for (int i = 0; i < tableaux_villes.tabVilleArrivee.n; i++) {
//         printf("Arrivées : %s\n", tableaux_villes.tabVilleArrivee.tab[i]);
//     }
//     if (!est_dans_tableau(tableaux_villes.tabVilleDepart.n, tableaux_villes.tabVilleDepart.tab, struc_buffer.ville_d)) {
//         strcpy(buffer, "La ville de départ n'existe pas.\n");
//         write(socket, buffer, sizeof(buffer));
//     } else {
//         strcpy(buffer, " ");
//         write(socket, buffer, sizeof(buffer)); 
//     }
//     memset(buffer, 0, 256);
//     read(socket, buffer, 256);
//     strcpy(struc_buffer.ville_a, buffer);
//     if (!est_dans_tableau(tableaux_villes.tabVilleArrivee.n, tableaux_villes.tabVilleArrivee.tab, struc_buffer.ville_a)) {
//         strcpy(buffer, "La ville d'arrivée n'existe pas.\n");
//         write(socket, buffer, sizeof(buffer));
//     } else {
//         strcpy(buffer, " ");
//         write(socket, buffer, sizeof(buffer));        
//     }
//     return 0;
// }

int verif_des_villes(char ville_struc[256], struct tableau tableau_villes, int socket) {
    // Initialisation des variables de la fonction
    char buffer[256] = "";
    memset(buffer, 0, 256);
    // Lecture de la ville reçue
    read(socket, buffer, 256);
    // Si la ville n'est pas dans le tableau, on envoie au client le code d'échec 1 et on renvoie 1, auquel cas on bouclera sur cette même méthode 
    if (!est_dans_tableau(tableaux_villes.tabVilleDepart.n, tableaux_villes.tabVilleDepart.tab, struc_buffer.ville_d)) {
        strcpy(buffer, "1");
        write(socket, buffer, sizeof(buffer));
        return 1;
    } else { // Si on trouve la ville dans le tableau des villes concerné
        // Copie de la ville dans la structure trajet
        strcpy(ville_struc, buffer);
        // Envoi du code de retour de succès de la fonction
        strcpy(buffer, "0");
        write(socket, buffer, sizeof(buffer)); 
    }
    return 0;
}


void server_loop(int server_socket, struct tableaux tableaux_ville){
        
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
                struct trajet struc_buffer;
                verif_des_villes(struc_buffer, tableaux_ville, client_socket);
                // Envoi du résultat de la requête client grâce au socket de service (write())
                write(client_socket, &struc_buffer, sizeof(struc_buffer));

                // Fermeture de la socket de service (close())
                close(client_socket);
                exit(0);
            default:
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
    FILE* trajets = fopen("trajets.txt", "r");
    // Initialisation d'une structure contenant le tableau de villes d'arrivée ainsi que celui des villes de départ avec le nombre de valeurs qu'ils contiennent
    struct tableaux tableaux_villes = server_data_init(trajets);

    int server_socket = server_socket_init(atoi(argv[1]));

    if(signal_setup() != 0){
        printf("Erreur lors du traitement du signal");
        exit(1);
    }

    server_loop(server_socket, tableaux_villes);

    // Fermeture du socket d'écoute (close())
    close(server_socket);
    // Fermeture du fichier
    fclose(trajets);
    // Fin du programme serveur 
    return 0;
}