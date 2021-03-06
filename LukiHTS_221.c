#include <stdlib.h>
#include <string.h>
#define BUFLEN 1024
#include "HTS_221.h"



void print_struct_array(struct HTS_221 *temp_HTS, int numb_HTS) //3     // Ausgabe von numb_Records
{
    for (int numb = 1; numb<numb_HTS; numb++)
    {
        printf(" %f, %d, %d, %d, %d, %d \n",temp_HTS[numb-1].timestamp,temp_HTS[numb-1].TEMP_OUT_L,temp_HTS[numb-1].TEMP_OUT_H,temp_HTS[numb-1].HUM_OUT_L,temp_HTS[numb-1].HUM_OUT_H);
    }

}

struct HTS_221 *read_source(char *filename_read) //1         // Einlesen der Daten und Speichern in struct Array
{

    char line[BUFLEN];

    char field_delim[] = ";";
    char *p_field_search;                               // pointer f�r die Funktion strtok

    int numb = 1;                                       // number of Record

    struct HTS_221 *temp_HTS = malloc(sizeof(struct HTS_221));    // Speicher f�r das erste Feld im Array
    if (temp_HTS == NULL)
    {
        printf("No memory available");
        exit(-1);
    }

    FILE *open_file = fopen(filename_read, "r");
    if (NULL==open_file)
    {
        printf("could not open %s",open_file);
        exit(-1);
    }


    while (fgets(line, BUFLEN, open_file)!=NULL)                       // zeilenweises Einlesen - ACHTUNG auf line Delimiter im Source File
    {
        //printf("%s",line);
        temp_HTS = realloc(temp_HTS,numb*sizeof(struct HTS_221));      // vergr��ern des Speichers bei jedem Record
        if (temp_HTS == NULL)
        {
            printf("No memory available");
            exit(-1);
        }

        p_field_search = strtok(line,field_delim);                           // 1. Feld in line
        temp_HTS[numb-1].timestamp = atof(p_field_search);

        p_field_search = strtok(NULL,field_delim);
        temp_HTS[numb-1].TEMP_OUT_L = (int)strtol(p_field_search, NULL, 16);

        p_field_search = strtok(NULL,field_delim);
        temp_HTS[numb-1].TEMP_OUT_H = (int)strtol(p_field_search, NULL, 16);

        p_field_search = strtok(NULL,field_delim);
        temp_HTS[numb-1].HUM_OUT_L = (int)strtol(p_field_search, NULL, 16);

        p_field_search = strtok(NULL,field_delim);
        temp_HTS[numb-1].HUM_OUT_H = (int)strtol(p_field_search, NULL, 16);


        numb++;
    }


    fclose(open_file);

    temp_HTS = realloc(temp_HTS,numb*sizeof(struct HTS_221));
    temp_HTS[numb-1].timestamp = 0.0;                            // sicherstellen, dass am Ende des Arrays 0.0 im timestamp steht

    //print_struct_array(temp_Record, numb);
    return(temp_HTS);
}

void write_struct_array(struct HTS_221 *temp_HTS, char *filename_write, int numb_HTS)//4
{
    FILE *open_file = fopen(filename_write, "w+");
    if (NULL==open_file)
    {
        printf("could not open %s",open_file);
        exit(-1);
    }

    for (int numb = 1; numb<=numb_HTS; numb++)
    {
        fprintf(open_file,"%d.%d.%d %d:%d:%d ; %.2f%%  ;%.2f�C\n",temp_HTS[numb-1].Jahr,temp_HTS[numb-1].Monat,temp_HTS[numb-1].Tag,temp_HTS[numb-1].Stunde,temp_HTS[numb-1].Minute,temp_HTS[numb-1].Sekunde,temp_HTS[numb-1].Feuchtigkeit,temp_HTS[numb-1].Temperatur);
    }
    fclose(open_file);
}

int count_HTS(struct HTS_221 *All_HTS) //2
{
    int i = 0;
    while(All_HTS[i].timestamp > 0.0)       // ACHTUNG!! setzt voraus, dass wirklich 0.0 im letzten timestamp steht
    {
        i++;
    }
    return i;
}

