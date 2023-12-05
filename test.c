// Les include génériques
 #include "stdio.h"
 #include "stdlib.h"
 #include "unistd.h"
 #include "stdbool.h"
 #include "string.h"

// Les include pour traiter le signal
#include <sys/wait.h>
#include <newlib/sys/select.h>
#include <signal.h>

 // Include des fonctions et types fondamentaux  
 #include "sys/socket.h"
 #include "sys/types.h"

 // Include des macros
 #include "netinet/in.h"

struct trajet {
    int num_train;
    char ville_d[100];
    char ville_a[100];
    char heure_d[5];
    char heure_a[5];
    float prix;
    char promotion;
};

// structure tableau qui contient le tableau d'une taille max
// avec le nombre d'element qui y a dans le tableau 
struct tableau{
    int n ; // taille du tableau 
    char tab[100][100] ; // tableau d'entier
};

struct tableaux{
    struct tableau tabVilleDepart;
    struct tableau tabVilleArrivee;
};


// Si la valeur est déjà dans le tableau renvoie true, sinon renvoie false
bool est_dans_tableau(int size, char tab_villes[100][100], char ville[100]) {
    for (int i = 0; i < size; i++) {
        if (strcmp(tab_villes[i], ville) == 0) {
            return true;
        }
    }
    return false;
}

int init_v_depart(int compteur_vdepart, char v_depart[100], bool ville_existe, char villes_depart[100][100]){
    // On ne peut pas regarder à l'indice 0 car il n'y a rien (segfault)
    if (compteur_vdepart == 0) {
        strcpy(villes_depart[compteur_vdepart], v_depart);
        ville_existe = true;
        compteur_vdepart++;
    } else {
        ville_existe = est_dans_tableau(compteur_vdepart, villes_depart, v_depart);
    }
    if (ville_existe == false) {
        strcpy(villes_depart[compteur_vdepart], v_depart);
        compteur_vdepart++;
    }
    // On retourne la taille du tableau
    return compteur_vdepart;
}

int init_v_arrivee(int compteur_varrivee, char v_arrivee[100], bool ville_existe, char villes_arrivee[100][100]) {
    if (compteur_varrivee == 0) {
        strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
        ville_existe = true;
        compteur_varrivee++;
    } else {
        ville_existe = est_dans_tableau(compteur_varrivee, villes_arrivee, v_arrivee);
    }
    if (ville_existe == false) {
        strcpy(villes_arrivee[compteur_varrivee], v_arrivee);
        compteur_varrivee++;
    }
    // On retourne la taille du tableau
    return compteur_varrivee;
}

// Initialise 2 tableaux comportant respectivement les villes de départ et d'arrivée
struct tableaux server_data_init(FILE* file) {
    char string[1000];
    char v_depart[100];
    char v_arrivee[100];
    char villes_depart[100][100] = { 0 };
    char villes_arrivee[100][100] = { 0 };
    int compteur_vdepart = 0;
    int compteur_varrivee = 0;
    bool ville_existe = false;
    // Parcours tout le fichier jusqu'à la fin
    while(!feof(file)) {
        fgets(string,sizeof(string), file);
        sscanf(string, "%*[^;];%127[^;]", v_depart);
        // Boucle départs
        compteur_vdepart = init_v_depart(compteur_vdepart, v_depart, ville_existe, villes_depart);
        // Reset du booléen pour pouvoir analyser les arrivées
        ville_existe = false;
        sscanf(string, "%*[^;];%*[^;];%127[^;]", v_arrivee);
        // Boucle arrivées
        compteur_varrivee = init_v_arrivee(compteur_varrivee, v_arrivee, ville_existe, villes_arrivee);
        // Reset du booléen pour pouvoir analyser les départs
        ville_existe = false;
    }
    struct tableau tableau_vdepart = {0};
    memcpy(tableau_vdepart.tab, villes_depart, sizeof(villes_depart));
    tableau_vdepart.n = compteur_vdepart;
    struct tableau tableau_varrivee = {0};
    memcpy(tableau_varrivee.tab, villes_arrivee, sizeof(villes_arrivee));
    tableau_varrivee.n = compteur_varrivee;
    struct tableaux tableaux_villes;
    tableaux_villes.tabVilleArrivee = tableau_varrivee;
    tableaux_villes.tabVilleDepart = tableau_vdepart;
    return tableaux_villes;
} 

int main() {
    FILE* trajets = fopen("trajets.txt", "r");
    struct tableaux tableaux_villes = server_data_init(trajets);
    for (int i = 0; i < tableaux_villes.tabVilleDepart.n; i++) {
        printf("%s\n", tableaux_villes.tabVilleDepart.tab[i]);
    }
    for (int i = 0; i < tableaux_villes.tabVilleArrivee.n; i++) {
        printf("%s\n", tableaux_villes.tabVilleArrivee.tab[i]);
    }
    fclose(trajets);
    return 0;
}