
typedef struct HTS_221         // Values Temp und Press in Hex
{
    double timestamp; // Zeitstempel in Sekunden (LINUX)
    int Jahr;
    int Monat;
    int Tag;
    int Stunde;
    int Minute;
    int Sekunde;
    int TEMP_OUT_L;  // Temperatur LSB   (°F)
    int TEMP_OUT_H;     // Temperatur MSB
    float Temperatur;
    int HUM_OUT_L;   // Feuchtikeit LSB       (%)
    int HUM_OUT_H;   // Feuchtigkeit MSB
    float Feuchtigkeit;

} HTS_221;

void print_struct_array(struct HTS_221*, int );

struct HTS_221 *read_source(char *);

void write_struct_array(struct HTS_221*, char *, int );

int count_HTS(struct HTS_221*);

void UnixzeitNachDatum(unsigned long int ,int*, int*, int*,int*, int*, int*);

void arraytimecalc(struct HTS_221*, int );

void HUM_CALCULATION(struct HTS_221*, int);

void TEMP_CALCULATION(struct HTS_221*, int);
