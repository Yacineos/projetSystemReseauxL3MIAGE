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
    char etat_prix;
    };

//fonction qui calcule les prix et élimine la derniére case
float calcule_prix(struct trajet trajetClient){

    switch (trajetClient.etat_prix){
        case 'r':
            trajetClient.prix = trajetClient.prix - ((20 * trajetClient.prix)/ 100);
            break;
        case 's':
            trajetClient.prix = trajetClient.prix + ((10 * trajetClient.prix)/ 100);
            break;
        default:  

        break;
    }

    return trajetClient.prix;
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
   trajetAless.prix = 17.60;
   trajetAless.etat_prix = 'r';

    //appele de le fonction calcule_prix
  float nouveau_prix = calcule_prix(trajetAless);

   //affichage du nouveau prix
   printf("le nouveau prix est : %2f\n", nouveau_prix);
   
    return 0;

}