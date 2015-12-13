/**
 * Copyright (c) 2015, Team Unknown,
 *                     Ján Mochňak,    <xmochn00@vutbr.cz>
 *                     Tibor Dudlák,   <xdudla00@vutbr.cz>
 *                     Dávid Prexta,   <xprext00@vutbr.cz>
 *                     Martin Krajňák, <xkrajn02@vutbr.cz>
 *                     Patrik Segedy,  <xseged00@vutbr.cz>
 *
 * This source code is licensed under the MIT license found in the
 * license.txt file in the root directory of this source tree.
 */

#ifndef HUGO_H_
#define HUGO_H_

/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_pop_instr(instruction_t *i) {
    i->type = I_ADD_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_ADD_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_ADD_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_ADD_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_ADD_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_ADD_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_ADD_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_ADD_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_ADD_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_ADD_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_pop_instr(instruction_t *i) {
    i->type = I_SUB_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_SUB_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_SUB_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_SUB_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_SUB_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_SUB_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_SUB_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_SUB_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_SUB_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_SUB_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_pop_instr(instruction_t *i) {
    i->type = I_MUL_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_MUL_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_MUL_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_MUL_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_MUL_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_MUL_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_MUL_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_MUL_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_MUL_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_MUL_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_pop_instr(instruction_t *i) {
    i->type = I_DIV_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_DIV_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_DIV_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_DIV_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_DIV_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_DIV_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_DIV_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_DIV_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_DIV_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_DIV_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_pop_instr(instruction_t *i) {
    i->type = I_LT_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_LT_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_LT_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LT_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LT_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_LT_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LT_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_LT_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LT_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LT_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_pop_instr(instruction_t *i) {
    i->type = I_GT_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_GT_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_GT_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GT_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GT_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_GT_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GT_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_GT_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GT_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GT_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_pop_instr(instruction_t *i) {
    i->type = I_LE_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_LE_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_LE_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LE_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LE_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_LE_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LE_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_LE_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_LE_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_LE_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_pop_instr(instruction_t *i) {
    i->type = I_GE_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_GE_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_GE_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GE_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GE_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_GE_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GE_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_GE_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_GE_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GE_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_pop_instr(instruction_t *i) {
    i->type = I_EQ_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_EQ_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_EQ_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_EQ_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_EQ_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_EQ_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_EQ_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_EQ_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_EQ_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_EQ_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_pop_instr(instruction_t *i) {
    i->type = I_NQ_pop;



    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_pop_zval_instr(instruction_t *i, zval_t *a) {
    i->type = I_NQ_pop_zval;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_pop_offset_instr(instruction_t *i, zval_t *a) {
    i->type = I_NQ_pop_offset;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_NQ_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_offset_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_NQ_offset_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_offset_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_NQ_offset_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_NQ_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_zval_pop_instr(instruction_t *i, zval_t *a) {
    i->type = I_NQ_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * Do not change! Generated function
 */
INLINED result_t create_NQ_zval_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_NQ_zval_offset;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->first, a);

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}

#endif // HUGO_H_

