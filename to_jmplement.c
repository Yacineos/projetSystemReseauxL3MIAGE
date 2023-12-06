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
    char buffer[256];
    memset(buffer , 0 , 256);
    if(read(socket,&buffer,256) == -1){
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
int test_horaire(char horaire[256]) {
    // Vérifier la longueur de l'horaire
    if (strlen(horaire) != 5) {
        return 1; // Longueur incorrecte
    }

    // Extraire les heures et minutes
    int heures, minutes;
    if (sscanf(horaire, "%d:%d", &heures, &minutes) != 2) {
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

void lecture_horaire(char horaire[256]){
    do{
        printf("Veuillez entrer un horaire valide du format (hh:mm): \n");
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

int envoie_horaire(char horaire[256] , int socket){

    char buffer[256] ;
    memset(buffer , 0 , 256);
    strcpy(buffer,horaire);
    if(write(socket , &buffer , 256) == -1){
        perror("erreur dans l'envoie de l'heure");
        return 1 ;
    }
    return 0;
}