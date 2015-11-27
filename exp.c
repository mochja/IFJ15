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
 * LICENSE file in the root directory of this source tree.
 */

#include "exp.h"


const opRules PTable[14][14] ={
/*           0   1   2   3   4   5   6   7   8   9   10  11  12  13
/*           +   -   *   /   <   >   <=  >=  ==  !=  (   )   a   $
/*| 0  + |*/{M,  M,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 1 -  |*/{M,  M,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 2 *  |*/{M,  M,  M,  M,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 3 /  |*/{M,  M,  M,  M,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 4 <  |*/{L,  L,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 5 >  |*/{L,  L,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 6 <= |*/{L,  L,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 7 >= |*/{L,  L,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 8 == |*/{L,  L,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 9 != |*/{L,  L,  L,  L,  M,  M,  M,  M,  M,  M,  L,  M,  L,  M, },
/*| 10 ( |*/{L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  E,  L,  O, },
/*| 11 ) |*/{M,  M,  M,  M,  M,  M,  M,  M,  M,  M,  O,  M,  O,  M, },
/*| 12 a |*/{M,  M,  M,  M,  M,  M,  M,  M,  M,  M,  O,  M,  O,  M, },
/*| 13 $ |*/{L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  O,  L,  O, },
};


opRules getRule(struct T_Token t)
{
    if(t.result != EOK) fprintf(stderr, "ERR%s\n");

    switch(t.type){
        case PLUS:              printf("PLUS\n");
        break;
        case MINUS:             printf("MINUS\n");
        break;
        case MULTIPLY:          printf("MULTIPLY\n");
        break;
        case DEVIDE:            printf("DIVIDE\n");
        break;
        case EQUALS:             printf("EQUALS\n");
        break;
        case NOT_EQUAL:         printf("NOT_EQUAL\n");
        break;
        case LESS_OR_EQUAL:      printf("PLUS\n");
        break;
        case MORE_OR_EQUAL:      printf("PLUS\n");
        break;
        case LEFT_CULUM:        printf("LEFT_CULUM\n");
        break;
        case RIGHT_CULUM:      printf("RIGHT_VINCULUM\n");
        break;
        default: printf("ER\n");
    }

}

int main() {
    struct T_Token A;    A.type = LEFT_CULUM;     A.data.s = "(";   A.result = EOK;
    struct T_Token B;    B.type = INT;     B.data.s = "a";   B.result = EOK;
    struct T_Token C;    C.type = PLUS;     C.data.s = "+";   C.result = EOK;
    struct T_Token D;    D.type = INT;     D.data.s = "b";   D.result = EOK;
    struct T_Token E;    E.type = RIGHT_CULUM;     E.data.s = ")";   E.result = EOK;

    printf("%s",A.data.s );
    printf("%s",B.data.s );
    printf("%s",C.data.s );
    printf("%s",D.data.s );
    printf("%s",E.data.s );
    printf("\n");
    TStack stack;
    stack.top = 0;

    getRule(A);
    getRule(B);
    getRule(C);
    getRule(D);
    getRule(E);
    return 0;
 }

