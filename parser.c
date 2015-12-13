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

#include "parser.h"
#include <limits.h>

result_t init_parser(parser_t *parser, char *source) {
    listInit(&parser->varList);
    listInit(&parser->paramList);

    parser->fName[0] = '\0';
    parser->argsCounter = 0;
    parser->argsCounter1 = 0;
    parser->fDeclared = false;
    parser->hInt = 0;
    parser->label = 1;
    parser->assignVarName[0] = '\0';
    parser->offset_counter = 0;
    parser->has_return = false;

    parser->token = calloc(1, sizeof(token_t));
    parser->code = kl_init(instruction_list);

    // Call main function
    instruction_t call;
    create_CALL_instr(&call, 0, 0);
    *kl_pushp(instruction_list, parser->code) = call;

    instruction_t exit;
    create_EXIT_instr(&exit);
    *kl_pushp(instruction_list, parser->code) = exit;

    if ((parser->table = initHashTable(MAX_HTSIZE)) == NULL) {
        fprintf(stderr, "Could not initialize hash table.");
        return ESYS;
    }

    result_t res = init_scanner(&parser->scanner, source);

    inject_native_fn(parser);

    if (res != EOK) {
        fprintf(stderr, "Could not initialize scanner.");
        return res;
    }

    return EOK;
}

result_t parser_dispose(parser_t *parser) {

    listDispose(&parser->varList);
    listDispose(&parser->paramList);

    freeHashTable(parser->table);

    scanner_dispose(&parser->scanner);
    token_dispose(parser->token); free(parser->token);
    kl_destroy(instruction_list, parser->code);

    return EOK;
}

result_t parser_next_token(parser_t *parser) {
    token_dispose(parser->token);
    return scanner_get_next_token(&parser->scanner, parser->token);
}

char __nbuffer[64];
char *generate_var_name(int number) {
    sprintf(__nbuffer, "$%d", number);
    return __nbuffer;
}

struct build_in {
    unsigned int returnType;
    char name[16];
    unsigned int params[8];
};

result_t inject_native_fn(parser_t *parser){
    result_t result;

    static struct build_in list[] = {
        { INT_KW, "length", {STRING_KW, 0x00} },
        { INT_KW, "find", {STRING_KW, STRING_KW, 0x00} },
        { STRING_KW, "substr", {STRING_KW, INT_KW, INT_KW, 0x00} },
        { STRING_KW, "concat", {STRING_KW, STRING_KW, 0x00} },
        { STRING_KW, "sort", {STRING_KW, 0x00} },
        { 0x00, "", {0x00} }
    };

    struct build_in *t = &list[0];
    while (t->returnType != 0x00) {
        hTabItem *tableItem = createNewItem();
        tableItem->name = malloc(sizeof(char) * (strlen(t->name) + 1));
        strcpy(tableItem->name, t->name);
        tableItem->dataType = t->returnType;
        tableItem->isDefined = true;
        tableItem->f_label = ++parser->label;
        tableItem->params = 0;
        insertHashTable(parser->table, tableItem);

        unsigned int *param = t->params;

        tItemPtr item;
        if ((item = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(item->data);
        strncpy(item->functionId, t->name, sizeof(item->functionId));
        int offset = 1;

        while (*param != 0x00U) {
            char *hName = generate_var_name(parser->hInt++);
            hTabItem *tItem;
            tData data;

            if ((result = init_data_var(&data, hName, hName, 0)) != EOK)  {
                debug_print("%s\n", "<");
                return result;
            }
            item_append_data(item, data);

            tItem = createNewItem();
            tItem->name = calloc(1, strlen(hName) + 1);
            strcpy(tItem->name, hName);
            tItem->dataType = *param;
            tItem->paramPosition = offset++;
            insertHashTable(parser->table, tItem);

            param++;
        }

        tableItem->params = (int) kv_size(item->data);
        insertLast(item, &parser->paramList);

        t++;
    }

    return EOK;
}

INLINED result_t offset_of_current_token(parser_t *parser, int *offset) {

    if (varSearch(&parser->varList, parser->token->data.sVal) == NULL) {
        if (paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal) == NULL) {
            return ESEM;
        } else {
            *offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal);
        }
    } else {
        *offset = get_var_offset(&parser->varList, parser->token->data.sVal);
    }

    return EOK;
}




result_t parser_run(parser_t *parser) {
/**overim prvy parser->token**/
    result_t result;

    if ((result = parser_next_token(parser)) != EOK) {
        return result;
    }

    if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))
        return ESYN;

    if ((result = parse_fn(parser)) != EEOF) {
        fprintf(stderr, "Something bad happened.");
        return result;
    }

    tItemPtr tmp = parser->paramList.First;
    while (tmp != NULL) {
        hTabItem * tableItem;
        if ((tableItem = searchItem(parser->table, tmp->functionId)) == NULL)
            return ESYS;
        if (tableItem->isDefined == false)
            return ESEM;
        tmp = tmp->next;
    }

    if (searchItem(parser->table, "main") == NULL) {
        return ESEM;
    }
    return EOK;
}


