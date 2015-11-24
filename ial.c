#include "ial.h"

#define END 1

int length(char* input)
{
    int input_lenght=0;
    while (input[input_lenght] != '\0')     // prechadzanie pola charov pokial neporovnavam ukoncovaci znak /0
    {
        input_lenght++;                     // inkrementacia premennej ktora drzi dlzku retazca
    }
    return input_lenght;
}

char* substr(char* input, size_t start, size_t count)
{
    //   ak dlzka retazca je vacsia ako sucet parametrov, nieje nula    a sucet parametrov je vacsi ako 1
    if ( ( start+count > 1 ) && ( length(input) + END >= start + count ) )
    {
        char *output = malloc( (count + END) *sizeof(char) );                // alokacia pamate pre pomocnu premennu
        for (int i = 0; i < count; ++i)
        {
            output[i] = input[start - END + i];                         // prechadzanie pola podla parametrov od start po start+count
        }
        output[count]='\0';                                             // pridanie koncoveho znaku do pola znakov
        return output;
    }
    else                                                                // ak neplati podmienka vraciame vstup
    {
        char *output = malloc( (length(input) + END) *sizeof(char) );   // teda alokujeme premennu taku velku ako je vstup
        strcpy(output, input);                                          // prekopirovanie obsahov - ide aj cez for
        return output;
    }   // vraciame stale naalokovanu pamat aby volajuca funkcia mohla v oboch pripadoch po zavolani spravit free();
}

char* concat(char* input1 , char* input2)
{
    int i = 0, output_lenght = (length(input1) + length(input2) + END); // inicializacia pomocnych premennych
    char* output = malloc(output_lenght * sizeof(char));                // naalokovanie potrebneho miesta premennej, ktoru vraciame
    while (i != output_lenght)                                          // pokial neprekrocime dlzku vysledneho retazca
    {
        if ( i < length(input1) )
            output[i] = input1[i];                                      // priradime tam hodnotu z prveho stringu ak je i < ako dlzka prveho
        else
            output[i] = input2[i - length(input1)];                     // naopak hodnotu z druheho z pozicie i-dlzka prveho
        i++;
    }
    output[output_lenght-END] = '\0';                                   // na konci pridame /0
    return output;                                                      // volajuca funkcia musi pouzit free();
}
/*int find(char*, char*)
{
    BY_DAVID_DEVAD
}

char* sort(char*)
{
    BY_DAVID_DEVAD
}*/
