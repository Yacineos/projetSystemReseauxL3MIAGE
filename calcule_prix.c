// Les include génériques
 #include "stdio.h"
 #include "stdlib.h"
 #include "unistd.h"
 #include "stdbool.h"
 #include "string.h"

struct trajet{
    int num_train;
    char ville_depart[100];
    char ville_arrivee[100];
    char heur_depart[5];
    char heur_arrivee[5];
    float prix;
    char promotion;
};

/*
 * fonction qui calcule les prix suivant la valeur de trajetClient.promotion
 * si 'r' il y a réduction donc on réduit le prix de 20%
 * si 's' il y a supplément donc on rajout au prix 10%
 * sinon on fait rien
 * 
 * @param struct trajet
 * @res int     0 -> erreur 1 -> succes  
 */
int calcule_prix(struct trajet *trajetClient){
    int status = 1 ; // on suppose qu'il y a pas erreur 
    if(trajetClient->prix > 0){
        switch (trajetClient->promotion){
            case 'r':
                trajetClient->prix = trajetClient->prix - ((20 * trajetClient->prix)/ 100);
                break;
            case 's':
                trajetClient->prix = trajetClient->prix + ((10 * trajetClient->prix)/ 100);
                break;
            default:  
            break;
        }
    }else{
        printf("le trajet fournie en paramètre à calcule_prix a un prix inférieur à 0 ! \n");
        status = 0;
    }
    return status;
};

int main(int argc, char *argv[]) {
   //déclaration de la structure trajet
   struct trajet trajetAless;

   //initialisation des membres de la structure trajetAless
   trajetAless.num_train = 04;
   strcpy(trajetAless.ville_depart, "Grenoble");
   strcpy(trajetAless.ville_arrivee, "Valence");
   strcpy(trajetAless.heur_depart, "12:00");
   strcpy(trajetAless.heur_arrivee, "13:25");
   trajetAless.prix = 10;
   trajetAless.promotion = ' ';

    //appele de le fonction calcule_prix
   if(calcule_prix(&trajetAless) != 0){
    //affichage du nouveau prix
    printf("le nouveau prix est : %2f\n", trajetAless.prix);
   }
   

   
    return 0;

}