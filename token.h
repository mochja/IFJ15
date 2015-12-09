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

#ifndef TOKEN_H_
#define TOKEN_H_

#include "klist.h"
#include "zval.h"
#include "globals.h"
#include "list.h"

/********Key words ****************/
#define AUTO_KW         0x02U
#define CIN_KW          0x04U
#define COUT_KW         0x08U
#define DOUBLE_KW       0x10U
#define ELSE_KW         0x20U
#define FOR_KW          0x40U
#define IF_KW           0x80U
#define INT_KW          0x100U
#define RETURN_KW       0x200U
#define STRING_KW       0x400U
#define MAIN_KW         0x800U

/* CONTANT TYPES */
#define INT_CONST       0x02U
#define DOUBLE_CONST    0x04U
#define TEXT_CONST      0x08U

/* VAR TOKEN TYPE */
#define OFFSET_ID       0x02U

/*************SYMBOLS*************/
#define SEMICOLON_SMBL       0x02U
#define LEFT_CULUM_SMBL      0x04U
#define RIGHT_CULUM_SMBL     0x08U
#define LEFT_VINCULUM_SMBL   0x10U
#define RIGHT_VINCULUM_SMBL  0x20U
#define COMMA_SMBL           0x40U
#define DOT_SMBL             0x80U
#define EQUALS_SMBL          0x100U
#define DBL_ARR_LEFT_SMBL    0x200U
#define DBL_ARR_RIGHT_SMBL   0x400U
#define LEFT_ARROW_SMBL      0x800U
#define RIGHT_ARROW_SMBL     0x1000U
#define LESS_OR_EQUAL_SMBL   0x2000U
#define MORE_OR_EQUAL_SMBL   0x4000U
#define NOT_EQUAL_SMBL       0x8000U
#define PLUS_SMBL            0x10000U
#define MINUS_SMBL           0x20000U
#define MULTIPLY_SMBL        0x40000U
#define DEVIDE_SMBL          0x80000U
#define UV_SMBL              0x100000U
#define ASSIGN_SMBL          0x200000U

/*********FUNCTIONS***************/
#define LENGTH_FN       0x02U
#define SUBSTR_FN       0x04U
#define CONCAT_FN       0x08U
#define FIND_FN         0x10U
#define SORT_FN         0x20U

/**
 * TOKEN TYPES
 */
#define SMBL_TYPE       0x01
#define KW_TYPE         0x02
#define ID_TYPE         0x04
#define FN_TYPE         0x08
#define BASIC_TYPE      0x10
#define EOF_TYPE        0x20
#define CONST_TYPE      0x40

#define TOKEN_HAS_TFLAG(x, t, f)        ((((x)->type & (t)) == (t)) && ((x)->flags & (f)))
#define TOKEN_HAS_FLAG(x, f)            (((x)->flags & f))
#define TOKEN_IS(x, t)                  (((x)->type & (t)) == t)

#define __token_set(x, tt, ff, v)                       \
    (x)->type = tt; (x)->flags = ff;                    \
    zval_set(&(x)->data, v)

#define TOKEN_SET_CONST_INT(t, v)                       \
    __token_set((t), CONST_TYPE, INT_CONST, (v))

typedef struct __token_t token_t;

struct __token_t {
    unsigned char type;
    unsigned int flags;

    zval_t data;
};

#define __token_t_free(x)
KLIST_INIT(token_list, token_t*, __token_t_free)

INLINED void token_set_type(token_t *dest, const unsigned char type) {
    dest->type = type;
}

INLINED void token_set_symbol(token_t *dest, const unsigned int flags) {
    dest->type = SMBL_TYPE;
    dest->flags = flags;
    zval_init(&dest->data);
}

INLINED void token_set_kw(token_t *dest, const unsigned int flags) {
    dest->type = KW_TYPE;
    dest->flags = flags;
    zval_init(&dest->data);
}

INLINED void token_set_fn(token_t *dest, const unsigned int flags) {
    dest->type = FN_TYPE;
    dest->flags = flags;
    zval_init(&dest->data);
}

INLINED void token_set_var(token_t *dest, const char *name) {
    dest->type = ID_TYPE;
    dest->flags = 0;
    zval_set(&dest->data, name);
}

INLINED void token_set_text_const(token_t *dest, const char *val) {
    dest->type = CONST_TYPE;
    dest->flags = TEXT_CONST;
    zval_set(&dest->data, val);
}

INLINED void token_set_int_const(token_t *dest, int val) {
    dest->type = CONST_TYPE;
    dest->flags = INT_CONST;
    zval_set(&dest->data, val);
}

INLINED void token_set_double_const(token_t *dest, double val) {
    dest->type = CONST_TYPE;
    dest->flags = DOUBLE_CONST;
    zval_set(&dest->data, val);
}

INLINED void token_set_flags(token_t *dest, const unsigned int flags) {
    dest->flags = flags;
}

INLINED result_t token_init(token_t *dest) {
    token_set_type(dest, BASIC_TYPE);
    zval_init(&dest->data);
    return EOK;
}

INLINED result_t token_copy(token_t *dest, token_t *src) {

    dest->type = src->type;
    dest->flags = src->flags;

    return zval_copy(&dest->data, &src->data);
}

INLINED void token_dispose(token_t *t) {

    t->type = BASIC_TYPE;
    t->flags = 0;

    zval_dispose(&t->data);
}

#endif // TOKEN_H_
