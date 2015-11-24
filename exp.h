#ifndef EXP_H_
#define EXP_H_

#include <stdio.h>
#include "scaner.h"

typedef enum
{
    L,   //LESS
    M,   //MORE
    E,  //EQUAL
    O,   //EMPTY, ERR
}opRules;

typedef enum
{
    Op_PLUS,        //0  +
    Op_MINUS,       //1  -
    Op_MUL,         //2  *
    Op_DIV,         //3  /
    Op_LESS,        //4  <
    Op_MORE,        //5  >
    Op_LEQ,        //6  <=
    Op_MEQ,         //7  >=
    Op_EQ,          //8  ==
    Op_NEQ,         //9  !=
    Op_LB,          //10  left bracket
    Op_RB,          //11 right bracket
    Op_VAR,         //12 variable
    Op_DOL,         //13 dollar
    Op_ERR,         //14 Error
}Operators;

typedef struct
{
    int top;
    char array[256];
}TStack;

#endif // EXP_H_
