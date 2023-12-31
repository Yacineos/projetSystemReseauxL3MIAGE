/*
 * Fichier qui contient la définition de toutes les fonctions d'initialisation du serveur, du client ainsi que de leurs protocoles
 */

#include <stdio.h>
#include "init.h"
#include "utilitaires.h"

/* Fonction qui initialise le socket d'écoute du serveur
 *  @param : int Port number
    @return : int Descripteur de fichier de la socket d'écoute
 */
int server_socket_init(int port)
{

    // Création du socket d'écoute (socket())
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("First socket creation error on server side");
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    // Création de la structure adresse du serveur (struct sockaddr_in)
    struct sockaddr_in sockaddr_server;
    sockaddr_server.sin_family = AF_INET;
    sockaddr_server.sin_port = htons(port);
    sockaddr_server.sin_addr.s_addr = INADDR_ANY;

    // Association de la structure d'adresse et du socket d'écoute (bind())
    if (bind(server_socket, (struct sockaddr *)&sockaddr_server, sizeof(sockaddr_server)) != 0)
    {
        perror("Bind failed");
        exit(1);
    }

    return server_socket;
}

/*  Cette fonction permet de remplir un tableau de villes de départ. S'il est vide, on y introduit la ville sans tests. Sinon, on teste l'existence au sein du tableau.
    Si elle n'est pas déjà présente, on l'y introduit. Sinon, on ne fait rien.

    @param : compteur_vdepart = nombre d'éléments dans le tableau
    @param : chaîne de caractères à tester
    @param : ville_existe = true si la ville est déjà dans le tableau, false sinon
    @param : villes_depart = tableau contenant les villes de départ (uniques)

    @return : Renvoie le compteur_vdepart qui est le nombre d'éléments dans le tableau (peut avoir changé par rapport à celui passé en paramètres)
*/
int init_v_depart(int compteur_vdepart, char v_depart[MAX_SIZE_STRING], bool ville_existe, char villes_depart[100][MAX_SIZE_STRING])
{
    // On ne peut pas regarder à l'indice 0 car il n'y a rien (segfault)
    if (compteur_vdepart == 0)
    {
        strcpy(villes_depart[compteur_vdepart], v_depart);
        ville_existe = true;
        compteur_vdepart++;
    }
    else
    {
        ville_existe = est_dans_tableau(compteur_vdepart, villes_depart, v_depart);
    }
    if (ville_existe == false)
    {
        strcpy(villes_depart[compteur_vdepart], v_depart);
        compteur_vdepart++;
    }
    // On retourne la taille du tableau
    return compteur_vdepart;
}

/*  Cette fonction permet de remplir un tableau de villes d'arrivée. S'il est vide, on y introduit la ville sans tests. Sinon, on teste l'existence au sein du tableau.
    Si elle n'est pas déjà présente, on l'y introduit. Sinon, on ne fait rien.

    @param : compteur_varrivee = nombre d'éléments dans le tableau
    @param : v_arrivee = chaîne de caractères à tester
    @param : ville_existe = true si la ville est déjà dans le tableau, false sinon
    @param : villes_arrivee = tableau contenant les villes d'arrivée (uniques)

    @return : Renvoie le compteur_varrivee qui est le nombre d'éléments dans le tableau
*/
int init_v_arrivee(int compteur_varrivee, char v_arrivee[MAX_SIZE_STRING], bool ville_existe, char villes_arrivee[100][MAX_SIZE_STRING])
{
    if (compteur_varrivee == 0)
    {
        strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
        ville_existe = true;
        compteur_varrivee++;
    }
    else
    {
        ville_existe = est_dans_tableau(compteur_varrivee, villes_arrivee, v_arrivee);
    }
    if (ville_existe == false)
    {
        strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
        compteur_varrivee++;
    }
    // On retourne la taille du tableau
    return compteur_varrivee;
}

