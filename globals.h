/**
 * Copyright (c) 2015, Team Unknown,
 *                     Ján Mochňak,    <xmochn00@vutbr.cz>
 *                     Tibor Dudlák,   <xdudla00@vutbr.cz>
 *                     Dávid Prexta,   <xprext00@vutbr.cz>
 *                     Martin Krajňák, <xkrajn02@vutbr.cz>
 *                     Patrik Segedy,  <xseged00@vutbr.cz>
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * license.txt file in the root directory of this source tree.
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

typedef enum {
    EOK,
    ELEX,
    ESYN,
    ESEM,
    ESEM2,
    ESEM3,
    ESEM4,
    EEOF,
    ESYS = 99,
} tresult;

#define BASIC   2

/***********ID************/
#define ID      4

/********Key words ****************/
#define AUTO    5
#define CIN     6
#define COUT    7
#define DOUBLE  8
#define ELSE    9
#define FOR     10
#define IF      11
#define INT     12
#define RETURN  13
#define STRING  14
#define MAIN    15

/*********/
#define INT_NUM     16
#define DOUBLE_NUM  17
#define TEXT        18

/*************SYMBOLS*************/

#define SEMICOLON       32      // ;
#define LEFT_CULUM      33      // (
#define RIGHT_CULUM     34      // )
#define LEFT_VINCULUM   35      // {
#define RIGHT_VINCULUM  36      // }
#define COMMA           37      // ,
#define DOT             38      // .
#define EQUALS          39      // ==
#define DBL_ARR_LEFT    40      // <<
#define DBL_ARR_RIGHT   41      // >>
#define LEFT_ARROW      42      // <
#define RIGHT_ARROW     43      // >
#define LESS_OR_EQUAL   44      // <=
#define MORE_OR_EQUAL   45      // >=
#define NOT_EQUAL       46      // !=
#define PLUS            47      // +
#define MINUS           48      // -
#define MULTIPLY        49      // *
#define DEVIDE          51      // /
#define UV              59      // "
#define ASSIGN          60      // =

/*********FUNCTIONS***************/
#define LENGTH  52
#define SUBSTR  53
#define CONCAT  54
#define FIND    55
#define SORT    56

#define END_OF_FILE 50

#define SYS_ERROR 99
#define LEX_ERROR 1

#endif // GLOBALS_H_
