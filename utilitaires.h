/*
 * Fichier qui contient la déclaration de toutes les fonctions utilitaires qui ne font pas partie du processus d'initialisation du serveur, du client ou bien de leurs protocoles
 * Contient également les includes appropriés pour l'utilisation des fonctions qui en découlent.
 */

// Setup des gardes d'inclusion (Mécanisme qui empêche les inclusions multiples dans le même fichier objet)
#ifndef UTILITAIRES_H
#define UTILITAIRES_H

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

#define MAX_SIZE_STRING 256

/* -----------------------------------------------
            DEFINITION DES STRUCTURES :
                - trajet
                - tableau
                - tableaux
--------------------------------------------------*/

/* Structure qui contient toutes les données d'un trajet.
    int : num_train = numéro du train
    char : ville_d = ville de départ, ville_a = ville d'arrivée, heure_d = heure de départ, heure_a = heure d'arrivée
    float : prix = prix du trajet
*/
struct trajet
{
    int num_train;
    char ville_d[MAX_SIZE_STRING];
    char ville_a[MAX_SIZE_STRING];
    char heure_d[MAX_SIZE_STRING];
    char heure_a[MAX_SIZE_STRING];
    float prix;
};

// structure tableau qui contient le tableau d'une taille max
// avec le nombre d'element qui y a dans le tableau
struct tableau
{
    int n;                          // taille du tableau
    char tab[100][MAX_SIZE_STRING]; // tableau d'entier
};

// Structure tableaux qui contient les tableaux d'arrivée et de départ
struct tableaux
{
    struct tableau tabVilleDepart;
    struct tableau tabVilleArrivee;
};

/* -----------------------------------------------
            DEFINITION DES FONCTIONS UTILITAIRES :
                    - trajet
                    - tableau
                    - tableaux
--------------------------------------------------*/

/* Teste si la valleur du paramètre ville est déjà dans le tableau.
Renvoie true, sinon renvoie false*/
bool est_dans_tableau(int size, char tab_villes[100][MAX_SIZE_STRING], char ville[MAX_SIZE_STRING]);

void end_child();

int verif_des_villes(char ville_struc[MAX_SIZE_STRING], struct tableau tableau_villes, int socket);

int ville_existe(int socket, char destination[MAX_SIZE_STRING]);

int envoie_n_trajets(struct trajet *tableau_de_trajet, int n, int socket_service);

int envoie_trajet(struct trajet *trajet_train, int socket_service);

void affiche_trajet(struct trajet struc_trajet);

int lecture_trajet(struct trajet *struc_buffer, int socket);

int lecture_n_trajet(struct trajet *list_trajet_a_remplir, int n, int socket);

void supprimerCaractere(char chaine[MAX_SIZE_STRING], char caractere);

int compare_horaire(char h_depart_demande[MAX_SIZE_STRING], char h_depart_trouve[MAX_SIZE_STRING]);

float calcule_prix(char prix_trajet[MAX_SIZE_STRING], char tarif[MAX_SIZE_STRING]);

int recuperation_champs_fichier(char formula[], char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING]);

int match_depart(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet);

int match_arrivee(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet);

int match_horaire(char horaire_demande[MAX_SIZE_STRING], char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], bool result_found, struct trajet *trajet);

int match_plage_horaire(struct trajet trajet, char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet_a_tester);

int apply_price(char string[MAX_SIZE_STRING], char reduction[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet);

int get_train_number(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet);

int reception_horaire(struct trajet *trajet_courant, int socket, int type_horaire);

int test_horaire(char horaire[MAX_SIZE_STRING]);

void lecture_horaire(char horaire[MAX_SIZE_STRING]);

int envoie_horaire(char horaire[MAX_SIZE_STRING], int socket);

int lecture_et_envoi_horaire(char horaire[MAX_SIZE_STRING], int socket);

int lecture_et_envoie_plage_horaire(int socket);

int reception_plage_horaire(struct trajet *trajet_courant, int socket);

int duree_trajet(char horaire1[MAX_SIZE_STRING], char horaire2[MAX_SIZE_STRING]);

int convertir_duree(int nombre_minutes, char horaire_resultat[MAX_SIZE_STRING]);

int recherche_trajet(struct trajet *trajet, FILE *fichier_trajets);

int recherche_n_trajets_selon_plage(struct trajet trajet, FILE *fichier_trajets, struct trajet *trajets_trouves);

int recherche_tous_trajets_selon_itineraire(struct trajet trajet, FILE *fichier_trajets, struct trajet *trajets_trouves);

int select_trajet_le_moins_cher(struct trajet *trajets_trouves, int array_size);

int select_trajet_le_plus_rapide(struct trajet *trajets_trouves, int array_size);

int select_choix(int choice, struct trajet *trajets_trouves, int array_size);

int compare_villes(struct trajet trajet, int socket);

int verif_villes_existent_et_doublon(int client_socket, struct tableaux tableaux_villes, struct trajet *struc_buffer);

int exec_choix_un(int client_socket, FILE *file, struct tableaux tableaux_ville);

int exec_choix_deux(int client_socket, FILE *file, struct tableaux tableaux_ville);

int exec_choix_trois(int client_socket, FILE *file, struct tableaux tableaux_ville);

int requete_verif_villes(int socket);

int requete_choix_un(int socket);

int requete_choix_deux(int socket);

int requete_choix_trois(int socket);

int affichage_menu_principal();

int choix_utilisateur_menu_principal();

int branchement_selon_choix_principal_client(int choix, int socket);

int branchement_selon_choix_principal_serveur(int choix, int socket, FILE* file, struct tableaux tableaux_ville);

int envoie_chaine(char *chaine, int n, int socket);

int reception_chaine(char chaine_lu[MAX_SIZE_STRING], int socket);

#endif // UTILITAIRES_H