#include "ial.h"

/*int length(char* input_str)
{
    int result_lenght=0;
    while (input_str[result_lenght] != '\0')        // prechadzanie pola charov pokial neporovnavam ukoncovaci znak /0
    {
        result_lenght++;                            // inkrementacia premennej ktora drzi dlzku retazca
    }
    return result_lenght;
}*/

char* substr(char* input_str, int start, int size)
{
    //   ak dlzka retazca je vacsia ako sucet parametrov, nieje nula    a sucet parametrov je vacsi ako 1
    if ( ( start+size > 1 ) && ( length(input_str) + 1 >= start + size ) )
    {
        char *result_str = malloc( (size + 1) *sizeof(char) );                 // alokacia pamate pre pomocnu premennu
        for (int i = 0; i < size; ++i)
        {
            result_str[i] = input_str[start - 1 + i];                           // prechadzanie pola podla parametrov od start po start+size
        }
        result_str[size]='\0';                                                 // pridanie koncoveho znaku do pola znakov
        return result_str;
    }
    else                                                                        // ak neplati podmienka vraciame vstup
    {
        char *result_str = malloc( (length(input_str) + 1) *sizeof(char) );     // teda alokujeme premennu taku velku ako je vstup
        strcpy(result_str, input_str);                                          // prekopirovanie obsahov - ide aj cez for
        return result_str;
    }   // vraciame stale naalokovanu pamat aby volajuca funkcia mohla v oboch pripadoch po zavolani spravit free();
}

char* concat(char* str1 , char* str2)
{
    int i = 0, result_str_lenght = (length(str1) + length(str2) + 1);       // inicializacia pomocnych premennych
    char* result_str = malloc(result_str_lenght * sizeof(char));            // naalokovanie potrebneho miesta premennej, ktoru vraciame
    while (i != result_str_lenght)                                          // pokial neprekrocime dlzku vysledneho retazca
    {
        if ( i < length(str1) )
            result_str[i] = str1[i];                                        // priradime tam hodnotu z prveho stringu ak je i < ako dlzka prveho
        else
            result_str[i] = str2[i - length(str1)];                         // naopak hodnotu z druheho z pozicie i-dlzka prveho
        i++;
    }
    result_str[result_str_lenght-1] = '\0';                                 // na konci pridame /0
    return result_str;                                                      // volajuca funkcia musi pouzit free();
}
/*int find(char*, char*)
{
    BY_DAVID_DEVAD
}

char* sort(char*)
{
    BY_DAVID_DEVAD
}*/