result_t parse_fn(parser_t *parser) {
    result_t result;

    int fType = parser->token->flags;

    if ((result = parser_next_token(parser)) != EOK) {
        debug_print("%s\n", "<");
        return result;
    }

    if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, MAIN_KW) && fType == INT_KW) {
        /***********telo funkcie main**************/
        strncpy(parser->fName, "main", sizeof(parser->fName));

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (searchItem(parser->table, parser->fName) != NULL) {
            return ESEM;
        }
        else {
            hTabItem *tableItem = createNewItem();
            tableItem->name = malloc(sizeof(char) * (strlen(parser->fName) + 1));
            strcpy(tableItem->name, parser->fName);
            tableItem->dataType = INT_KW;
            tableItem->isDefined = true;
            tableItem->f_label = 0;
            insertHashTable(parser->table, tableItem);
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))  {
            return ESYN;
        }
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {   // )
            return ESEM;
        }
        tItemPtr item;
        if ((item = calloc(1, sizeof(struct tItem))) == NULL){
            return ESYS;
        }
        kv_init(item->data);

        strncpy(item->functionId, parser->fName, sizeof(item->functionId));
        insertLast(item, &parser->paramList);

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))// {
            return ESYN;

        instruction_t lbl;
        create_LABEL_instr(&lbl, 0);
        *kl_pushp(instruction_list, parser->code) = lbl;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        result = parse_fn_body(parser);

        if (result != EOK)
            return result;

        if ((result = parser_next_token(parser)) != EOK && result != EEOF) {
            debug_print("%s\n", "<");
            return result;
        }

        if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))    ///mala by nasledovat dalsia funkcia
            result = parse_fn(parser);        //rekurzivne volanie pre spracovanie dalsej funkcie
        else if (TOKEN_IS(parser->token, EOF_TYPE))
            return EEOF;
        else
            return ESYN;

        return result;                /// po main uz ziadne dalsie rekurzivne volanie ---- main je posledna funkcia v programe
    }
    else if (TOKEN_IS(parser->token, ID_TYPE)) {
        /*****************telo funkcie*************/
        strncpy(parser->fName, ZVAL_GET_STRING(&parser->token->data), sizeof(parser->fName));
        parser->fDeclared = false;
        /********MISSSING: kontrola ts**********************/

        hTabItem *tableItem = searchItem(parser->table, parser->fName);
        if (tableItem != NULL) {
            parser->fDeclared = true;
        }
        else {
            tableItem = createNewItem();
            tableItem->name = malloc(sizeof(char) * (strlen(parser->fName) + 1));
            strcpy(tableItem->name, parser->fName);
            tableItem->dataType = fType;
            tableItem->isDefined = false;
            tableItem->f_label = ++parser->label;
            insertHashTable(parser->table, tableItem);
        }
        /***ak uz je v tabulke nastavy sa fDeclared na true a pokracuje dalej*/
        /**ak nie je v TS tak sa do nej vlozi nazov funkcie a jej typ*/

        tItemPtr item;
        if (parser->fDeclared) {
            item = parser->paramList.First;
            while (item != NULL) {
                if (!strcmp(item->functionId, parser->fName))
                    break;
                item = item->next;
            }
            if (item == NULL)
                return ESYS;
        }
        else {
            if ((item = calloc(1, sizeof(struct tItem))) == NULL)
                return ESYS;
            kv_init(item->data);
            strncpy(item->functionId, parser->fName, sizeof(item->functionId));
        }

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
            result = parse_fn_args(parser, item);    //parsovanie argumentov funkcie

        if (result != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if ((tableItem = searchItem(parser->table, parser->fName)) == NULL)
            return ESYS;

        if (parser->fDeclared) {
            if (tableItem->params != parser->argsCounter)
                return ESEM;
        } else {
            tableItem->params = parser->argsCounter;
            int j = 0;

            for (int i = parser->offset_counter; i > 0; i--) {
                kv_A(item->data, j).offset = -(i);
                debug_print("PARAMETER:%s...offset:%d\n", kv_A(item->data, j).id, kv_A(item->data, j).offset);
                j++;
            }

            parser->offset_counter = 0;
            insertLast(item, &parser->paramList);
        }

        parser->argsCounter = 0;
        //uz sa nekontroluje parser->token ")" pretoze funkcia parse_args sa vrati vzdy uspesne ukoncena tymto parser->tokenom
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL) && tableItem->isDefined == false) {
            tableItem->isDefined = true;

            debug_print("LABEL F %d\n", tableItem->f_label);
            instruction_t lbl;
            create_LABEL_instr(&lbl, tableItem->f_label);
            *kl_pushp(instruction_list, parser->code) = lbl;
            parser->label++;

            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                            return result;
            }
                   //ak nie je funkcia prazdna
            result = parse_fn_body(parser);

            if (result != EOK)
                return result;

            /********MISSING: vlozenie 3AK -- navrat z funkcie*******/
            debug_print("LABEL ENDF %d\n", tableItem->f_label);
        }
        else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL) && tableItem->isDefined == true)
            return ESEM;
        else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            result = EOK;
        else
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))    ///mala by nasledovat dalsia funkcia
            result = parse_fn(parser);        //rekurzivne volanie pre spracovanie dalsej funkcie
        else if (TOKEN_IS(parser->token, EOF_TYPE))
            return EEOF;
        else
            return ESYN;    //ak nie je ziadna dalsia funkcia je to chyba
    } else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, MAIN_KW) && fType != INT_KW)
        return ESEM;
    else if(TOKEN_IS(parser->token,FN_TYPE))
        return ESEM;
    else return ESYN;

    return result;
}


result_t parse_fn_body(parser_t *parser) {
/******telo funkcie********/
    result_t result = EOK;

    tItemPtr varBlock;
    if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
        return ESYS;
    kv_init(varBlock->data);

    insertLast(varBlock, &parser->varList);
    parser->argsCounter = 0;

    result = parse_list(parser);
    if (result != EOK && result != EEOF)
        return result;

    if (!parser->has_return){
        debug_print("%s\n","<");
        return ERUN1;
    }

    parser->has_return = false;

    if (parser->varList.First != NULL)
        return ESYN;

    return result;
}

