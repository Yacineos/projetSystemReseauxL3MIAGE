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
    printf("Le train numéro %d au départ de %s et à destination de %s prévu à %s arrivera à %s. Coût du billet : %.2f\n",struc_trajet.num_train , struc_trajet.ville_d , struc_trajet.ville_a , struc_trajet.heure_d , struc_trajet.heure_a , struc_trajet.prix );
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

int recuperation_champs_fichier(char formula[], char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING]) {
    int formula_size = snprintf(NULL, 0, "%s", formula);
    char format_formula[formula_size + 1];
    sprintf(format_formula, "%s", formula);
    memset(donnee, 0, MAX_SIZE_STRING);
    sscanf(string, format_formula, donnee);
    return 0;
}

int match_depart(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet) {
    recuperation_champs_fichier("%*[^;];%127[^;]", string, donnee);
    if (strcmp(donnee, trajet->ville_d) == 0) {
        recuperation_champs_fichier("%*[^;];%*[^;];%127[^;]", string, donnee);
        return 0;
    }
    return 1;
}

int match_arrivee(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet) {
    if (strcmp(donnee, trajet->ville_a) == 0) {
        recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee);
        return 0;
    }
    return 1;
}

int match_horaire(char horaire_demande[MAX_SIZE_STRING], char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], bool result_found, struct trajet *trajet) {
    if ((compare_horaire(horaire_demande, donnee) == 0 && !result_found) || (compare_horaire(horaire_demande, donnee) == 0 && compare_horaire(donnee, trajet->heure_d) == 0 && result_found)) {
        strcpy(trajet->heure_d, donnee);
        recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee);
        strcpy(trajet->heure_a, donnee);
        recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee);
        return 0;
    }
    return 1;
}

int match_plage_horaire(struct trajet trajet, char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet_a_tester) {
    if (compare_horaire(trajet.heure_d, donnee) == 0 && compare_horaire(donnee, trajet.heure_a) == 0) {
        strcpy(trajet_a_tester->heure_d, donnee);
        recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee);
        strcpy(trajet_a_tester->heure_a, donnee);
        recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee);
        return 0;
    }
    return 1;
}

