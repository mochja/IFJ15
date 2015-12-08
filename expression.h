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

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "klist.h"
#include "zval.h"
#include "token.h"

#define EXPR_IS_OPERAND(x)  (((x)->flags >> 0) & 1)
#define EXPR_IS_OFFSET(x)   (((x)->flags >> 1) & 1)
#define EXPR_IS_INT(x)      (((x)->flags >> 2) & 1)
#define EXPR_IS_DOUBLE(x)   (((x)->flags >> 3) & 1)
#define EXPR_IS_STRING(x)   (((x)->flags >> 4) & 1)

#define EXPR_SET_OPERAND(x, v)  (x)->flags = 0x00 | 1 << 0; (x)->op_t = v;
#define EXPR_SET_OFFSET(x, v)   (x)->flags = 0x00 | 1 << 1; (x)->val.iVal = v;
#define EXPR_SET_INT(x, v)      (x)->flags = 0x00 | 1 << 2; zval_set(&(x)->val, v);
#define EXPR_SET_DOUBLE(x, v)   (x)->flags = 0x00 | 1 << 3; (x)->val.dVal = v;
#define EXPR_SET_STRING(x, v)   (x)->flags = 0x00 | 1 << 4; (x)->val.sVal = v;

#define EXPR_GET_OPERAND(x)  ((x)->op_t)
#define EXPR_GET_OFFSET(x)   ((x)->val.iVal)
#define EXPR_GET_INT(x)      ((x)->val.iVal)
#define EXPR_GET_DOUBLE(x)   ((x)->val.dVal)
#define EXPR_GET_STRING(x)   ((x)->val.sVal)

typedef struct __expr_t expr_t;

struct __expr_t {
    unsigned short flags;

    union {
        unsigned int op_t;
        zval_t val;
    };
};

#define __expr_t_free(x)
KLIST_INIT(expr_stack, expr_t*, __expr_t_free)

const static enum {
    L, M, E, O
} __op;

const static unsigned char __table[][14] = {
/*           0   1   2   3   4   5   6   7   8   9   10  11  12  13   */
/*           +   -   *   /   <   >   <=  >=  ==  !=  (   )   a   $    */
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

const static enum {
    Op_PLUS = 0,    //0  +
    Op_MINUS,       //1  -
    Op_MUL,         //2  *
    Op_DIV,         //3  /
    Op_LESS,        //4  <
    Op_MORE,        //5  >
    Op_LEQ,         //6  <=
    Op_MEQ,         //7  >=
    Op_EQ,          //8  ==
    Op_NEQ,         //9  !=
    Op_LB,          //10  left bracket
    Op_RB,          //11 right bracket
    Op_VAR,         //12 variable
    Op_DOL,         //13 dollar
    Op_ERR,         //14 Error
} __op_code;

klist_t(expr_stack)* build_expression(klist_t(token_list) *tokens);

#endif // EXPRESSION_H_