/**********************prikazy v jednom bloku************************************/
result_t parse_list(parser_t *parser) {
    result_t result = EOK;

    /*******************ID <priradenie> ; <parse_list>************************/
    if (TOKEN_IS(parser->token, ID_TYPE)) {
        char *hName = NULL;
        int var_offset=0;

        if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
            if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                return ESEM;
            else {
                var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal);
            }
        } else {
            var_offset = get_var_offset(&parser->varList, parser->token->data.sVal);
        }

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        strncpy(parser->assignVarName, hName, sizeof(parser->assignVarName));
        result = parse_assign(parser);

        if (result != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        debug_print("\tMV TO OFFSET:%d VYRAZ\n", var_offset);

        instruction_t store;
        create_STORE_instr(&store, var_offset);
        *kl_pushp(instruction_list, parser->code) = store;

        /*vlozenie 3AK na priradenie vysledku z funkcie parse_assign do hName*/
    }

        /***************** { <deklaracia> <parse_list> ****************************/
    else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL)) {

        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        parser->argsCounter = 0;

        if (result != EOK){
            debug_print("%s\n", "<");
            return result;
        }

        insertLast(varBlock, &parser->varList);
        debug_print("%s\n",".....NEW BLOCK.....\n");
        result = parse_list(parser);

        if (result != EOK){
            debug_print("%s\n", "<");
            return result;
        }
    }

        /*************** } <parse_list> ***********************/
    else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_VINCULUM_SMBL)) {
        debug_print("%s\n","....END BLOCK....\n");

        tItemPtr item = parser->varList.Last;
        // Move this on call stack
        if (kv_size(item->data) > 0) {
//            instruction_t *pop = malloc(sizeof(instruction_t));
//            create_POP_N_instr(pop, (int) kv_size(item->data));
//            *kl_pushp(instruction_list, parser->code) = pop;
//
//            debug_print("BLOCK POPN: %lu\n", kv_size(item->data));

            parser->offset_counter -= kv_size(item->data);
        }

        deleteLast(&parser->varList);
        return EOK;
    }
        /*********** if ( <EXP> ) { <list> } else { <parse_list> }*******************/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, IF_KW)) {

        int elseLabel = parser->label;
        int endLabel = ++parser->label;
        parser->label++;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;

        /*********hack vyrazu ******/
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        int lbracket = 1, rbracket = 0;
        klist_t(token_list) *tokens = kl_init(token_list);
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL)) {

            if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL)) {
                lbracket++;
            } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
                rbracket++;
            }

            if (lbracket == rbracket) { // end of expression
                break;
            }

            token_t cpy;
            token_init(&cpy);

            if (TOKEN_IS(parser->token, ID_TYPE)) {
                int offset;

                if ((result = offset_of_current_token(parser, &offset)) != EOK) {
                    debug_print("%s\n", "<");
                    return result;
                }

                cpy.type = ID_TYPE;
                cpy.flags = OFFSET_ID;
                zval_set(&cpy.data, offset);
            } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SMBL_WHITELIST) || TOKEN_IS(parser->token, CONST_TYPE)) {
                token_copy(&cpy, parser->token);
            } else {
                debug_print("%s [%d]\n", "< UNKNOWN TOKEN FOR EXPRESSION", parser->token->type);
                return ELEX;
            }

            *kl_pushp(token_list, tokens) = cpy;
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (TOKEN_IS(parser->token, ID_TYPE) && (searchItem(parser->table, parser->token->data.sVal) != NULL)) {
                debug_print("%s\n", "Undefined variable.");
                return ELEX;
            }
        }

        if (lbracket != rbracket) {
            debug_print("%s\n", "Unknown bracket count in if statement");
            return ESYN;
        }

        klist_t(expr_stack) *expr = kl_init(expr_stack);
        if ((result = expr_from_tokens(expr, tokens)) != EOK) {
            kl_destroy(token_list, tokens);
            kl_destroy(expr_stack, expr);
            return result;
        }
        kl_destroy(token_list, tokens);

        if ((result = append_instr_from_expr(parser->code, expr)) != EOK) {
            kl_destroy(expr_stack, expr);
            return result;
        }
        kl_destroy(expr_stack, expr);
        /********hack vyrazu********/

        debug_print("%s\n","EXP\n");
        debug_print("JMPF %d\n", elseLabel);

        // jmp nq
        instruction_t jmp;
        zval_t zero;
        zval_set_int(&zero, 0);
        create_JMPE_instr(&jmp, elseLabel, &zero);
        *kl_pushp(instruction_list, parser->code) = jmp;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;

        debug_print("%s\n","....NEW BLOCK....\n");
        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        //result = parse_fn_declaration(parser, varBlock);
        parser->argsCounter = 0;
        if (result != EOK)
            return result;

        insertLast(varBlock, &parser->varList);
        result = parse_list(parser);

        if (result != EOK)
            return result;

        debug_print("JMP %d\n", endLabel);
        instruction_t jmpend;
        create_JMP_instr(&jmpend, endLabel);
        *kl_pushp(instruction_list, parser->code) = jmpend;
        /*vlozenie 3AK - skok na endLabel*/

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, ELSE_KW))
            return ESYN;

        /******vlozenie 3AK - elseLabel*******/
        debug_print("LABEL ELSE %d\n", elseLabel);
        instruction_t lblelse;
        create_LABEL_instr(&lblelse, elseLabel);
        *kl_pushp(instruction_list, parser->code) = lblelse;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;
        debug_print("%s\n","....NEW BLOCK....\n");
        tItemPtr varBlock2;
        if ((varBlock2 = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock2->data);

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        //result = parse_fn_declaration(parser, varBlock2);
        parser->argsCounter = 0;
        if (result != EOK){
            debug_print("%s\n", "<");
            return result;
        }
        insertLast(varBlock2, &parser->varList);
        result = parse_list(parser);

        if (result != EOK)
            return result;

        /**vlozenie 3AK - endLabel***/
        debug_print("LABEL END %d\n", endLabel);
        instruction_t lblend;
        create_LABEL_instr(&lblend, endLabel);
        *kl_pushp(instruction_list, parser->code) = lblend;
    }

        /** for ( <deklaracia> ; <vyraz> ; priradenie) <parse_list>**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, FOR_KW)) {
        int expLabel = parser->label;
        int assignLabel = ++parser->label;
        int beginLabel = ++parser->label;
        int endLabel = ++parser->label;
        parser->label++;
        char *hName = NULL;
        hTabItem *tItem;
        int var_offset=0;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;

        /**deklaracia riadiacej premennej**/
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW | AUTO_KW))
            return ESYN;

        int varType = parser->token->flags;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_IS(parser->token, ID_TYPE))
            return ESYN;

        if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) != NULL)
            return ESEM;

        if ((tItem = searchItem(parser->table, parser->token->data.sVal)) != NULL)
            return ESEM;

        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        hName = generate_var_name(parser->hInt++);
        parser->offset_counter++;

        tData data;
        if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token->data), hName,parser->offset_counter)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        item_append_data(varBlock, data);
        debug_print("Variable Push %s...offset:%d\n",parser->token->data.sVal,data.offset);

        insertLast(varBlock, &parser->varList);

        /*vlozenie riadiacje premennej do TS*/
        tItem = createNewItem();
        tItem->name = calloc(1, strlen(hName) + 1);
        strcpy(tItem->name, hName);
        tItem->dataType = varType;
        insertHashTable(parser->table, tItem);

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

            /*********hack**********/
            klist_t(token_list) *tokens = kl_init(token_list);
            while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
                token_t cpy;
                token_init(&cpy);

                if (TOKEN_IS(parser->token, ID_TYPE)) {
                    int offset;

                    if ((result = offset_of_current_token(parser, &offset)) != EOK) {
                        kl_destroy(token_list, tokens);
                        debug_print("%s\n", "<");
                        return result;
                    }

                    cpy.type = ID_TYPE;
                    cpy.flags = OFFSET_ID;
                    zval_set(&cpy.data, offset);
                } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SMBL_WHITELIST) || TOKEN_IS(parser->token, CONST_TYPE)) {
                    token_copy(&cpy, parser->token);
                } else {
                    kl_destroy(token_list, tokens);
                    debug_print("%s [%d]\n", "< UNKNOWN TOKEN FOR EXPRESSION", parser->token->type);
                    return ELEX;
                }

                *kl_pushp(token_list, tokens) = cpy;
                if ((result = parser_next_token(parser)) != EOK) {
                    debug_print("%s\n", "<");
                    return result;
                }

                if (TOKEN_IS(parser->token, ID_TYPE) && (searchItem(parser->table, parser->token->data.sVal) != NULL)) {
                    debug_print("%s\n", "Undefined variable");
                    return ELEX;
                }
            }

            klist_t(expr_stack) *expr = kl_init(expr_stack);
            if ((result = expr_from_tokens(expr, tokens)) != EOK) {
                kl_destroy(token_list, tokens);
                kl_destroy(expr_stack, expr);
                if(result == ESEM2)
                    return ESEM4;
                return result;
            }
            kl_destroy(token_list, tokens);

            if ((result = append_instr_from_expr(parser->code, expr)) != EOK) {
                kl_destroy(expr_stack, expr);
                return result;
            }
            kl_destroy(expr_stack, expr);
            /*********************/

            /**vlozenie hodnoty riadiacej premennej podla vysledku parse_assign()**/
            /*******3AK, MV , #1, NULL, hName******/

            instruction_t store;
            create_STORE_instr(&store, data.offset);
            *kl_pushp(instruction_list, parser->code) = store;

        } else if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            return ESYN;
        }

        /**vlozenie 3AK  - label expLabel**/
        debug_print("EXP LABEL %d\n", expLabel);
        debug_print("%s\n","EXP\n");
        instruction_t explbl;
        create_LABEL_instr(&explbl, expLabel);
        *kl_pushp(instruction_list, parser->code) = explbl;

        /*********hack**********/
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        klist_t(token_list) *tokens = kl_init(token_list);
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            token_t cpy;
            token_init(&cpy);

            if (TOKEN_IS(parser->token, ID_TYPE)) {
                int offset;

                if ((result = offset_of_current_token(parser, &offset)) != EOK) {
                    kl_destroy(token_list, tokens);
                    debug_print("%s\n", "<");
                    return result;
                }

                cpy.type = ID_TYPE;
                cpy.flags = OFFSET_ID;
                zval_set(&cpy.data, offset);
            } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SMBL_WHITELIST) || TOKEN_IS(parser->token, CONST_TYPE)) {
                token_copy(&cpy, parser->token);
            } else {
                kl_destroy(token_list, tokens);
                debug_print("%s [%d]\n", "< UNKNOWN TOKEN FOR EXPRESSION", parser->token->type);
                return ELEX;
            }

            *kl_pushp(token_list, tokens) = cpy;
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (TOKEN_IS(parser->token, ID_TYPE) && (searchItem(parser->table, parser->token->data.sVal) != NULL)) {
                debug_print("%s\n", "Undefined variable");
                return ELEX;
            }
        }

        klist_t(expr_stack) *expr = kl_init(expr_stack);
        if ((result = expr_from_tokens(expr, tokens)) != EOK) {
            kl_destroy(token_list, tokens);
            kl_destroy(expr_stack, expr);
            if(result == ESEM2)
                return ESEM4;
            return result;
        }
        kl_destroy(token_list, tokens);

        if ((result = append_instr_from_expr(parser->code, expr)) != EOK) {
            kl_destroy(expr_stack, expr);
            return result;
        }
        kl_destroy(expr_stack, expr);
        /*********************/

        instruction_t jmp;
        zval_t res;
        zval_set(&res, 0);
        create_JMPE_instr(&jmp, endLabel, &res);
        *kl_pushp(instruction_list, parser->code) = jmp;

        create_JMP_instr(&jmp, beginLabel);
        *kl_pushp(instruction_list, parser->code) = jmp;

        /**vyhodnotenie vyrazu**/
        debug_print("JMPT BEGIN %d\n", beginLabel);
        debug_print("JMPF END %d\n", endLabel);
        /**vlozenie 3AK - skok na beginLabel ak je vyraz true**/
        /**vlozenie 3AK - skok na endLabel ak je vyraz false**/

        debug_print("LABEL ASSIGNLABEL %d\n", assignLabel);
        /**vlozenie 3AK - label assignLabel**/
        instruction_t asgnlbl;
        create_LABEL_instr(&asgnlbl, assignLabel);
        *kl_pushp(instruction_list, parser->code) = asgnlbl;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        if (!TOKEN_IS(parser->token, ID_TYPE))
            return ESYN;
        if (varSearch(&parser->varList, parser->token->data.sVal) == NULL) {
            if (paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal) == NULL)
                return ESEM;
            else {
                var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal);
            }
        } else {
            var_offset = get_var_offset(&parser->varList, parser->token->data.sVal);
        }

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;

        /*********hack**********/
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        int lbracket = 1, rbracket = 0;
        tokens = kl_init(token_list);
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL)) {

            if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL)) {
                lbracket++;
            } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
                rbracket++;
            }

            if (lbracket == rbracket) { // end of expression
                break;
            }

            token_t cpy;
            token_init(&cpy);

            if (TOKEN_IS(parser->token, ID_TYPE)) {
                int offset;

                if ((result = offset_of_current_token(parser, &offset)) != EOK) {
                    debug_print("%s\n", "<");
                    return result;
                }

                cpy.type = ID_TYPE;
                cpy.flags = OFFSET_ID;
                zval_set(&cpy.data, offset);
            } else if (TOKEN_IS(parser->token, SMBL_TYPE) || TOKEN_IS(parser->token, CONST_TYPE)) {
                token_copy(&cpy, parser->token);
            } else {
                debug_print("%s [%d]\n", "< UNKNOWN TOKEN FOR EXPRESSION", parser->token->type);
                return ELEX;
            }

            *kl_pushp(token_list, tokens) = cpy;
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (TOKEN_IS(parser->token, ID_TYPE) && (searchItem(parser->table, parser->token->data.sVal) != NULL)) {
                debug_print("%s\n", "Undefined variable.");
                return ELEX;
            }
        }

        if (lbracket != rbracket) {
            debug_print("%s\n", "Unknown bracket count in for statement");
            return ESYN;
        }

        expr = kl_init(expr_stack);
        if ((result = expr_from_tokens(expr, tokens)) != EOK) {
            kl_destroy(token_list, tokens);
            kl_destroy(expr_stack, expr);
            return result;
        }
        kl_destroy(token_list, tokens);

        if ((result = append_instr_from_expr(parser->code, expr)) != EOK) {
            kl_destroy(expr_stack, expr);
            return result;
        }
        kl_destroy(expr_stack, expr);
        /*********************/

        debug_print("%s\n","ASSING EXP\n");

        instruction_t store;
        create_STORE_instr(&store, var_offset);
        *kl_pushp(instruction_list, parser->code) = store;

        /**vyhodnotenie vyrazu**/
        debug_print("MV TO OFFSET %d EXPRESULT\n", var_offset);
        /**vlozenie 3AK priradnie do premennej hNameID**/
        debug_print("JMP EXPLABEL %d\n", expLabel);
        /**vlozenie 3AK - skok na label expLabel**/

        create_JMP_instr(&jmp, expLabel);
        *kl_pushp(instruction_list, parser->code) = jmp;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;

        tItemPtr varBlock2;
        if ((varBlock2 = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock2->data);

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        parser->argsCounter = 0;
        if (result != EOK){
            debug_print("%s\n", "<");
            return result;
        }
        insertLast(varBlock2, &parser->varList);

        /**vlozenie 3AK - label beginLabel**/
        debug_print("LABEL BEGIN %d \n", beginLabel);
        instruction_t blbl;
        create_LABEL_instr(&blbl, beginLabel);
        *kl_pushp(instruction_list, parser->code) = blbl;

        result = parse_list(parser);

        if (result != EOK){
            debug_print("%s\n", "<");
            return result;
        }
        parser->offset_counter--;
        deleteLast(&parser->varList);
        debug_print("JMP ASSIGNLABEL %d\n", assignLabel);

        create_JMP_instr(&jmp, assignLabel);
        *kl_pushp(instruction_list, parser->code) = jmp;

        /**vlozenie 3AK - skok na assignLabel**/
        debug_print("LABEL END %d\n", endLabel);
        /**vlozenie 3AK - label endLabel**/
        instruction_t endlbl;
        create_LABEL_instr(&endlbl, endLabel);
        *kl_pushp(instruction_list, parser->code) = endlbl;
    }

        /**cin |>> ID|*n ;**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, CIN_KW)) {
        int var_offset=0;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_RIGHT_SMBL))
            return ESYN;

        while (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_RIGHT_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }
            if (!TOKEN_IS(parser->token, ID_TYPE))
                return ESYN;
            if ((var_offset = get_var_offset(&parser->varList, parser->token->data.sVal)) == 0) {
                if ((var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal)) == 0)
                    debug_print("%s\n", "<");
                    return ESEM;
            }
            /**vygenerovanie 3AK - nacitanie zo SV do premennej hName**/
            debug_print("\tCIN STDIN %d\n", var_offset);

            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            return ESYN;
    }

        /**cout |<< term |*n  ---- term==ID || TEXT || INT || DOUBLE**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, COUT_KW)) {
        int var_offset = 0;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_LEFT_SMBL))
            return ESYN;

        while (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_LEFT_SMBL)) {

            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (TOKEN_IS(parser->token, ID_TYPE)) {
                if ((var_offset = get_var_offset(&parser->varList, parser->token->data.sVal)) == 0) {
                    if ((var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal)) == 0)
                        return ESEM;
                }

                instruction_t cout;
                create_COUT_offset_instr(&cout, var_offset);
                *kl_pushp(instruction_list, parser->code) = cout;
                /**vlozenie 3AK - vypis na STDOUT z premennej hName**/
                debug_print("\tCOUNT STDOUT OFFSET:%d\n", var_offset);
            }
            else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, INT_CONST|DOUBLE_CONST|TEXT_CONST)) {
                instruction_t cout;
                create_COUT_zval_instr(&cout, &parser->token->data);
                *kl_pushp(instruction_list, parser->code) = cout;

                debug_print("\tCOUNT STDOUT CONST:%s\n", "");
            }
            else return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            return ESYN;
    }

        /**return <EXP> ;**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, RETURN_KW)) {

        /*********hack**********/
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        klist_t(token_list) *tokens = kl_init(token_list);
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            token_t cpy;
            token_init(&cpy);

            if (TOKEN_IS(parser->token, ID_TYPE)) {
                int offset;

                if ((result = offset_of_current_token(parser, &offset)) != EOK) {
                    kl_destroy(token_list, tokens);
                    debug_print("%s\n", "<");
                    return result;
                }

                cpy.type = ID_TYPE;
                cpy.flags = OFFSET_ID;
                zval_set(&cpy.data, offset);
            } else if (TOKEN_IS(parser->token, SMBL_TYPE) || TOKEN_IS(parser->token, CONST_TYPE)) {
                token_copy(&cpy, parser->token);
            } else {
                kl_destroy(token_list, tokens);
                debug_print("%s [%d]\n", "< UNKNOWN TOKEN FOR EXPRESSION", parser->token->type);
                return ELEX;
            }

            *kl_pushp(token_list, tokens) = cpy;
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (TOKEN_IS(parser->token, ID_TYPE) && (searchItem(parser->table, parser->token->data.sVal) != NULL)) {
                debug_print("%s\n", "Undefined variable");
                return ELEX;
            }
        }

        klist_t(expr_stack) *expr = kl_init(expr_stack);
        if ((result = expr_from_tokens(expr, tokens)) != EOK) {
            kl_destroy(token_list, tokens);
            kl_destroy(expr_stack, expr);
            if(result == ESEM2)
                return ESEM4;
            return result;
        }
        kl_destroy(token_list, tokens);

        if ((result = append_instr_from_expr(parser->code, expr)) != EOK) {
            kl_destroy(expr_stack, expr);
            return result;
        }
        kl_destroy(expr_stack, expr);
        /*********************/

        parser->has_return = true;

        debug_print("%s\n","RETURN\n");

        instruction_t ret;
        create_RETURN_instr(&ret);
        *kl_pushp(instruction_list, parser->code) = ret;
    }
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW|AUTO_KW)) {
        /***deklaracia mimo zaciatku bloku**/
        result = parse_adv_declaration(parser);
        if (result != EOK)
            return result;
    }
    else return ESYN;

    if ((result = parser_next_token(parser)) != EOK) {
        debug_print("%s\n", "<");
        return result;
    }

    return parse_list(parser);
}



