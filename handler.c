#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <newlib/sys/select.h>
#include <signal.h>

// fonction qui sera appelé quand le signal SIGCHLD est reçu
// cette fonction enterine le fils 
void end_child(){
    wait(NULL);
}

/* Programme de test pour tester Le handler
*  création de n processus fils 
*  et redirection du signal sigchild qui est envoyer par le fils au père quand il est mort
*  par redirection on veut dire execution d'une fonction 
*  cette fonction va tout simplement faire un wait() 
*/
int main(void){

    // déclaration d'une structure qui contient les 
    struct sigaction ac;
    // affectation des 
    ac.sa_handler = end_child;
    ac.sa_flags = SA_RESTART;

    // execution du handler au moment où on reçoit SIGCHLD
    sigaction(SIGCHLD, &ac, NULL);

    // on fixe le nombre de processus crée à 5 
    int nombreDeProcessus = 5 ;

    // on fork autant qu'il y a de processus fils
    for(int i = 0 ; i < nombreDeProcessus ; i++){
        // création du fils i
        switch(fork()){
            case -1 :
                break ;
            case 0 :
                sleep(4*i);
                printf("je suis le processus fils : %d\n",(i));
                exit(0);        
            default:
                break;
        }

    }
    //----------------------------------------------------------------------
    // partie code du père 
    // ---------------------------------------------------------------------

    while(1){
        sleep(1);
        printf("Je suis le serveur\n");
    }

    //affichage sur l'écran du id du fils qu'on vient d'enteriner 


    //fin du process père 
    return 0 ;
}