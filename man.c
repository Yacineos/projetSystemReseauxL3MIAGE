#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define MAX_SIZE_STRING 256


/*
    Fonction qui calcule la durée en minutes d’une plage horaire VALIDE 
    donc l'horaire d'arrivée est forcément après l'horaire de départ 

    @param char horaire1[256]
    @param char horaire2[256]

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
    @param char horaire_resultat[256] : La chaîne où le résultat sera écrit

    @return int Retourne 1 en cas d'erreur, 0 sinon
*/
int convertir_duree(int nombre_minutes, char horaire_resultat[256])
{
    if (nombre_minutes < 0)
    {
        // Gestion d'une entrée invalide (nombre de minutes négatif)
        return 1; // false
    }

    int heures = nombre_minutes / 60;
    int minutes = nombre_minutes % 60;

    // Utilisation de snprintf pour formater la chaîne de résultat
    snprintf(horaire_resultat, 256, "%d heures et %d minutes", heures, minutes);

    return 0;
}

void test_duree_trajet(){
     // Test with valid input
    assert(duree_trajet("08:30", "12:45") == 255);  // 4 hours and 15 minutes

    // Test with depart_hour > arrivee_hour
    assert(duree_trajet("14:30", "12:45") == -105);  // Negative value

    // Test with depart_hour == arrivee_hour and depart_minute > arrivee_minute
    assert(duree_trajet("12:45", "12:30") == -15);  // Negative value

    // Test with depart_hour == arrivee_hour and depart_minute < arrivee_minute
    assert(duree_trajet("12:30", "12:45") == 15);  // Positive value

    // Test with depart_hour == arrivee_hour and depart_minute == arrivee_minute
    assert(duree_trajet("12:30", "12:30") == 0);  // Zero duration

    printf("All tests passed successfully.\n");
    printf("All tests passed successfully.\n");
}


void test_convertir_duree() {
    char resultat[256];

    // Test avec une durée positive
    assert(convertir_duree(123, resultat) == 0);
    assert(strcmp(resultat, "2 heures et 3 minutes") == 0);  // Attendu : "2 heures et 3 minutes"

    // Test avec une durée nulle
    assert(convertir_duree(0, resultat) == 0);
    assert(strcmp(resultat, "0 heures et 0 minutes") == 0);  // Attendu : "0 heures et 0 minutes"

    // Test avec une durée d'une heure
    assert(convertir_duree(60, resultat) == 0);
    assert(strcmp(resultat, "1 heures et 0 minutes") == 0);  // Attendu : "1 heures et 0 minutes"

    // Test avec une durée de plusieurs heures
    assert(convertir_duree(245, resultat) == 0);
    assert(strcmp(resultat, "4 heures et 5 minutes") == 0);  // Attendu : "4 heures et 5 minutes"

    // Test avec une durée de plusieurs heures et 0 minute
    assert(convertir_duree(240, resultat) == 0);
    assert(strcmp(resultat, "4 heures et 0 minutes") == 0);  // Attendu : "4 heures et 0 minutes"

    // Test avec une durée négative (devrait retourner une erreur)
    assert(convertir_duree(-30, resultat) == 1);  // Attendu : 1 (erreur)
        printf("All tests passed successfully.\n");

}


int main(){
test_duree_trajet();
test_convertir_duree();
    return 0;
}