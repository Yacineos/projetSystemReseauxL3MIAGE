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
    return 0;
}

/*
    fonction qui envoie n trajet de type trajet vers le client 

    @param struct trajet* tableau_de_trajet
    @param n le nombre de trajet
    @param socket_service : int 


    @result int = 1 false , int = 0 success 
    la fonction renvoie status d'erreur dès l'erreur sur un seul envoie 
*/
int envoie_n_trajets(struct trajet *tableau_de_trajet , int n , int socket_service){
    
    for(int i = 0 ; i < n ; i++){
        if(envoie_trajet(&tableau_de_trajet[i],socket_service) != 0){
            printf("erreur dans l'envoie du trajet : %d",i+1);
            return 1;
        }
    }
    return 0 ;
}

/*
    fonction qui envoie un trajet 
    champs par champs 
    vers un client en utilisant la socket de service 

    @param trajet_train : pointeur vers struct trajet 
    @param socket_service : i

    @return void
*/


int envoie_trajet(struct trajet *trajet_train , int socket_service){
    char buffer[MAX_SIZE_STRING];
    // envoie du numéro de train 
    if(write(socket_service, &trajet_train->num_train, sizeof(int)) == -1){
        perror("write envoie_trajet");
        return 1;
    }
    memset(buffer,0,MAX_SIZE_STRING);
    strcpy(buffer,trajet_train->ville_d);
    // envoie de la ville de départ 
    if(write(socket_service, &buffer, MAX_SIZE_STRING) == -1){
        perror("write envoie_trajet");
        return 1;
    }
    memset(buffer,0,MAX_SIZE_STRING);
    strcpy(buffer,trajet_train->ville_a);
    // envoie de la ville d'arrivée
    if(write(socket_service, &buffer, MAX_SIZE_STRING) == -1){
        perror("write envoie_trajet");
        return 1;
    }
    memset(buffer,0,MAX_SIZE_STRING);
    strcpy(buffer,trajet_train->heure_d);
    // envoie de l'heure de départ 
    if(write(socket_service, &buffer,MAX_SIZE_STRING) == -1){
        perror("write envoie_trajet");
        return 1;        
    }
    memset(buffer,0,MAX_SIZE_STRING);
    strcpy(buffer,trajet_train->heure_a);
    // envoie de l'heure d'arrivée 
    if(write(socket_service, &buffer,MAX_SIZE_STRING) == -1){
        perror("write envoie_trajet");
        return 1;
    }

    // envoie du prix 
    if(write(socket_service, &trajet_train->prix , sizeof(float)) == -1){
        perror("write envoie_trajet");
        return 1;
    }

    return 0;
}

/*
 * Fonction qui affiche un trajet reçu en paramètre 
 *
 */

void affiche_trajet(struct trajet struc_trajet){
    printf("num: %d | villDep: %s | villArr: %s | heureDep: %s | heureArr: %s | prix: %f \n",struc_trajet.num_train , struc_trajet.ville_d , struc_trajet.ville_a , struc_trajet.heure_d , struc_trajet.heure_a , struc_trajet.prix );
}

/*
    fonction qui lit un les champs d'un trajet envoyé par le serveur champs par champs et affecte les valeur au trajet passé en paramètre  

    @param socket : int
    
    @return int 0 success , 1 for error 
*/
int lecture_trajet(struct trajet *struc_buffer , int socket){
     
    char buffer[MAX_SIZE_STRING] = "";

    // lecture du numéro de train
    if(read(socket,&struc_buffer->num_train, sizeof(int)) == -1){
        perror("erreur lecture trajet");
        return 1 ;
    }

    // lecture ville départ 
    memset(buffer, 0, MAX_SIZE_STRING);
    if(read(socket, buffer,MAX_SIZE_STRING) == -1 ){
        perror("erreur lecture trajet");
        return 1 ;
    }
    strcpy(struc_buffer->ville_d,buffer);
    // lecture ville arrivée
    memset(buffer, 0, MAX_SIZE_STRING);
    
    if(read(socket, buffer, sizeof(buffer)) == -1){
        perror("erreur lecture trajet");
        return 1 ;        
    }
    strcpy(struc_buffer->ville_a,buffer);
    
    // lecture heure départ 
    memset(buffer, 0, MAX_SIZE_STRING);
    if(read(socket, buffer, MAX_SIZE_STRING) == -1){
        perror("erreur lecture trajet");
        return 1 ;
    }
    strcpy(struc_buffer->heure_d,buffer);
    
    // lecture heure arrivée 
    memset(buffer, 0, MAX_SIZE_STRING);
    if(read(socket, buffer,MAX_SIZE_STRING) == -1 ){
        perror("erreur lecture trajet");
        return 1 ;
    }
    strcpy(struc_buffer->heure_a,buffer);
    // lecture prix

    if(read(socket, &(struc_buffer->prix) , sizeof(float)) == -1){
        perror("erreur lecture trajet");
        return 1 ;
    }

    // success
    return 0;
    
    }