void UnixzeitNachDatum(unsigned long int unixtime,int *pJahr, int *pMonat, int *pTag,int *pStunde, int *pMinute, int *pSekunde)
{
    const unsigned long int SEKUNDEN_PRO_TAG   =  86400ul; /*  24* 60 * 60 */
    const unsigned long int TAGE_IM_GEMEINJAHR =    365ul; /* kein Schaltjahr */
    const unsigned long int TAGE_IN_4_JAHREN   =   1461ul; /*   4*365 +   1 */
    const unsigned long int TAGE_IN_100_JAHREN =  36524ul; /* 100*365 +  25 - 1 */
    const unsigned long int TAGE_IN_400_JAHREN = 146097ul; /* 400*365 + 100 - 4 + 1 */
    const unsigned long int TAGN_AD_1970_01_01 = 719468ul; /* Tagnummer bezogen auf den 1. Maerz des Jahres "Null" */

    unsigned long int TagN = TAGN_AD_1970_01_01 + unixtime/SEKUNDEN_PRO_TAG;
    unsigned long int Sekunden_seit_Mitternacht = unixtime%SEKUNDEN_PRO_TAG;
    unsigned long int temp;

    /* Schaltjahrregel des Gregorianischen Kalenders:
       Jedes durch 100 teilbare Jahr ist kein Schaltjahr, es sei denn, es ist durch 400 teilbar. */
    temp = 4 * (TagN + TAGE_IN_100_JAHREN + 1) / TAGE_IN_400_JAHREN - 1;
    *pJahr = 100 * temp;
    TagN -= TAGE_IN_100_JAHREN * temp + temp / 4;

    /* Schaltjahrregel des Julianischen Kalenders:
       Jedes durch 4 teilbare Jahr ist ein Schaltjahr. */
    temp = 4 * (TagN + TAGE_IM_GEMEINJAHR + 1) / TAGE_IN_4_JAHREN - 1;
    *pJahr += temp;
    TagN -= TAGE_IM_GEMEINJAHR * temp + temp / 4;

    /* TagN enthaelt jetzt nur noch die Tage des errechneten Jahres bezogen auf den 1. Maerz. */
    *pMonat = (5 * TagN + 2) / 153;
    *pTag = TagN - (*pMonat * 153 + 2) / 5 + 1;


    *pMonat += 3;
    if (*pMonat > 12)
    {
        *pMonat -= 12;
        ++*pJahr;
    }

    *pStunde  = Sekunden_seit_Mitternacht / 3600;
    *pMinute  = Sekunden_seit_Mitternacht % 3600 / 60;
    *pSekunde = Sekunden_seit_Mitternacht % 60;
}

void arraytimecalc(struct HTS_221 *temp_HTS, int numb_HTS) // Array zeitstempel umrechnung
{
    for (int numb = 0; numb<numb_HTS; numb++)
    {
        //Variablen f�r Unixzeit///////////////////////
        unsigned long int unixtime=temp_HTS[numb].timestamp+7200;  //Timestamp + 2 Stunden f�r EU Zeit
        int *pJahr = &temp_HTS[numb].Jahr;                         //Pointer auf Struckt variablen
        int *pMonat= &temp_HTS[numb].Monat;                        //
        int *pTag= &temp_HTS[numb].Tag;                            //
        int *pStunde= &temp_HTS[numb].Stunde;                      //
        int *pMinute= &temp_HTS[numb].Minute;                      //
        int *pSekunde= &temp_HTS[numb].Sekunde;                    //
        ///////////////////////////////////////////////
        UnixzeitNachDatum(unixtime,pJahr, pMonat, pTag, pStunde, pMinute, pSekunde);    // Zeit umrechnen und in Array schreiben
    }

}

void HUM_CALCULATION(struct HTS_221 *temp_HTS, int numb_HTS){
    // Formel laut Datenblatt


    for (int numb = 0; numb<numb_HTS; numb++){

        //Parameter
        float h0_out_rh = 0x44;
        float h1_out_rh = 0x86;
        float H0_T_out= 0x0005;
        float H1_T_out= 0xd9aa;
        float H_T_out = (temp_HTS[numb].HUM_OUT_H<<8)|temp_HTS[numb].HUM_OUT_L;

        temp_HTS[numb].Feuchtigkeit= ((((h1_out_rh-h0_out_rh)/2)/(H1_T_out-H0_T_out))*(H_T_out-H0_T_out)+(h0_out_rh/2));

    }
}


void TEMP_CALCULATION(struct HTS_221 *temp_HTS, int numb_HTS){
    // Formel laut Datenblatt


    for (int numb = 0; numb<numb_HTS; numb++){

        //Parameter
        float T1_deg_cx8 = ((0xc4 &0x3 )<<8) | 0x9e ;
        float T0_deg_cx8 = ((0xc4 &0xc )<<6) | 0x9  ;
        float T1_out = 0xfffb;
        float T0_out = 0x02d5;
        float T_out = (temp_HTS[numb].TEMP_OUT_H<<8)|temp_HTS[numb].TEMP_OUT_L;

        temp_HTS[numb].Temperatur = ((((T1_deg_cx8-T0_deg_cx8)/8)/(T1_out-T0_out))*(T_out-T0_out)+(T0_deg_cx8)/8);

    }
}



