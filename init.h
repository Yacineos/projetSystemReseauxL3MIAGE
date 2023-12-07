/* Fichier qui contient la déclaration de toutes les fonctions d'initialisation du serveur, du client ainsi que de leurs protocoles. 
 * Contient également les includes appropriés pour l'utilisation des fonctions qui en découlent.
 */

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
#include <newlib/sys/select.h>
#include <signal.h>

// Les include [... A DEF]
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// Les include pour traiter les sockets
#include <sys/socket.h>
#include <sys/types.h>
#define MAX_SIZE_STRING 256
#include "utilitaires.h"

// Fonctions côté serveur
int init_v_depart(int compteur_vdepart, char v_depart[MAX_SIZE_STRING], bool ville_existe, char villes_depart[100][MAX_SIZE_STRING]);

int init_v_arrivee(int compteur_varrivee, char v_arrivee[MAX_SIZE_STRING], bool ville_existe, char villes_arrivee[100][MAX_SIZE_STRING]);

struct tableaux server_data_init(FILE* file);

int recherche_trajet(struct trajet *trajet, FILE* fichier_trajets);

int recherche_n_trajets_selon_plage(struct trajet trajet, FILE *fichier_trajets, struct trajet *trajets_trouves);

int recherche_tous_trajets_selon_itineraire(struct trajet trajet, FILE *fichier_trajets, struct trajet *trajets_trouves);

int select_trajet_le_moins_cher(struct trajet *trajets_trouves, int array_size);

int server_socket_init(int port);

int signal_init();

void server_loop(int server_socket, struct tableaux tableaux_ville, FILE* fichier_trajets);

// Fonctions côté client
int client_socket_init(int port);

int communication_to_server(int socket, char *request);