/* Fonction qui lit n trajet du serveur et remplie un tableau de trajets 
 * 
 * @param struct trajet* à remplir à partir des données lus
 * @param n taille de la liste de trajet 
 * 
 * @return int 1 false , 0 true 
 * 
 * exemple d'appel : 
 * struct trajet struc_buffer[3];
 * lecture_n_trajet(struc_buffer,3,socket);
 */
int lecture_n_trajet(struct trajet *list_trajet_a_remplir, int n , int socket ){
    for(int i = 0 ; i < n ; i++){
        if(lecture_trajet(&list_trajet_a_remplir[i] , socket ) != 0){
            printf("erreur lecture trajet : %d\n",i+1);
            return 1 ;
        }
    }
    return 0 ;
}

//fonction pour suprimer un char d'un char[256]
void supprimerCaractere(char chaine[], char caractere) {
    int i, j;
    char copie_chaine[MAX_SIZE_STRING];

    strcpy(copie_chaine, chaine); // Copie de la chaîne d'origine

    for (i = 0, j = 0; i < strlen(copie_chaine); ++i) {
        if (copie_chaine[i] != caractere) {
            chaine[j++] = copie_chaine[i];
        }
    }
}

//fonction qui compare les deux horaires
int compare_horaire(char h_depart_demande[MAX_SIZE_STRING], char h_depart_trouve[MAX_SIZE_STRING]){
    char copie_h_depart_demande[MAX_SIZE_STRING];
    strcpy(copie_h_depart_demande, h_depart_demande); // Copie de la chaîne d'origine

    supprimerCaractere(copie_h_depart_demande, ':');
    int h_dep_dem = atoi(copie_h_depart_demande);

    char copie_h_depart_trouve[MAX_SIZE_STRING];
    strcpy(copie_h_depart_trouve, h_depart_trouve);
    supprimerCaractere(copie_h_depart_trouve, ':');
    int h_dep_trv = atoi(copie_h_depart_trouve);

    int response;
    if(h_dep_trv >= h_dep_dem){
        return 0;
    }
    return 1;
}

float calcule_prix(char prix_trajet[MAX_SIZE_STRING], char tarif[MAX_SIZE_STRING]) {
    float prix;
    prix = atof(prix_trajet);

    if (strcmp(tarif, "REDUC") == 0) {
        prix = prix - ((20 * prix) / 100);
    } else if (strcmp(tarif, "SUPPL") == 0) {
        prix = prix + ((10 * prix) / 100);
    }

    return prix;
}


/* Fonction qui lit un choix utilisateur puis lit un fichier ligne par ligne et recherche un trajet dont la ville de départ match avec celle demandée. Si match, alors on regarde
si la ville d'arrivée match aussi. Si non, on avance. Si oui, selon le choix utilisateur on fait :
    - cas 1 :
        > création d'une variable horaire_demande à laquelle on affecte l'horaire de départ de la structure passée en paramètre 
        > comparaison de l'horaire de départ trouvé avec l'horaire_demande
            >> Si horaire_demande < horaire_trouve, alors on compare avec l'horaire de départ de la structure :
                >>> Si horaire_trouve < horaire de la structure, alors on le remplace, sinon on ne fait rien
            >> Sinon on ne fait rien
    - cas 2 :
    - cas 3 : 
*/
// int exec_recherche() {
//     return 0;
// }

