#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HTS_221.h"

#define BUFLEN 1024


int main()
{
    //Variablen zur Datendatei Einlesen
    char filename_read[] = "Test_Data_10k_HTS221.csv";     // Datei zum Einlesen
    char filename_write[] = "write_test.txt";    // Datei zum Schreiben

    struct HTS_221 *All_HTS;                  // Pointer auf Startadresse des Arrays
    ////////////////////////////////////////////////


    All_HTS = read_source(filename_read);    // Einlesen der Daten

    int numb_HTS = count_HTS (All_HTS);    // Z‰hlen der Records

    arraytimecalc(All_HTS, numb_HTS);          //Umrechnung der Unix zeit in regul‰res Zeitmaﬂ

    HUM_CALCULATION(All_HTS, numb_HTS);         // Umrechnung Feuchtigkeit in %

    TEMP_CALCULATION(All_HTS, numb_HTS);        // Temperatur ausrechnen in ∞C

    write_struct_array(All_HTS, filename_write, numb_HTS);   // Schreiben der Daten in neue Datei

    free(All_HTS);                                // Speicher freigeben

    return 0;
}
