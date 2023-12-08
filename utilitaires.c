/* 
* Fichier qui contient la définition de toutes les fonctions utilitaires qui ne font pas partie du processus d'initialisation du serveur, du client ou bien de leurs protocoles
*/


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

/* demande au serveur si la ville entré en paramètre existe ou non 
     redemande la ville à l'utilisateur tant qu'elle existe pas
     @param une ville : char[100]@return void 
*/
int ville_existe(int socket, char destination[MAX_SIZE_STRING]){
    char buffer[MAX_SIZE_STRING];
    int i = 0;
    do{
        // Demande util de la ville
        if (i != 0) {
            printf("Cette ville n'existe pas dans notre base de données.\n");
        }
        printf("Entrez une ville %s : ", destination);
        memset(buffer, 0, MAX_SIZE_STRING);
        fgets(buffer, MAX_SIZE_STRING, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        // Envoie de la ville à vérifier au serveur
        write(socket, buffer, MAX_SIZE_STRING);
        // Récupération du code de succès de la vérification serveur
        memset(buffer, 0, MAX_SIZE_STRING);
        read(socket, buffer , MAX_SIZE_STRING);
        i++;
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
 *  @param : struct trajet
 */

void affiche_trajet(struct trajet struc_trajet){
    printf("| Le train numéro %d au départ de %s et à destination de %s prévu à %s arrivera à %s. Coût du billet : %.2f |\n",struc_trajet.num_train , struc_trajet.ville_d , struc_trajet.ville_a , struc_trajet.heure_d , struc_trajet.heure_a , struc_trajet.prix );
    printf("--------------------------------------------\n");
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

/* Fonction qui supprime un caractère d'une chaîne de caractères 
 *  @param : char chaîne de caractères à traiter, char caractère à retirer de la chaîne
 */
void supprimerCaractere(char chaine[MAX_SIZE_STRING], char caractere) {
    int i, j;
    char copie_chaine[MAX_SIZE_STRING];

    strcpy(copie_chaine, chaine); // Copie de la chaîne d'origine

    for (i = 0, j = 0; i < strlen(copie_chaine); ++i) {
        if (copie_chaine[i] != caractere) {
            chaine[j++] = copie_chaine[i];
        }
    }
}

/* Fonction qui compare deux horaires passés en paramètre et renvoie un résultat en fonction du plus grand paramètre
 *  @param : char horaire demandé (comparant), char horaire trouvé (comparé)
 *  @return : int 0 si comparant < comparé
 *            int 1 si comparant > comparé
 */ 
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

/* Fonction qui permet de convertir une chaîne de caractère en un float et de calculer un prix par rapport à la chaîne convertie et une éventuelle réduction ou supplément
 * @param : char prix de base du trajet, char "REDUC", "SUPPL" ou une chaîne quelconque
 * @return : float prix du billet calculé 
 */
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

/* Fonction qui permet de récupérer une chaîne de caractère correspondant à une expression régulière passée en paramètre et de la stocker dans le buffer passé en paramètre
 *  @param : char expression régulière sous forme de chaîne de caractère, char chaîne de caractère à parser, char chaîne de caractère buffer
 *  @return : int 0 si la fonction ne crash pas
 */
int recuperation_champs_fichier(char formula[], char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING]) {
    int formula_size = snprintf(NULL, 0, "%s", formula);
    char format_formula[formula_size + 1];
    sprintf(format_formula, "%s", formula);
    memset(donnee, 0, MAX_SIZE_STRING);
    sscanf(string, format_formula, donnee);
    return 0;
}

/* Fonction qui permet de vérifier qu'une ville de départ passée en paramètres correspond à la ville de départ du trajet dont la chaîne de caractère est passée en paramètres
 *  @param : char chaîne de caratère contenant les données brutes d'un trajet, char chaîne de caractère servant de buffer pour récupérer la ville de départ du premier argument, struct trajet un trajet avec une ville de départ à comparer
 *  @return : int 0 si match entre la ville récupérée et la ville de départ du trajet passé en paramètre
 *            int 1 si pas match
 */
int match_depart(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet) {
    recuperation_champs_fichier("%*[^;];%127[^;]", string, donnee);
    if (strcmp(donnee, trajet->ville_d) == 0) {
        recuperation_champs_fichier("%*[^;];%*[^;];%127[^;]", string, donnee);
        return 0;
    }
    return 1;
}

/* Fonction qui permet de vérifier qu'une ville d'arrivée passée en paramètres correspond à la ville d'arrivée du trajet dont la chaîne de caractère est passée en paramètres
 *  @param : char chaîne de caratère contenant les données brutes d'un trajet, char chaîne de caractère servant de buffer pour récupérer la ville d'arrivée du premier argument, struct trajet un trajet avec une ville d'arrivée à comparer
 *  @return : int 0 si match entre la ville récupérée et la ville d'arrivée du trajet passé en paramètre
 *            int 1 si pas match
 */
int match_arrivee(char string[MAX_SIZE_STRING], char donnee[MAX_SIZE_STRING], struct trajet *trajet) {
    if (strcmp(donnee, trajet->ville_a) == 0) {
        recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee);
        return 0;
    }
    return 1;
}

/* Fonction qui permet de vérifier qu'un horaire passée en paramètres est inférieur à l'horaire de du trajet dont la chaîne de caractère est passée en paramètres et s'il est, assigne les valeurs de départ et d'arrvée dans la structure 
 * S'il y a déjà un trajet trouvé auparavant, compare aussi le nouvel horaire avec l'horaire du trajet précédemment trouvé. Si le nouvel horaire trouvé < ancien horaire trouvé, on remplace l'ancien trajet par le nouveau 
 *  @param : char horaire demandée sous forme de chaîne de caractères, char chaîne de caratère contenant les données brutes d'un trajet, 
 *           char chaîne de caractère servant de buffer pour récupérer l'horaire d'arrivée du second argument, bool true si un trajet optimal a déjà été trouvé auparavant, struct trajet un trajet dont on veut remplir l'heure de départ et d'arrivée
 *  @return : int 0 si l'horaire trouvé correspond et que les données ont été correctement assignées à la structure
 *            int 1 si pas match
 */
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

/* Fonction qui permet de vérifier que l'horaire de départ d'un trajet dont la chaîne de caractères est passée en paramètres correspond à une plage horaire contenue dans une structure et met à jour les champs de laseconde structure 
 * passée en paramètre si cela correspond.  
 *  @param : struct trajet qui contient la plage horaire demandée, char chaîne de caractère du trajet à tester, char chaîne buffer pour les données à tester, 
 * struct trajet qui est la structure à remplir si le trajet trouvé correspond à nos critères
 *  @return : int 0 si l'horaire testé correspond à la plage horaire demandée et que l'attribution se fait correctement
 *            int 1 sinon
 */           
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
        printf("Entrez un horaire valide (hh:mm) : ");
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

/*
    Fonction qui calcule la durée en minutes d’une plage horaire VALIDE 
    donc l'horaire d'arrivée est forcément après l'horaire de départ 

    @param char horaire1[MAX_SIZE_STRING]
    @param char horaire2[MAX_SIZE_STRING]

    Exemple : 8:55 , 12:00 ->  185 minutes
    @return int durée du trajet en minutes

*/
int duree_trajet(char horaire1[MAX_SIZE_STRING], char horaire2[MAX_SIZE_STRING]) {
    // Parse hours and minutes from the input strings
    int heure1, minute1, heure2, minute2;

    sscanf(horaire1, "%d:%d", &heure1, &minute1);
    sscanf(horaire2, "%d:%d", &heure2, &minute2);

    // Calculate the duration in minutes
    int duration = (heure2 - heure1) * 60 + (minute2 - minute1);

    return duration;
}



/*
    Fonction qui convertit un nombre de minutes en format "x heures et y minutes"

    Exemple : 123 minutes -> “2 heures et 3 minutes”

    @param int nombre_minutes : Le nombre de minutes à convertir
    @param char horaire_resultat[MAX_SIZE_STRING] : La chaîne où le résultat sera écrit

    @return int Retourne 1 en cas d'erreur, 0 sinon
*/
int convertir_duree(int nombre_minutes, char horaire_resultat[MAX_SIZE_STRING]) {
    if (nombre_minutes < 0)
    {
        // Gestion d'une entrée invalide (nombre de minutes négatif)
        return 1; // false
    }

    int heures = nombre_minutes / 60;
    int minutes = nombre_minutes % 60;

    // Utilisation de snprintf pour formater la chaîne de résultat
    snprintf(horaire_resultat, MAX_SIZE_STRING, "%d heures et %d minutes", heures, minutes);

    return 0;
}

int recherche_trajet(struct trajet *trajet, FILE *fichier_trajets)
{
    // Variable qui stocke l'horaire demandé par l'utilisateur
    char horaire_demande[MAX_SIZE_STRING];
    strcpy(horaire_demande, trajet->heure_d);
    // Variable qui stocke une par une chaque ligne du fichier lue par fgets
    char string[MAX_SIZE_STRING];
    // Variable qui stocke les données parsées par sscanf une par une
    char donnee_trouvee[MAX_SIZE_STRING];
    // Variable qui stocke, une réduction s'il y en a, sinon sera vide
    char reduction[MAX_SIZE_STRING];
    // Variable qui indique si nous avons trouvé un train optimal ou si aucun train n'a été trouvé
    bool result_found = false;
    // Le fichier a déjà été parcouru à l'initialisation du serveur, on remet donc le curseur au début du fichier
    rewind(fichier_trajets);
    while (!feof(fichier_trajets))
    {
        // Lecture du fichier ligne par ligne
        fgets(string, MAX_SIZE_STRING, fichier_trajets);
        // Vérification de la ville de départ
        if (match_depart(string, donnee_trouvee, trajet) == 0)
        {
            // Vérification de la ville d'arrivée
            if (match_arrivee(string, donnee_trouvee, trajet) == 0)
            {
                // Vérif que l'horaire correspond
                if (match_horaire(horaire_demande, string, donnee_trouvee, result_found, trajet) == 0)
                {
                    apply_price(string, reduction, donnee_trouvee, trajet);
                    get_train_number(string, donnee_trouvee, trajet);
                    result_found = true;
                }
            }
        }
    }
    // Codes de retour en fonction de si on trouve un train. Interprété par le client
    if (result_found)
    {
        return 0;
    }
    return 1;
}

int recherche_n_trajets_selon_plage(struct trajet trajet, FILE *fichier_trajets, struct trajet *trajets_trouves)
{
    char string[MAX_SIZE_STRING];
    char donnee_trouvee[MAX_SIZE_STRING];
    char reduction[MAX_SIZE_STRING];
    int array_size = 0;
    rewind(fichier_trajets);
    while (!feof(fichier_trajets))
    {
        struct trajet trajet_a_tester;
        fgets(string, MAX_SIZE_STRING, fichier_trajets);
        if (match_depart(string, donnee_trouvee, &trajet) == 0)
        {
            strcpy(trajet_a_tester.ville_d, trajet.ville_d);
            if (match_arrivee(string, donnee_trouvee, &trajet) == 0)
            {
                strcpy(trajet_a_tester.ville_a, trajet.ville_a);
                if (match_plage_horaire(trajet, string, donnee_trouvee, &trajet_a_tester) == 0)
                {
                    apply_price(string, reduction, donnee_trouvee, &trajet_a_tester);
                    get_train_number(string, donnee_trouvee, &trajet_a_tester);
                    trajets_trouves[array_size] = trajet_a_tester;
                    array_size++;
                }
            }
        }
    }
    return array_size;
}

int recherche_tous_trajets_selon_itineraire(struct trajet trajet, FILE *fichier_trajets, struct trajet *trajets_trouves)
{
    char string[MAX_SIZE_STRING];
    char donnee_trouvee[MAX_SIZE_STRING];
    char reduction[MAX_SIZE_STRING];
    int array_size = 0;
    rewind(fichier_trajets);
    while (!feof(fichier_trajets))
    {
        struct trajet trajet_a_tester;
        fgets(string, MAX_SIZE_STRING, fichier_trajets);
        if (match_depart(string, donnee_trouvee, &trajet) == 0)
        {
            strcpy(trajet_a_tester.ville_d, trajet.ville_d);
            if (match_arrivee(string, donnee_trouvee, &trajet) == 0)
            {
                strcpy(trajet_a_tester.ville_a, trajet.ville_a);
                strcpy(trajet_a_tester.heure_d, donnee_trouvee);
                recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee_trouvee);
                strcpy(trajet_a_tester.heure_a, donnee_trouvee);
                recuperation_champs_fichier("%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%127[^;]", string, donnee_trouvee);
                apply_price(string, reduction, donnee_trouvee, &trajet_a_tester);
                get_train_number(string, donnee_trouvee, &trajet_a_tester);
                trajets_trouves[array_size] = trajet_a_tester;
                array_size++;
            }
        }
    }
    return array_size;
}

int select_trajet_le_moins_cher(struct trajet *trajets_trouves, int array_size)
{
    int array_index = array_size + 1;
    float smallest_price = FLT_MAX;
    for (int i = 0; i < array_size; i++)
    {
        if (trajets_trouves[i].prix < smallest_price)
        {
            array_index = i;
            smallest_price = trajets_trouves[i].prix;
        }
    }
    if (array_index < array_size)
    {
        return array_index;
    }
    return -1;
}

int select_trajet_le_plus_rapide(struct trajet *trajets_trouves, int array_size)
{
    int array_index = array_size + 1;
    int quickest_travel_time = INT_MAX;
    int travel_time;
    for (int i = 0; i < array_size; i++)
    {
        travel_time = duree_trajet(trajets_trouves[i].heure_d, trajets_trouves[i].heure_a);
        if (travel_time < quickest_travel_time)
        {
            array_index = i;
            quickest_travel_time = travel_time;
        }
    }
    if (array_index < array_size)
    {
        return array_index;
    }
    return -1;
}

int select_choix(int choice, struct trajet *trajets_trouves, int array_size)
{
    if (choice == 1)
    {
        return select_trajet_le_plus_rapide(trajets_trouves, array_size);
    }
    else if (choice == 2)
    {
        return select_trajet_le_moins_cher(trajets_trouves, array_size);
    }
    else
    {
        return -1;
    }
}

int compare_villes(struct trajet trajet, int socket) {
    int error = 1;
    if (strcmp(trajet.ville_d, trajet.ville_a) == 0) {
        write(socket, &error, sizeof(int));
        return error; 
    }
    error = 0;
    write(socket, &error, sizeof(int));
    return error;
}

int verif_villes_existent_et_doublon(int client_socket, struct tableaux tableaux_villes, struct trajet struc_buffer) {
    int error = -1;
    do {
        int success = 1;
        while (success == 1)
        {
            success = verif_des_villes(struc_buffer.ville_d, tableaux_villes.tabVilleDepart, client_socket);
        }
        success = 1;
        while (success == 1)
        {
            success = verif_des_villes(struc_buffer.ville_a, tableaux_villes.tabVilleArrivee, client_socket);
        }
        error = compare_villes(struc_buffer, client_socket);
    } while (error != 0);
}

int exec_choix_un(int client_socket, FILE *file, struct tableaux tableaux_ville)
{
    char buffer[MAX_SIZE_STRING] = "";
    struct trajet struc_buffer;
    int success = 1;
    int error = -1;
    verif_villes_existent_et_doublon(client_socket, tableaux_ville, struc_buffer);
    reception_horaire(&struc_buffer, client_socket, 0);
    error = recherche_trajet(&struc_buffer, file);
    if (error != 0) {
        write(client_socket, &error, sizeof(int));
        return 1;
    }
    write(client_socket, &error, sizeof(int));
    envoie_trajet(&struc_buffer, client_socket);
    return 0;
}

int exec_choix_deux(int client_socket, FILE *file, struct tableaux tableaux_ville)
{
    char buffer[MAX_SIZE_STRING] = "";
    struct trajet struc_buffer;
    struct trajet trajets_trouves[MAX_SIZE_STRING];
    int success = 1;
    while (success == 1)
    {
        success = verif_des_villes(struc_buffer.ville_d, tableaux_ville.tabVilleDepart, client_socket);
    }
    success = 1;
    while (success == 1)
    {
        success = verif_des_villes(struc_buffer.ville_a, tableaux_ville.tabVilleArrivee, client_socket);
    }
    reception_plage_horaire(&struc_buffer, client_socket);
    int array_size = recherche_n_trajets_selon_plage(struc_buffer, file, trajets_trouves);
    write(client_socket, &array_size, sizeof(int));
    envoie_n_trajets(trajets_trouves, array_size, client_socket);
}

int exec_choix_trois(int client_socket, FILE *file, struct tableaux tableaux_ville)
{
    char buffer[MAX_SIZE_STRING] = "";
    struct trajet struc_buffer;
    struct trajet trajets_trouves[MAX_SIZE_STRING];
    int success = 1;
    while (success == 1)
    {
        success = verif_des_villes(struc_buffer.ville_d, tableaux_ville.tabVilleDepart, client_socket);
    }
    success = 1;
    while (success == 1)
    {
        success = verif_des_villes(struc_buffer.ville_a, tableaux_ville.tabVilleArrivee, client_socket);
    }
    int array_size = recherche_tous_trajets_selon_itineraire(struc_buffer, file, trajets_trouves);
    write(client_socket, &array_size, sizeof(int));
    envoie_n_trajets(trajets_trouves, array_size, client_socket);
    int choice;
    read(client_socket, &choice, sizeof(int));
    int array_index = select_choix(choice, trajets_trouves, array_size);
    write(client_socket, &array_index, sizeof(int));
    return 0;
}

int requete_choix_un(int socket)
{
    char buffer[MAX_SIZE_STRING] = "";
    struct trajet struc_buffer;
    int error = 0;

    do {
        // Définition du paramètre pour la fonction suivante
        strcpy(buffer, "de départ");
        // Saisie, envoi et vérification de la ville de départ
        ville_existe(socket, buffer);

        // Définition du paramètre pour la fonction suivante
        strcpy(buffer, "d'arrivée");
        // Saisie, envoi et vérification de la ville d'arrivée
        ville_existe(socket, buffer);
        printf("\n");
        read(socket, &error, sizeof(int));
    } while (error != 0); 

    lecture_et_envoi_horaire(buffer, socket);
    read(socket, &error, sizeof(int));
    if (error != 0) {
        printf("Navrés, aucun train n'est disponible à partir de cet horaire aujourd'hui.\n");
        return 1;
    }
    lecture_trajet(&struc_buffer, socket);
    affiche_trajet(struc_buffer);
    return 0;
}

int requete_choix_deux(int socket)
{
    char buffer[MAX_SIZE_STRING] = "";
    struct trajet struc_buffer;
    struct trajet trajets_trouves[MAX_SIZE_STRING];
    int array_size = 0;


    // Définition du paramètre pour la fonction suivante
    strcpy(buffer, "départ");
    // Saisie, envoi et vérification de la ville de départ
    ville_existe(socket, buffer);

    // Définition du paramètre pour la fonction suivante
    strcpy(buffer, "arrivée");
    // Saisie, envoi et vérification de la ville d'arrivée
    ville_existe(socket, buffer);
    lecture_et_envoie_plage_horaire(socket);
    read(socket, &array_size, sizeof(int));
    if (array_size == 0) {
        printf("Navrés, aucun train n'est disponible dans la tranche horaire selectionnée.\n");
        return 1;
    }
    lecture_n_trajet(trajets_trouves, array_size, socket);
    for (int i = 0; i < array_size; i++)
    {
        affiche_trajet(trajets_trouves[i]);
    }
    return 0;
}

int requete_choix_trois(int socket)
{
    char buffer[MAX_SIZE_STRING] = "";
    struct trajet struc_buffer;
    struct trajet trajets_trouves[MAX_SIZE_STRING];
    int array_size = 0;
    int array_index = 0;
    int choice = -1;


    // Définition du paramètre pour la fonction suivante
    strcpy(buffer, "départ");
    // Saisie, envoi et vérification de la ville de départ
    ville_existe(socket, buffer);

    // Définition du paramètre pour la fonction suivante
    strcpy(buffer, "arrivée");
    // Saisie, envoi et vérification de la ville d'arrivée
    ville_existe(socket, buffer);
    read(socket, &array_size, sizeof(int));
    lecture_n_trajet(trajets_trouves, array_size, socket);
    for (int i = 0; i < array_size; i++)
    {
        affiche_trajet(trajets_trouves[i]);
    }
    printf("Lequel souhaitez-vous sélectioner ? 1 - Trajet le plus rapide | 2 - Trajet le moins cher : ");
    scanf("%d", &choice);
    write(socket, &choice, sizeof(int));
    read(socket, &array_index, sizeof(int));
    affiche_trajet(trajets_trouves[array_index]);
    return 0;
}

int affichage_menu_principal() {
    printf("----------------------------------------------------\n");
    printf("                     MAIN MENU                      \n");
    printf("\n");
    printf("Souhaitez-vous ?\n");
    printf("1. Trouver un train selon un itinéraire et un horaire de départ\n");
    printf("\n");
    printf("2. Afficher tous les trains possibles selon une tranche horaire\n");
    printf("\n");
    printf("3. Trouver le meilleur train selon un itinéraire et des critères (prix ou durée)\n");
    printf("\n");
    printf("4. Quitter\n");
    printf("\n");
    printf("----------------------------------------------------\n");
    printf("Votre choix : ");

    return 0;
}

int choix_utilisateur_menu_principal() {
    affichage_menu_principal();
    char buffer[MAX_SIZE_STRING];
    fgets(buffer, MAX_SIZE_STRING, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    int choix = atoi(buffer);
    while (choix != 1 && choix != 2 && choix != 3 && choix != 4) {
        printf("Erreur de saisie, entrez 1 , 2 , 3 ou 4 : ");
        fgets(buffer, MAX_SIZE_STRING, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        printf("\n");
        choix = atoi(buffer);
    }
    printf("\n");
    return choix;
}

int branchement_selon_choix_principal_client(int choix, int socket) {
    switch (choix) {
        case 1:
            write(socket, &choix, sizeof(int));
            requete_choix_un(socket);
            break;
        case 2:
            write(socket, &choix, sizeof(int));
            requete_choix_deux(socket);
            break;
        case 3:
            write(socket, &choix, sizeof(int));
            requete_choix_trois(socket);
            break;
        case 4:
            printf("Merci et au revoir.\n");
            close(socket);
            exit(0);
        default:
            printf("Comment êtes-vous arrivé(e) ici...?\n");
            close(socket);
            exit(-1);
    }
    return 0;
}

int branchement_selon_choix_principal_serveur(int choix, int socket, FILE* file, struct tableaux tableaux_ville) {
    switch (choix) {
        case 1:
            exec_choix_un(socket, file, tableaux_ville);
            break;
        case 2:
            exec_choix_deux(socket, file, tableaux_ville);
            break;
        case 3:
            exec_choix_trois(socket, file, tableaux_ville);
            break;
        default:
            printf("Comment êtes-vous arrivé(e) ici...?\n");
            exit(-1);
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

