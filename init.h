/* Fichier qui contient la déclaration de toutes les fonctions d'initialisation du serveur, du client ainsi que de leurs protocoles.
 * Contient également les includes appropriés pour l'utilisation des fonctions qui en découlent.
 */

/* ---------------------------------------

                Inclusions

   --------------------------------------- */

// Les include génériques
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <float.h>

// Les include pour traiter le signal
#include <sys/wait.h>
#include <signal.h>

// Les include pour traiter les sockets, les adresses IP ainsi que les DNS
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

// Inclusions & définition de macros perso
#define MAX_SIZE_STRING 256
#include "utilitaires.h"

/* ---------------------------------------

         Fonctions côté serveur 

   --------------------------------------- */

int server_socket_init(int port);

int init_v_depart(int compteur_vdepart, char v_depart[MAX_SIZE_STRING], bool ville_existe, char villes_depart[100][MAX_SIZE_STRING]);

int init_v_arrivee(int compteur_varrivee, char v_arrivee[MAX_SIZE_STRING], bool ville_existe, char villes_arrivee[100][MAX_SIZE_STRING]);

struct tableaux server_data_init(FILE *file);

int signal_init();

void server_loop(int server_socket, struct tableaux tableaux_ville, FILE *fichier_trajets);

/* ---------------------------------------

         Fonctions côté client 

   --------------------------------------- */

int client_socket_init(int port, char* host);

int communication_to_server(int socket);