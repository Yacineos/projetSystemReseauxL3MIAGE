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

int server_socket_init(int port);

int signal_init();

void server_loop(int server_socket, struct tableaux tableaux_ville);

// Fonctions côté client
int client_socket_init(int port);

int communication_to_server(int socket, char *request);