result_t parse_adv_declaration(parser_t *parser) {
    result_t result;
    int varType = parser->token->flags;
    char *hName;
    hTabItem *tItem;

    if ((result = parser_next_token(parser)) != EOK) {
        debug_print("%s\n", "<");
        return result;
    }

    if (!TOKEN_IS(parser->token, ID_TYPE))
        return ESYN;

    if ((result = list_foo_bar(parser->varList.Last, ZVAL_GET_STRING(&parser->token->data))) != EOK) {
        debug_print("%s\n", "<");
        return result;
    }

    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) != NULL)
        return ESEM;

    if ((tItem = searchItem(parser->table, parser->token->data.sVal)) != NULL)
        return ESEM;

    hName = generate_var_name(parser->hInt++);
    parser->offset_counter++;

    tData data;
    if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token->data), hName,parser->offset_counter)) != EOK) {
        debug_print("%s\n", "<");
        return result;
    }
    debug_print("Variable Push %s...offset:%d\n", parser->token->data.sVal, data.offset);

    // add loc variable to stack
    zval_t val;

    if (varType == INT_KW) {
        zval_set(&val, 0);
        zval_set_undefined(&val);
    } else if (varType == DOUBLE_KW) {
        zval_set(&val, 0.0);
        zval_set_undefined(&val);
    } else if (varType == STRING_KW) {
        zval_set(&val, "undefined.");
        zval_set_undefined(&val);
    } else {
        zval_init(&val); // auto
    }

    instruction_t store;
    create_STORE_zval_instr(&store, &val);
    zval_dispose(&val);
    *kl_pushp(instruction_list, parser->code) = store;
    // End add variable to stack

    item_append_data(parser->varList.Last, data);

    /***vytvorenie novej polozky do TS****/
    tItem = createNewItem();
    tItem->name = calloc(1, strlen(hName) + 1);
    strcpy(tItem->name, hName);
    tItem->dataType = varType;
    insertHashTable(parser->table, tItem);

    if (varType == AUTO_KW) {
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESEM3;

        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        strncpy(parser->assignVarName, hName, sizeof(parser->assignVarName));
        result = parse_assign(parser);

        if (result != EOK){
            debug_print("%s\n", "<");
            return result;
        }/*******3AK , MV , #1 ,NULL, hName******/

    } else {
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }

        if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            return EOK;
        } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }
            strncpy(parser->assignVarName, hName, sizeof(parser->assignVarName));
            result = parse_assign(parser);

            if (result != EOK){
                debug_print("%d %s\n",result, "<");
                return result;
            }

            instruction_t mv;
            create_STORE_instr(&mv, parser->offset_counter);
            *kl_pushp(instruction_list, parser->code) = mv;
            debug_print("MV to offset %d\n", parser->offset_counter);

            return EOK;
        } else return ESYN;
    }
    return result;
}






