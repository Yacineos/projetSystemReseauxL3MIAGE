#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <newlib/sys/select.h>
#include <string.h>

int main() {
    FILE* trajets = fopen("trajets.txt", "r");
    char string[1000];
    char v_depart[100];
    char v_arrivee[100];
    char villes_depart[100][100] = { 0 };
    char villes_arrivee[100][100] = { 0 };
    int compteur_vdepart = 0;
    int compteur_varrivee = 0;
    bool ville_existe = false;
    // Parcours tout le fichier jusqu'à la fin
    while(!feof(trajets)) {
        fgets(string,sizeof(string), trajets);
        sscanf(string, "%*[^;];%127[^;]", v_depart);
        // On ne peut pas regarder à l'indice 0 car il n'y a rien (segfault)
        if (compteur_vdepart == 0) {
            strcpy(villes_depart[compteur_vdepart], v_depart);
            ville_existe = true;
            compteur_vdepart++;
        } else {
            // for (int i = 0; i < compteur_vdepart; i++) {
            //     if (strcmp(villes_depart[i], v_depart)) {
            //         ville_existe = true;
            //         break;
            //     }
            // }
            int i = 0;
            while (i < compteur_vdepart && !(ville_existe)) {
                if (strcmp(villes_depart[i], v_depart) == 0) {
                    ville_existe = true;
                }
                i++;
            }
        }
        if (ville_existe == false) {
            strcpy(villes_depart[compteur_vdepart], v_depart);
            compteur_vdepart++;
        }
        ville_existe = false;

        sscanf(string, "%*[^;];%*[^;];%127[^;]", v_arrivee);
        ville_existe = false;
        if (compteur_varrivee == 0) {
            strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
            ville_existe = true;
            compteur_varrivee++;
        } else {
            // for (int i = 0; i < compteur_vdepart; i++) {
            //     if (strcmp(villes_depart[i], v_depart)) {
            //         ville_existe = true;
            //         break;
            //     }
            // }
            int i = 0;
            while (i < compteur_varrivee && !(ville_existe)) {
                if (strcmp(villes_arrivee[i], v_arrivee) == 0) {
                    ville_existe = true;
                }
                i++;
            }
        }
        if (ville_existe == false) {
            strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
            compteur_varrivee++;
        }
        ville_existe = false;
    }
    for (int i = 0; i < compteur_varrivee; i++) {
        printf("%s\n", villes_arrivee[i]);
    }
    // printf("\n");
    fclose(trajets);
    return 0;
}