int apply_price(char string[MAX_SIZE_STRING], char reduction[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet) {
    if (sscanf(string, "%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%127[^;]", reduction) == 1) {
        donnee[strcspn(donnee, "\n")] = 0;
        reduction[strcspn(reduction, "\n")] = 0;
        trajet->prix = calcule_prix(donnee, reduction);
        return 0;
    } else {
        memset(reduction, 0, MAX_SIZE_STRING);
        trajet->prix = calcule_prix(donnee, reduction);
        return 1;
    }
    return -1;
}

int get_train_number(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet) {
    recuperation_champs_fichier("%127[^;]", string, donnee);
    trajet->num_train = atoi(donnee);
}


/*
 * fonction qui reception un horaire et l'affecte à un trajet fournie en parmètre
 * 
 * @param struct trajet * 
 * @param int socket
 * @param type_horaire si 0 on affecte l'horaire lu dans ville depart 
 *                     si 1 on affecte l'horaire lu dans ville arrivée 
 * 
 * @return int status 1 pour erreur , 0 pour success 
 * 
 */
int reception_horaire(struct trajet *trajet_courant , int socket , int type_horaire){
    char buffer[MAX_SIZE_STRING];
    memset(buffer , 0 , MAX_SIZE_STRING);
    if(read(socket,&buffer,MAX_SIZE_STRING) == -1){
        perror("erreur lecture horaire");
        return 1 ;
    }
    if(type_horaire == 0){
        strcpy(trajet_courant->heure_d,buffer);    
    }else{
        strcpy(trajet_courant->heure_a,buffer);    
    }
    
    return 0;

}

/*
 * Fonction qui vérifie si l'horaire entré respecte les normes (entre 00:00 et 23:59)
 *
 * @param char horaire[256] : horaire entrée par l'utilisateur
 *
 * @return int response : 0 si l'horaire est dans les normes, 1 sinon
 */
int test_horaire(char horaire[MAX_SIZE_STRING]) {
    // Vérifier la longueur de l'horaire
    if (strlen(horaire) != 5 && strlen(horaire) != 4) {
        return 1; // Longueur incorrecte
    }

    // Extraire les heures et minutes
    int heures, minutes;
    if (sscanf(horaire, "%d:%d", &heures, &minutes) != 2 && sscanf(horaire, "%d:%d", &heures, &minutes) != 1) {
        return 1; // Format incorrect
    }

    // Vérifier les plages horaires
    if (heures < 0 || heures > 23 || minutes < 0 || minutes > 59) {
        return 1; // Plage horaire invalide
    }

    // L'horaire est valide
    return 0;
}


/*
 *  une fonction qui lit une horaire au clavier et verifie si elle est valide ou pas 
 *  si elle est pas valide elle relit une autre
 *  sinon elle retourne affecte l'horaire lu à celle passée en paramètre
 * 
 *  @param char[256] horaire
 *  
 *  @return void
 */

void lecture_horaire(char horaire[MAX_SIZE_STRING]){
    do{
        printf("Entrez un horaire valide (hh:mm) : \n");
        scanf("%s",horaire);
    }while( test_horaire(horaire) == 1 ); 
}



/*
 * Fonction qui envoie une horaire 
 * on suppose que horaire est valide 
 * si c'est valide elle envoie au serveur et renvoie 0
 * sinon elle renvoie 1
 * 
 *  @param l'horaire à envoyer : char[256]
 * 
 *  @return status : int 
 */

int envoie_horaire(char horaire[MAX_SIZE_STRING] , int socket){

    char buffer[MAX_SIZE_STRING] ;
    memset(buffer , 0 , MAX_SIZE_STRING);
    strcpy(buffer,horaire);
    if(write(socket , &buffer , MAX_SIZE_STRING) == -1){
        perror("erreur dans l'envoie de l'heure");
        return 1 ;
    }
    return 0;
}

int lecture_et_envoi_horaire(char horaire[MAX_SIZE_STRING], int socket) {
    lecture_horaire(horaire);
    envoie_horaire(horaire, socket);
    return 0;
}

/* #include "utilitaires.h"

struct trajet* nouveau_trajet() {
    struct trajet *trajet;
    trajet->prix = 10.0;
    return trajet;
}

int main() {
    printf("%f", nouveau_trajet()->prix);
    return 0;
}
 * 
 * Fonction qui récupère une plage horaire entré au clavier les 2 horaires et les envoie au serveur une par une
 * 
 * @param int socket
 * 
 * @return status : int si 1 donc erreur
 *                      si 0 success
 *  
 */
int lecture_et_envoie_plage_horaire(int socket){
    // declare 2 horaires
    char horaire1[MAX_SIZE_STRING] ;
    char horaire2[MAX_SIZE_STRING] ;

    // on répète jusqu'à avoir une plage horaire valide (compare_horaire)
    do{
        printf("---------------------------------------------\n");
        printf("Entrez une plage horaire :\n");
        printf("Horaire 1 -- ");
        // * Appel get horaire 2 fois
        lecture_horaire(horaire1);
        printf("Horaire 2 -- ");
        lecture_horaire(horaire2);
        //Vérifie que la 2 horaire est supérieur à la première
            // si c'est pas le cas on redemande les 2 horaires et on affiche l'erreur
    }while(compare_horaire(horaire1,horaire2) != 0 );
    // appel envoie_horaire 2 fois 
    if(envoie_horaire(horaire1,socket) != 0){
        perror("Erreur envoie horaire dans lecture_et_envoie_plage_horaire ");
        return 1 ;
    }
    if(envoie_horaire(horaire2,socket) != 0){
        perror("Erreur envoie horaire dans lecture_et_envoie_plage_horaire ");
        return 1;
    }
    printf("---------------------------------------------\n");
    // retourne code de reussite
    return 0 ;
}

/* 
 * Fonction qui récéptionne une plage horaire et l'affecte au trajet passée en paramètre
 * où l'heure de départ est la première horaire de la plage
 * l'heure d'arrivée est la 2 ème horaire de la plage
 * 
 * @param struct trajet* 
 * @param int socket 
 *  
 * @return int status renvoie 0 si success
 *                            1 si failure
*/
int reception_plage_horaire(struct trajet *trajet_courant , int socket ){

    if(reception_horaire(trajet_courant , socket , 0) != 0){
        perror("erreur lors de la reception in reception_plage_horaire");
        return 1 ;
    }

    if(reception_horaire(trajet_courant , socket , 1) != 0){
        perror("erreur lors de la reception in reception_plage_horaire");
        return 1 ;
    }

    return 0;
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