result_t parse_assign(parser_t *parser) {
    result_t result;
    debug_print("ass %s\n", "<");
    if (TOKEN_IS(parser->token, ID_TYPE) || TOKEN_IS(parser->token, FN_TYPE)) {

        hTabItem *tableItem;
        if ((tableItem = searchItem(parser->table, parser->token->data.sVal)) == NULL) {

            klist_t(token_list) *tokens = kl_init(token_list);
            while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
                token_t cpy;
                token_init(&cpy);

                if (TOKEN_IS(parser->token, ID_TYPE)) {
                    int offset;

                    if ((result = offset_of_current_token(parser, &offset)) != EOK) {
                        debug_print("%s\n", "<");
                        return result;
                    }

                    cpy.type = ID_TYPE;
                    cpy.flags = OFFSET_ID;
                    zval_set(&cpy.data, offset);
                } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SMBL_WHITELIST) || TOKEN_IS(parser->token, CONST_TYPE)) {
                    token_copy(&cpy, parser->token);
                } else {
                    debug_print("%s [%d]\n", "< UNKNOWN TOKEN FOR EXPRESSION", parser->token->type);
                    return ESYN;
                }

                *kl_pushp(token_list, tokens) = cpy;
                if ((result = parser_next_token(parser)) != EOK) {
                    debug_print("%s\n", "<");
                    return result;
                }

                if (TOKEN_IS(parser->token, ID_TYPE) && ((tableItem = searchItem(parser->table, parser->token->data.sVal)) != NULL)) {
                    debug_print("%s\n", "Undefined variable");
                    return ESEM;
                }
            }

            klist_t(expr_stack) *expr = kl_init(expr_stack);
            if ((result = expr_from_tokens(expr, tokens)) != EOK) {
                kl_destroy(expr_stack, expr);
                kl_destroy(token_list, tokens);
                debug_print("%d %s\n",result, "<");
                return result;
            }
            kl_destroy(token_list, tokens);
            debug_print("%d %s\n",result, "<");
            if ((result = append_instr_from_expr(parser->code, expr)) != EOK) {
                kl_destroy(expr_stack, expr);
                debug_print("%d %s\n",result, "<");
                return result;
            }
            kl_destroy(expr_stack, expr);
            debug_print("%d %s\n",result, "<");
        } else {

            bool is_native = TOKEN_IS(parser->token, FN_TYPE);

            /**volanie uzivatelskej funkcie**/
            tItemPtr item;
            item = parser->paramList.First;
            while (item != NULL) {
                if (!strcmp(item->functionId, parser->token->data.sVal))
                    break;
                item = item->next;
            }
            if (item == NULL)
                return ESYS;

            hTabItem *tItem;

            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != tableItem->dataType && tItem->dataType != AUTO_KW) {
                if (tItem->dataType == STRING_KW || tableItem->dataType == STRING_KW)
                    return ESEM2;
                /*else {
                    if (tItem->dataType == DOUBLE_KW && tableItem->dataType == INT_KW) {
                        tItem->dataType = INT_KW;
                        tItem->iVal = (int)tItem->dVal;
                    } else if (tItem->dataType == INT_KW && tableItem->dataType == DOUBLE_KW) {
                        tItem->dataType = DOUBLE_KW;
                        tItem->dVal = (double)tItem->iVal;
                    }
                }*/
            }
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                result = parse_params(parser, item);

            if (result != EOK){
                debug_print("%s\n", "<");
                return result;
            }

            if (kv_size(item->data) != parser->argsCounter1)
                return ESEM2;

            hTabItem *fnItem = searchItem(parser->table, item->functionId);

            if (fnItem == NULL)
                return ELEX;

            debug_print("f call: %d\n", fnItem->f_label);

            instruction_t call;
            if (is_native) {
                create_CALLN_instr(&call, item->functionId);
            } else {
                create_CALL_instr(&call, fnItem->f_label, (int) kv_size(item->data));
            }
            *kl_pushp(instruction_list, parser->code) = call;


            parser->argsCounter1 = 0;

            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            /***********3AK , JMP, LABEL, PARAMETRE, NULL***********/
        }
    } else if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {

        klist_t(token_list) *tokens = kl_init(token_list);
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            token_t cpy;
            token_init(&cpy);

            if (TOKEN_IS(parser->token, ID_TYPE)) {
                int offset;

                if ((result = offset_of_current_token(parser, &offset)) != EOK) {
                    debug_print("%s\n", "<");
                    return result;
                }

                cpy.type = ID_TYPE;
                cpy.flags = OFFSET_ID;
                zval_set(&cpy.data, offset);
            } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SMBL_WHITELIST) || TOKEN_IS(parser->token, CONST_TYPE)) {
                token_copy(&cpy, parser->token);
            } else {
                debug_print("%s [%d]\n", "< UNKNOWN TOKEN FOR EXPRESSION", parser->token->type);
                return ESYN;
            }

            *kl_pushp(token_list, tokens) = cpy;
            if ((result = parser_next_token(parser)) != EOK) {
                debug_print("%s\n", "<");
                return result;
            }

            if (TOKEN_IS(parser->token, ID_TYPE) && (searchItem(parser->table, parser->token->data.sVal) != NULL)) {
                debug_print("%s\n", "Undefined variable");
                return ESEM;
            }
        }

        klist_t(expr_stack) *expr = kl_init(expr_stack);
        if ((result = expr_from_tokens(expr, tokens)) != EOK) {
            kl_destroy(token_list, tokens);
            kl_destroy(expr_stack, expr);
            debug_print("%d %s\n",result, "<");
            return result;
        }
        kl_destroy(token_list, tokens);

        if ((result = append_instr_from_expr(parser->code, expr)) != EOK) {
            kl_destroy(expr_stack, expr);
            debug_print("%d %s\n",result, "<");
            return result;
        }
        kl_destroy(expr_stack, expr);
        debug_print("ok%d %s\n",result, "<");
        /*********************/
    } else return ESYN;
    return EOK;
}



