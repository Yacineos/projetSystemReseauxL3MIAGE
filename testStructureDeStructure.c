#include <stdio.h>
#include <stdlib.h>

// structure tableau qui contient le tableau d'une taille max
// avec le nombre d'element qui y a dans le tableau 
struct tableau{
    int n ; // taille du tableau 
    int tab[100] ; // tableau d'entier
};

struct tableaux{
    struct tableau tabVilleDepart;
    struct tableau tabVilleArrivee;
};

int main(){
    
    struct tableau tableauDesVillesDeDeparts ;
    tableauDesVillesDeDeparts.n = 0 ;
    tableauDesVillesDeDeparts.tab[0]=44;
    tableauDesVillesDeDeparts.n++;
    tableauDesVillesDeDeparts.tab[1]=55;
    tableauDesVillesDeDeparts.n++;
    tableauDesVillesDeDeparts.tab[2]=66;
    tableauDesVillesDeDeparts.n++;
    


    struct tableau tableauDesVillesDeArrivee ;
    tableauDesVillesDeArrivee.n = 3;
    tableauDesVillesDeArrivee.tab[0]= 22;
    tableauDesVillesDeArrivee.tab[1]= 33;
    tableauDesVillesDeArrivee.tab[2]= 44;

    struct tableaux tabs;
    tabs.tabVilleDepart=tableauDesVillesDeDeparts;
    tabs.tabVilleArrivee=tableauDesVillesDeArrivee;

    printf("la taille du tab dep : %d\n",tabs.tabVilleDepart.n);
    printf("la taille du tab arv : %d\n",tabs.tabVilleArrivee.n);
    printf("la 2 ème valeur du tableau dep : %d\n",tabs.tabVilleDepart.tab[1]);
    printf("la: %d\n",tabs.tabVilleArrivee.tab[0]);
    return 0 ; 

}