#include <stdio.h> 
#include "utilitaires.h"

// Si la valeur est déjà dans le tableau renvoie true, sinon renvoie false
bool est_dans_tableau(int size, char tab_villes[100][MAX_SIZE_STRING], char ville[MAX_SIZE_STRING]) {
    for (int i = 0; i < size; i++) {
        if (strcmp(tab_villes[i], ville) == 0) {
            return true;
        }
    }
    return false;
}

// Fonction qui sera appelé quand le signal SIGCHLD est reçu
// cette fonction enterine le fils 
void end_child(){
    wait(NULL);
}

/* Fonction qui permet de vérifier qu'une ville d'arrivée ou de départ reçue en lecture sur la socket de service existe bien dans les tableaux de départ ou d'arrivée
    @param : ville_struc[] (char) = partie de la structure trajet dans laquelle on stocke la ville lorsqu'on reçoit une ville valide,
             tableau_villes (struct tableau) = tableau contenant toutes les villes de départ ou d'arrivée
             socket (int) = socket de service
*/  
int verif_des_villes(char ville_struc[MAX_SIZE_STRING], struct tableau tableau_villes, int socket) {
    // Initialisation des variables de la fonction
    char buffer[MAX_SIZE_STRING] = "";
    memset(buffer, 0, MAX_SIZE_STRING);
    // Lecture de la ville reçue et copie dans le buffer
    read(socket, buffer, MAX_SIZE_STRING);
    // Si la ville n'est pas dans le tableau, on envoie au client le code d'échec 1 et on renvoie 1, auquel cas on bouclera sur cette même méthode 
    if (!est_dans_tableau(tableau_villes.n, tableau_villes.tab, buffer)) {
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

/* demande au serveur sila ville entré en paramètre existe ou non 
     redemande la ville à l'utilisateur tant qu'elle existe pas
     @param une ville : char[100]@return void 
*/
int ville_existe(int socket, char destination[MAX_SIZE_STRING]){
    char buffer[MAX_SIZE_STRING];
    do{
        // Demande util de la ville
        printf("Entrez une ville de %s : ", destination);
        memset(buffer, 0, MAX_SIZE_STRING);
        fgets(buffer, MAX_SIZE_STRING, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        // Envoie de la ville à vérifier au serveur
        write(socket, buffer, MAX_SIZE_STRING);
        // Récupération du code de succès de la vérification serveur
        memset(buffer, 0, MAX_SIZE_STRING);
        read(socket, buffer , MAX_SIZE_STRING);
    } while(strcmp(buffer,"1") == 0);
}