result_t parse_params(parser_t *parser, tItemPtr item) {
    result_t result = EOK;
    char *hName;
    hTabItem *tableItem;
    int var_offset = 0;

    if (TOKEN_IS(parser->token, ID_TYPE)) {
        hTabItem *tItem1;
        debug_print("token ID %d\n", parser->token->data.iVal);
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESEM2;
        if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
            if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                return ESEM;
            else { var_offset= get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal);}
        }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
        if ((tItem1 = searchItem(parser->table, hName)) == NULL)
            return ESYS;

        if ((tableItem->dataType != AUTO_KW) && (tableItem->dataType != tItem1->dataType) && (tItem1->dataType != AUTO_KW))
        {
            //pretypovanie
            if(tableItem->dataType == INT_KW && tItem1->dataType == DOUBLE_KW){
                parser->token->data.iVal = (int)parser->token->data.dVal;
                parser->token->flags = INT_KW;
            }
            else if(tableItem->dataType == DOUBLE_KW && tItem1->dataType == INT_KW){
                parser->token->data.dVal = (double)parser->token->data.iVal;
                parser->token->flags = DOUBLE_KW;
            }
            else return ESEM2;
        }
        debug_print("PUSH PARAM ID: %s OFFSET: %d\n", parser->token->data.sVal, var_offset);

        zval_t a;
        zval_set(&a, var_offset);
        instruction_t i;
        create_LOAD_instr(&i, &a);
        *kl_pushp(instruction_list, parser->code) = i;

        /****************parameter tItem1*****************/
    }
    else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, INT_CONST)) {
        debug_print("token INT %d\n",parser->token->data.iVal);
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESEM2;

        if (tableItem->dataType != INT_KW && tableItem->dataType != AUTO_KW){
            if(tableItem->dataType == DOUBLE_KW){
                parser->token->data.dVal = (double)parser->token->data.iVal;
                parser->token->flags = DOUBLE_KW;
            }
            else{
                return ESEM2;
            }
        }

        instruction_t i;
        create_PUSH_zval_instr(&i, &parser->token->data);
        *kl_pushp(instruction_list, parser->code) = i;
        debug_print("PUSH PARAM %d\n",parser->token->data.iVal);
        /**************************************/
    }
    else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, DOUBLE_CONST)) {
        debug_print("token DBL %d\n",parser->token->data.iVal);
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESEM2;

        if (tableItem->dataType != DOUBLE_KW && tableItem->dataType != AUTO_KW){
            if(tableItem->dataType == INT_KW){
                parser->token->data.iVal = (int)parser->token->data.dVal;
                parser->token->flags = INT_KW;
            }
            else{
                return ESEM2;
            }
        }

        instruction_t i;
        create_PUSH_zval_instr(&i, &parser->token->data);
        *kl_pushp(instruction_list, parser->code) = i;
        debug_print("PUSH PARAM %lf\n",parser->token->data.dVal);
        /****************************************/
    }
    else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL){
            return ESEM2;
        }
        if (tableItem->dataType != STRING_KW && tableItem->dataType != AUTO_KW){
            return ESEM2;
        }

        instruction_t i;
        create_PUSH_zval_instr(&i, &parser->token->data);
        *kl_pushp(instruction_list, parser->code) = i;
        debug_print("PUSH PARAM %s\n",parser->token->data.sVal);
        /**************************************/
    }
    else return ESYN;

    parser->argsCounter1++;

    if ((result = parser_next_token(parser)) != EOK) {
        debug_print(" %s\n",parser->token->data.sVal );
        return result;
    }
    if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL)) {
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("PUSH PARAM %s\n",parser->token->data.sVal );
            return result;
        }
        if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL)){
            debug_print("PUSH PARAM %s\n",parser->token->data.sVal );
            return ESEM2;
        }
        else
            result = parse_params(parser, item);
    }
    else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
        return EOK;
    }
    else return ESYN;

    return result;
}
result_t parse_fn_args(parser_t *parser, tItemPtr item) {
    /******spracovanie argumentov funkcii********/
    result_t result;

    int varType = parser->token->flags;

    if (varType == AUTO_KW)
        return ESEM4;

    hTabItem *tItem;
    if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))
        return ESYN;

    if ((result = parser_next_token(parser)) != EOK) {
        debug_print("%s\n", "<");
        return result;
    }

    if (!TOKEN_IS(parser->token, ID_TYPE))
        return ESYN;

    if (parser->fDeclared) {
        char *hName = NULL;
        int i = 0;
        while (i < kv_size(item->data)) {
            if (!strcmp(parser->token->data.sVal, kv_A(item->data, i).id)) {
                hName = kv_A(item->data, i).hid;
                break;
            }
            i++;
        }

        if (hName == NULL)
            return ESEM;

        if ((tItem = searchItem(parser->table, hName)) == NULL) {
            return ESYS;
        }
        /****kontrola typu a poradia parametru****/
        if (tItem->dataType != varType)
            return ESEM;
        if (tItem->paramPosition != parser->argsCounter)
            return ESEM;

    }
    else {
        char *hName = generate_var_name(parser->hInt++);

        int i = 0;
        while (i < kv_size(item->data)) {
            if (!strcmp(parser->token->data.sVal, kv_A(item->data, i).id))
                return ESEM;
            i++;
        }

        if((tItem = searchItem(parser->table, parser->token->data.sVal)) != NULL)
            return ESEM;

        parser->offset_counter++;
        tData data;
        if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token->data), hName,0)) != EOK)  {
            debug_print("%s\n", "<");
            return result;
        }
        item_append_data(item, data);

        /*vlozi parameter do TS -nazov, typ, funkciu a poradie*/
        tItem = createNewItem();
        tItem->name = calloc(1, strlen(hName) + 1);
        strcpy(tItem->name, hName);
        tItem->dataType = varType;
        tItem->paramPosition = parser->argsCounter;
        insertHashTable(parser->table, tItem);
    }

    parser->argsCounter++;

    /*ak je dalsi parser->token , rekurzivne sa zavola funckia parse_args a nacitava sa dalsi parameter*/
    /*ak je to zatvorka tak sa vracia EOK*/
    /*iny parser->token znamena chybu*/
    if ((result = parser_next_token(parser)) != EOK) {
        debug_print("%s\n", "<");
        return result;
    }

    if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
        return EOK;
    }
    else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL)) {
        if ((result = parser_next_token(parser)) != EOK) {
            debug_print("%s\n", "<");
            return result;
        }
        result = parse_fn_args(parser, item);
    }
    else return ESYN;
    return result;
}
