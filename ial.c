#include "ial.h"

#define END 1

int length(char* input)
{
    int input_lenght=0;
    while (input[input_lenght] != '\0')
    {
        input_lenght++;
    }
    return input_lenght;
}

char* substr(char* input, size_t start, size_t count)
{
    char *output = malloc((count+END)*sizeof(char));
    if ( length(input) != 0 )
    {
        for (int i = 0; i < count; ++i)
        {
            output[i] = input[start - END + i];
        }
        output[count]='\0';
        return output;
    }
    else
        return input;
}

char* concat(char* input1 , char* input2)
{
    int i = 0, output_lenght = (length(input1) + length(input2) + END);
    char* output = malloc(output_lenght*sizeof(char)) ;
    while (i != output_lenght)
    {
        if ( i < length(input1) )
            output[i] = input1[i];
        else
            output[i] = input2[i - length(input1)];
        i++;
    }
    output[output_lenght-END] = '\0';
    return output;
}
/*int find(char*, char*)
{

}

char* sort(char*)
{

}*/