/* Initialise 2 tableaux comportant respectivement les villes de départ et d'arrivée

   @param : FILE* pointeur vers le fichier ouvert (ici en lecture)

   @return : struct tableaux Les tableaux initialisés et remplis
*/
struct tableaux server_data_init(FILE *file)
{
    char string[1000];
    char v_depart[MAX_SIZE_STRING];
    char v_arrivee[MAX_SIZE_STRING];
    char villes_depart[100][MAX_SIZE_STRING] = {0};
    char villes_arrivee[100][MAX_SIZE_STRING] = {0};
    int compteur_vdepart = 0;
    int compteur_varrivee = 0;
    bool ville_existe = false;
    // Parcours tout le fichier jusqu'à la fin
    while (!feof(file))
    {
        fgets(string, sizeof(string), file);
        sscanf(string, "%*[^;];%127[^;]", v_depart);
        // Boucle départs
        compteur_vdepart = init_v_depart(compteur_vdepart, v_depart, ville_existe, villes_depart);
        ville_existe = false;
        sscanf(string, "%*[^;];%*[^;];%127[^;]", v_arrivee);
        // Boucle arrivées
        compteur_varrivee = init_v_arrivee(compteur_varrivee, v_arrivee, ville_existe, villes_arrivee);
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

/* Fonction qui initialise le traitement du signal SIGCHLD 
    @return : int 0 si succès
              int 1 sinon
*/
int signal_init()
{
    // déclaration d'une structure qui contient les actions à effectuer à la reception du signal
    struct sigaction ac;
    // affectation des valeurs
    ac.sa_handler = end_child;
    ac.sa_flags = SA_RESTART;

    // execution du handler au moment où on reçoit SIGCHLD
    if (sigaction(SIGCHLD, &ac, NULL) != 0)
    {
        perror("Sigaction failed");
        exit(1);
    }
    return 0;
}

/*  Réceptionne l'input utilisateur et exécute un branchement en conséquence

    @param : int Socket d'écoute du serveur
    @param : struct tableaux La structure qui contient les tableaux des villes de départ et d'arrivée déjà initialisés
    @param : FILE* Pointeur vers le fichier ouvert (en lecture ici)
*/
void server_loop(int server_socket, struct tableaux tableaux_ville, FILE *file)
{

    while (true)
    {
        struct sockaddr_in sockaddr_client;
        socklen_t serv_adress_size = sizeof(sockaddr_client);
        int client_socket = accept(server_socket, (struct sockaddr *)&sockaddr_client, &serv_adress_size);

        if (client_socket == -1)
        {
            printf("Accept failed on server side.");
            exit(1);
        }

        // Création de processus fils pour répondre à plusieurs requêtes de la part de différents clients en simultané 
        switch (fork())
        {
            case -1:
                perror("Fork failed");
                exit(1);
            case 0:
                close(server_socket);
                int choix = -1;
                while (choix != 4) {
                    read(client_socket, &choix, sizeof(int));
                    branchement_selon_choix_principal_serveur(choix, client_socket, file, tableaux_ville);
                }
                printf("J'ai pas loop\n");

                // Fermeture de la socket de service (close())
                close(client_socket);
                exit(0);
            default:
            // Pas de break car fin de switch, on met cependant un ; pour compiler dans les systèmes plus capricieux
                ;
        }
        close(client_socket);
    }
}

/* Creation de la socket de service côté client et attribution de son adresse

    @param : int : Port number
    @param : char* Host name

    @return : int Descripteur de fichier de la socket de service  */
int client_socket_init(int port, char* host)
{
    // Création de la socket
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1)
    {
        printf("First socket creation error on server side");
        exit(1);
    }

    // Récupération de l'adresse IP de l'hôte
    struct hostent* h = gethostbyname(host);

    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(port);
    memcpy(&(server_adress.sin_addr.s_addr), h->h_addr_list[0], h->h_length);

    /*   entier qui recevra l'etat de la connexion connect() renvoie 0 s'il réussit, ou -1 s'il échoue, auquel  cas  errno
         contient le code d'erreur. */
    int connection_status = connect(network_socket, (struct sockaddr *)&server_adress, sizeof(server_adress));

    if (connection_status == -1)
    {
        printf("Connection to server error on client side");
        exit(1);
    }
    return network_socket;
}

/*  Fonction principale de communication du client vers le serveur. Elle permet l'affichage d'un menu et la lecture des input utilisateur.
    Envoi, reception et interprétation des données serveur. 

    @param : int Socket de service

    @return : int 0 si la requête est complétée sans erreur
              int 1 sinon
 */
int communication_to_server(int socket)
{
    int menu = choix_utilisateur_menu_principal();
    while (menu != 4) {
        if (branchement_selon_choix_principal_client(menu, socket) == 0) {
            printf("Requête complétée!\n");
            menu = choix_utilisateur_menu_principal();
        }

    }
    printf("Merci et aurevoir !\n");
    return 0;
}
