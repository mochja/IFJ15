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

int debug = 1;
#include "parser.h"

result_t init_parser(parser_t *parser, char *source) {
    listInit(&parser->varList);
    listInit(&parser->paramList);

    parser->fName = NULL;
    parser->argsCounter = 0;
    parser->argsCounter1 = 0;
    parser->fDeclared = false;
    parser->hInt = 0;
    parser->label = 1;
    parser->assignVarName = NULL;
    parser->offset_counter=0;

    parser->token = calloc(1, sizeof(token_t));
    parser->code = kl_init(instruction_list);

    // jump to main() on start
    *kl_pushp(instruction_list, parser->code) = create_JMP_instr(0);

    if ((parser->table = initHashTable(MAX_HTSIZE)) == NULL) {
        fprintf(stderr, "Could not initialize hash table.");
        return ESYS;
    }

    result_t res = init_scanner(&parser->scanner, source);

    if (res != EOK) {
        fprintf(stderr, "Could not initialize scanner.");
        return res;
    }

    return EOK;
}

result_t parser_next_token(parser_t *parser) {
    return scanner_get_next_token(&parser->scanner, parser->token);
}

char __nbuffer[64];
char *generate_var_name(int number) {
    sprintf(__nbuffer, "$%d", number);
    return __nbuffer;
}

result_t parser_run(parser_t *parser) {
/**overim prvy parser->token**/
    result_t result;

    if ((result = parser_next_token(parser)) != EOK) {
        return result;
    }

    if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))
        return ESYN;

    if ((result = parse_fn(parser)) != EOK) {
        fprintf(stderr, "Something bad happened.");
        return result;
    }

    // Overime ci sme naozaj na konci
    if (parser_next_token(parser) != EEOF) {
        return ESYN;
    }

    return result;
}


result_t parse_fn(parser_t *parser) {
    result_t result;

    int fType = parser->token->flags;

    if ((result = parser_next_token(parser)) != EOK) {
        if (debug) printf("r92: %d, get_next_token in parse_fn\n", result);
        return result;
    }

    if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, MAIN_KW) && fType == INT_KW) {
        /***********telo funkcie main**************/
        parser->fName = "main";

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r101: %d, parser_next_token in parse_fn\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))  {
            return ESYN;
        }
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r109: %d, parser_next_token in parse_fn\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {   // )
            return ESYN;
        }
        tItemPtr item;
        if ((item = calloc(1, sizeof(struct tItem))) == NULL){
            return ESYS;
        }
        kv_init(item->data);

        item->functionId = parser->fName;
        insertLast(item, &parser->paramList);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r126: %d, parser_next_token in parse_fn\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))// {
            return ESYN;

        *kl_pushp(instruction_list, parser->code) = create_LABEL_instr(0);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r135: %d, parser_next_token in parse_fn\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_VINCULUM_SMBL))        //ak nie je funkcia prazdna
            result = parse_fn_body(parser);

        return result;                /// po main uz ziadne dalsie rekurzivne volanie ---- main je posledna funkcia v programe
    }
    else if (TOKEN_IS(parser->token, ID_TYPE)) {
        /*****************telo funkcie*************/
        parser->fName = ZVAL_GET_STRING(&parser->token->data);
        char *fLabel = parser->token->data.sVal;
        parser->fDeclared = false;
        /********MISSSING: kontrola ts**********************/

        hTabItem *tableItem;
        if ((tableItem = searchItem(parser->table, parser->fName)) != NULL) {
            parser->fDeclared = true;
        }
        else {
            tableItem = createNewItem();
            tableItem->name = malloc(sizeof(char) * (strlen(parser->fName) + 1));
            strcpy(tableItem->name, parser->fName);
            tableItem->dataType = fType;
            tableItem->isDefined = false;
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
            item->functionId = parser->fName;
        }

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r187: %d, parser_next_token in parse_fn\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r195: %d, parser_next_token in parse_fn\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
            result = parse_fn_args(parser, item);    //parsovanie argumentov funkcie

        if (result != EOK){
            if (debug) printf("r203: %d, parse_fn_args in parse_fn\n", result);
            return result;
        }

        if ((tableItem = searchItem(parser->table, parser->fName)) == NULL)
            return ESYS;

        if (parser->fDeclared) {
            if (tableItem->params != parser->argsCounter)
                return ESEM;
        } else {
            tableItem->params = parser->argsCounter;
            int j=0;
            for(int i = parser->offset_counter; i > 0; i--){
                kv_A(item->data,j).offset=-(i);
                printf("PARAMETER:%s...offset:%d\n",kv_A(item->data, j).id,  kv_A(item->data,j).offset );
                j++;
            }
            parser->offset_counter=0;
            insertLast(item, &parser->paramList);
        }

        parser->argsCounter = 0;
        //uz sa nekontroluje parser->token ")" pretoze funkcia parse_args sa vrati vzdy uspesne ukoncena tymto parser->tokenom
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r228: %d, parser_next_token in parse_fn\n", result);
            return result;
        }

        if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL) && tableItem->isDefined == false) {
            tableItem->isDefined = true;
            /********MISSING: vlozenie 3AK -- label zaciatku funkcie*******/
            printf("LABEL F %s\n", fLabel);
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r237: %d, parser_next_token in parse_fn\n", result);
                            return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_VINCULUM_SMBL))        //ak nie je funkcia prazdna
                result = parse_fn_body(parser);

            if (result != EOK)
                return result;

            /********MISSING: vlozenie 3AK -- navrat z funkcie*******/
            printf("LABEL ENDF %s\n", fLabel);
        }
        else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL) && tableItem->isDefined == true)
            return ESEM;
        else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            result = EOK;
        else
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r257: %d, parser_next_token in parse_fn\n", result);
            return result;
        }
        if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW))    ///mala by nasledovat dalsia funkcia
            result = parse_fn(parser);        //rekurzivne volanie pre spracovanie dalsej funkcie
        else if (TOKEN_IS(parser->token, EOF_TYPE))
            return ESEM;
        else
            return ESYN;    //ak nie je ziadna dalsia funkcia je to chyba
    } else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, MAIN_KW) && fType != INT_KW)
        return ESEM;
    else result = ESYN;

    return result;
}


result_t parse_fn_body(parser_t *parser) {
/******telo funkcie********/
    result_t result = EOK;

    tItemPtr varBlock;
    if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
        return ESYS;
    kv_init(varBlock->data);

    result = parse_fn_declaration(parser, varBlock);

    if (result != EOK){
        if (debug) printf("r286: %d, parse_fn_declaration in parse_fn_body\n", result);
        return result;
    }

    insertLast(varBlock, &parser->varList);
    parser->argsCounter = 0;

    result = parse_list(parser);

    return result;
}

/**********************prikazy v jednom bloku************************************/
result_t parse_list(parser_t *parser) {
    result_t result = EOK;

    /*******************ID <priradenie> ; <parse_list>************************/
    if (TOKEN_IS(parser->token, ID_TYPE)) {
        char *hName = NULL;
        if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
            if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                return ESEM;
        }
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r310: %d, parser_next_token in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r317: %d, parser_next_token in parse_list\n", result);
            return result;
        }
        parser->assignVarName = hName;
        result = parse_assign(parser);

        if (result != EOK){
            if (debug) printf("r324: %d, parse_assign in parse_list\n", result);
            return result;
        }
        printf("\tMV %s VYRAZ\n", hName);
        /*vlozenie 3AK na priradenie vysledku z funkcie parse_assign do hName*/
    }

        /***************** { <deklaracia> <parse_list> ****************************/
    else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL)) {

        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r340: %d, parser_next_token in parse_list\n", result);
            return result;
        }
        result = parse_fn_declaration(parser, varBlock);
        parser->argsCounter = 0;

        if (result != EOK){
            if (debug) printf("r347: %d, parse_fn_declaration in parse_list\n", result);
            return result;
        }

        insertLast(varBlock, &parser->varList);
        printf(".....NEW BLOCK.....\n");
        result = parse_list(parser);

        if (result != EOK){
            if (debug) printf("r356: %d, parse_list in parse_list\n", result);
            return result;
        }
    }

        /*************** } <parse_list> ***********************/
    else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_VINCULUM_SMBL)) {
        printf("....END BLOCK....\n");
        tItemPtr item = parser->varList.Last;
        for(int i = 0; i < kv_size(item->data) ; i++){
            //POP VARIABLE
            printf("VARIABLE POP:%s\n",kv_A(item->data, i).id );
            parser->offset_counter--;
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
            if (debug) printf("r381: %d, parse_assign in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;


        /*********hack vyrazu ******/
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r381: %d, parser_next_token in parse_list\n", result);
            return result;
        }
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r395: %d, parse_next_token in parse_list\n", result);
                return result;
            }
        }
        /********hack vyrazu********/


        /*vyhodnotenie vyrazu*/

        printf("EXP\n");
        printf("JMPF %d\n", elseLabel);
        /**vlozenie 3AK - podmieneny skok ak podmienka nebola splnena na elseLabel**/

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r409: %d, parse_next_token in parse_list\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;
        printf("....NEW BLOCK....\n");
        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r422: %d, parse_next_token in parse_list\n", result);
            return result;
        }

        result = parse_fn_declaration(parser, varBlock);
        parser->argsCounter = 0;
        if (result != EOK)
            return result;

        insertLast(varBlock, &parser->varList);
        result = parse_list(parser);

        if (result != EOK)
            return result;
        printf("JMP %d\n", endLabel);
        /*vlozenie 3AK - skok na endLabel*/

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r440: %d, parse_next_token in parse_list\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, ELSE_KW))
            return ESYN;
        /******vlozenie 3AK - elseLabel*******/
        printf("LABEL ELSE %d\n", elseLabel);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r450: %d, parse_next_token in parse_list\n", result);
            return result;
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;
        printf("....NEW BLOCK....\n");
        tItemPtr varBlock2;
        if ((varBlock2 = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock2->data);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r463: %d, parse_next_token in parse_list\n", result);
            return result;
        }

        result = parse_fn_declaration(parser, varBlock2);
        parser->argsCounter = 0;
        if (result != EOK){
            if (debug) printf("r470: %d, parse_fn_declaration in parse_list\n", result);
            return result;
        }
        insertLast(varBlock2, &parser->varList);
        result = parse_list(parser);

        if (result != EOK)
            return result;
        /**vlozenie 3AK - endLabel***/
        printf("LABEL END %d\n", endLabel);
    }

        /** for ( <deklaracia> ; <vyraz> ; priradenie) <parse_list>**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, FOR_KW)) {
        int expLabel = parser->label;
        int assignLabel = ++parser->label;
        int beginLabel = ++parser->label;
        int endLabel = ++parser->label;
        parser->label++;
        char *hName = NULL;
        char *hNameID = NULL;
        hTabItem *tItem;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r494: %d, parse_fn_declaration in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;

        /**deklaracia riadiacej premennej**/
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r502: %d, parse_next_token in parse_list\n", result);
            return result;
        }


        if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW | AUTO_KW))
            return ESYN;

        int varType = parser->token->flags;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r494: %d, parse_next_token in parse_list\n", result);
            return result;
        }


        if (!TOKEN_IS(parser->token, ID_TYPE))
            return ESYN;

        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        hName = generate_var_name(parser->hInt++);
        parser->offset_counter++;

        tData data;
        if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token->data), hName,parser->offset_counter)) != EOK) {
            if (debug) printf("r531: %d, init_data_var in parse_list\n", result);
            return result;
        }
        item_append_data(varBlock, data);

        insertLast(varBlock, &parser->varList);

        /*vlozenie riadiacje premennej do TS*/
        tItem = createNewItem();
        tItem->name = calloc(1, strlen(hName) + 1);
        strcpy(tItem->name, hName);
        tItem->dataType = varType;
        insertHashTable(parser->table, tItem);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r546: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r551: %d, parse_next_token in parse_list\n", result);
            return result;
        }
            /*********hack vyrazu ******/
            if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r556: %d, parse_next_token in parse_list\n", result);
            return result;
        }
            while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
                if ((result = parser_next_token(parser)) != EOK) {
                    if (debug) printf("r561: %d, parse_next_token in parse_list\n", result);
                    return result;
                }
        }
            /********hack vyrazu********/

            /**vlozenie hodnoty riadiacej premennej podla vysledku parse_assign()**/
            /*******3AK, MV , #1, NULL, hName******/
        } else if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            return ESYN;
        }


        /**vlozenie 3AK  - label expLabel**/
        printf("EXP LABEL %d\n", expLabel);
        printf("EXP\n");


        /*****hack*********/
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r581: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {                ///mala by sa volat precedencna analyza
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r586: %d, parse_next_token in parse_list\n", result);
                return result;
            }
        }


        /**vyhodnotenie vyrazu**/
        printf("JMPT BEGIN %d\n", beginLabel);
        printf("JMPF END %d\n", endLabel);
        /**vlozenie 3AK - skok na beginLabel ak je vyraz true**/
        /**vlozenie 3AK - skok na endLabel ak je vyraz false**/

        printf("LABEL ASSIGNLABEL %d\n", assignLabel);
        /**vlozenie 3AK - label assignLabel**/
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r601: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        if (!TOKEN_IS(parser->token, ID_TYPE))
            return ESYN;
        if ((hNameID = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
            if ((hNameID = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                return ESEM;
        }
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r611: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r617: %d, parse_next_token in parse_list\n", result);
            return result;
        }

        /*****hack*********/
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {                ///mala by sa volat precedencna analyza
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r624: %d, parse_next_token in parse_list\n", result);
                return result;
            }
        }

        printf("ASSING EXP\n");
        /**vyhodnotenie vyrazu**/
        printf("MV %s EXPRESULT\n", hNameID);
        /**vlozenie 3AK priradnie do premennej hNameID**/
        printf("JMP EXPLABEL %d\n", expLabel);
        /**vlozenie 3AK - skok na label expLabel**/

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r637: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;

        tItemPtr varBlock2;
        if ((varBlock2 = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock2->data);

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r649: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        result = parse_fn_declaration(parser, varBlock2);
        parser->argsCounter = 0;
        if (result != EOK){
            if (debug) printf("r655: %d, parse_fn_declaration in parse_list\n", result);
            return result;
        }
        insertLast(varBlock2, &parser->varList);

        /**vlozenie 3AK - label beginLabel**/
        printf("LABEL BEGIN %d \n", beginLabel);
        result = parse_list(parser);

        if (result != EOK){
            if (debug) printf("r494: %d, parse_list in parse_list\n", result);
            return result;
        }
        parser->offset_counter--;
        deleteLast(&parser->varList);
        printf("JMP ASSIGNLABEL %d\n", assignLabel);
        /**vlozenie 3AK - skok na assignLabel**/
        printf("LABEL END %d\n", endLabel);
        /**vlozenie 3AK - label endLabel**/

    }

        /**cin |>> ID|*n ;**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, CIN_KW)) {
        char *hName = NULL;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r682: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_RIGHT_SMBL))
            return ESYN;

        while (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_RIGHT_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r690: %d, parse_next_token in parse_list\n", result);
                return result;
            }
            if (!TOKEN_IS(parser->token, ID_TYPE))
                return ESEM4;
            if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                    return ESEM;
            }
            /**vygenerovanie 3AK - nacitanie zo SV do premennej hName**/
            printf("\tCIN STDIN %s\n", hName);

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r703: %d, parse_next_token in parse_list\n", result);
                return result;
            }
        }

        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            return ESYN;
    }

        /**cout |<< term |*n  ---- term==ID || TEXT || INT || DOUBLE**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, COUT_KW)) {
        char *hName = NULL;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r717: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_LEFT_SMBL))
            return ESYN;

        while (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, DBL_ARR_LEFT_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r725: %d, parse_next_token in parse_list\n", result);
                return result;
            }
            if (TOKEN_IS(parser->token, ID_TYPE)) {
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                }
                *kl_pushp(instruction_list, parser->code) = create_COUT_pop_instr();
                /**vlozenie 3AK - vypis na STDOUT z premennej hName**/
                printf("\tCOUNT STDOUT %s\n", hName);
            }
            else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, INT_CONST)) {
                printf("\tCOUNT STDOUT %d\n", parser->token->data.iVal);
            }/**vlozenie 3AK - vypis na STDOUT cislo parser->token->data.i**/
            else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, DOUBLE_CONST)) {
                printf("\tCOUNT STDOUT %f\n", parser->token->data.dVal);
            }/**vlozenie 3AK - vypis na STDOUT cislo parser->token->data.d**/
            else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
                printf("\tCOUNT STDOUT %s\n", parser->token->data.sVal);
            }/**vlozenie 3AK - vypis na STDOUT retazec parser->token->data.s**/
            else return ESEM4;


            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r750: %d, parse_next_token in parse_list\n", result);
                return result;
            }
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            return ESYN;
    }

        /**return <EXP> ;**/
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, RETURN_KW)) {

        /*********hack**********/
        if ((result = parser_next_token(parser)) != EOK){
            if (debug) printf("r763: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r768: %d, parse_next_token in parse_list\n", result);
                return result;
            }
        }
        /*********************/

        printf("RETURN\n");
        /****vyhdontenie vyrazu***/
        /**vloznenie 3AK - skos s5**/
    }
    else if (TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW | AUTO_KW)) {
        /***deklaracia mimo zaciatku bloku**/
        result = parse_adv_declaration(parser);
        if (result != EOK)
            return result;
    }
    else return ESYN;

    if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r787: %d, parse_next_token in parse_list\n", result);
            return result;
        }
    result = parse_list(parser);
    return result;
}

result_t parse_adv_declaration(parser_t *parser) {
    result_t result = EOK;
    int varType = parser->token->flags;
    char *hName;
    hTabItem *tItem;

    if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r801: %d, parse_next_token in parse_list\n", result);
            return result;
        }

    if (!TOKEN_IS(parser->token, ID_TYPE))
        return ESYN;

    if ((result = list_foo_bar(parser->varList.Last, ZVAL_GET_STRING(&parser->token->data))) != EOK) {
        if (debug) printf("r809: %d, parse_next_token in parse_list\n", result);
        return result;
    }

    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) != NULL)
        return ESEM;

    if((tItem = searchItem(parser->table, parser->token->data.sVal)) != NULL)
        return ESEM;

    hName = generate_var_name(parser->hInt++);
    parser->offset_counter++;

    tData data;
    if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token->data), hName,parser->offset_counter)) != EOK) {
        if (debug) printf("r824: %d, init_data_var in parse_list\n", result);
        return result;
    }
    printf("Variable Push %s...offset:%d\n",parser->token->data.sVal,data.offset);
    item_append_data(parser->varList.Last, data);

    /***vytvorenie novej polozky do TS****/
    tItem = createNewItem();
    tItem->name = calloc(1, strlen(hName) + 1);
    strcpy(tItem->name, hName);
    tItem->dataType = varType;
    insertHashTable(parser->table, tItem);

    if (varType == AUTO_KW) {
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r839: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESEM3;

        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r846: %d, parse_next_token in parse_list\n", result);
            return result;
        }
        parser->assignVarName = hName;
        result = parse_assign(parser);

        if (result != EOK){
            if (debug) printf("r853: %d, parse_assing in parse_list\n", result);
            return result;
        }/*******3AK , MV , #1 ,NULL, hName******/

    } else {
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r839: %d, parse_next_token in parse_list\n", result);
            return result;
        }

        if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            return EOK;
        } else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r867: %d, parse_next_token in parse_list\n", result);
                return result;
            }
            parser->assignVarName = hName;
            result = parse_assign(parser);

            if (result != EOK){
                if (debug) printf("r874: %d, parse_next_token in parse_list\n", result);
                return result;
            }
            return EOK;

            /*******3AK , MV , #1 ,NULL, hName******/
        } else return ESYN;
    }
    return result;
}

result_t parse_fn_declaration(parser_t *parser, tItemPtr varBlock) {
    /**deklaracia premennych ----- vola sa na zacitku kazdeho bloku ****/
    result_t result = EOK;
    int varType = parser->token->flags;
    char *hName;
    hTabItem *tItem;

    if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW | AUTO_KW))
        return EOK;

    if ((result = parser_next_token(parser)) != EOK) {
        if (debug) printf("r896: %d, parse_next_token in parse_fn_declaration\n", result);
        return result;
    }


    if (!TOKEN_IS(parser->token, ID_TYPE))
        return ESYN;

    /***vlozenie do bloku***/

    if ((result = list_foo_bar(varBlock, ZVAL_GET_STRING(&parser->token->data))) != EOK) {
        if (debug) printf("r907: %d, list_foo_bar in parse_fn_declaration\n", result);
        return result;
    }

    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) != NULL)
        return ESEM;

     if((tItem = searchItem(parser->table, parser->token->data.sVal)) != NULL)
        return ESEM;

    hName = generate_var_name(parser->hInt);
    parser->hInt++;
    parser->offset_counter++;

    tData data;
    if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token->data), hName,parser->offset_counter)) != EOK) {
        if (debug) printf("r923: %d, parse_next_token in parse_fn_declaration\n", result);
        return result;
    }
    printf("Variable Push %s...offset:%d\n",parser->token->data.sVal,data.offset);
    item_append_data(varBlock, data);


    /***vytvorenie novej polozky do TS****/
    tItem = createNewItem();
    tItem->name = calloc(1, strlen(hName) + 1);
    strcpy(tItem->name, hName);
    tItem->dataType = varType;
    insertHashTable(parser->table, tItem);

    if (varType == AUTO_KW) {
        if ((result = parser_next_token(parser)) != EOK) {
        if (debug) printf("r939: %d, parse_next_token in parse_fn_declaration\n", result);
        return result;
    }


        if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESEM3;

        if ((result = parser_next_token(parser)) != EOK) {
        if (debug) printf("r948: %d, parse_next_token in parse_fn_declaration\n", result);
        return result;
    }
        parser->assignVarName = hName;
        result = parse_assign(parser);

        if (result != EOK)
            return result;

        if ((result = parser_next_token(parser)) != EOK) {
        if (debug) printf("r958: %d, parse_next_token in parse_fn_declaration\n", result);
        return result;
    }
        result = parse_fn_declaration(parser, varBlock);

        /*******3AK , MV , #1 ,NULL, hName******/

    } else { //typ int, double alebo string

        if ((result = parser_next_token(parser)) != EOK) {
        if (debug) printf("r968: %d, parse_next_token in parse_fn_declaration\n", result);
        return result;
    }
        printf("LOCAL VAR\n");

        if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r975: %d, parse_next_token in parse_fn_declaration\n", result);
                return result;
            }

            result = parse_fn_declaration(parser, varBlock);
        }
        else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, ASSIGN_SMBL)) {

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r984: %d, parse_next_token in parse_fn_declaration\n", result);
                return result;
            }
            parser->assignVarName = hName;

            result = parse_assign(parser);

            if (result != EOK){
                if (debug) printf("r992: %d, parse_assign in parse_fn_declaration\n", result);
                return result;
            }
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r996: %d, parse_next_token in parse_fn_declaration\n", result);
                return result;
            }
            result = parse_fn_declaration(parser, varBlock);

        } else return ESYN;
    }
    return result;
}

result_t parse_assign(parser_t *parser) {
    result_t result = EOK;

    if (TOKEN_HAS_TFLAG(parser->token, FN_TYPE, LENGTH_FN|SUBSTR_FN|CONCAT_FN|FIND_FN|SORT_FN)) {
        result = parse_build_in_fn(parser);
        if(result != EOK){
            if (debug) printf("r1012: %d, parse_build_in_fn in parse_assign\n", result);
            return result;
        }
    }
    else if (TOKEN_IS(parser->token, ID_TYPE)) {
        char * hName;

        hTabItem *tableItem;
        if ((tableItem = searchItem(parser->table, parser->token->data.sVal)) == NULL) {

            if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                    return ESEM;
            }

            klist_t(token_list) *tokens = kl_init(token_list);
            while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
                token_t *cpy = calloc(1, sizeof(token_t));
                copy_token(cpy, parser->token);
                *kl_pushp(token_list, tokens) = cpy;
                if ((result = parser_next_token(parser)) != EOK) {
                    if (debug) printf("r1022: %d, parse_next_token in parse_assign\n", result);
                    return result;
                }
            }

            klist_t(expr_stack) *expr = build_expression(tokens);
            klist_t(instruction_list) *expr_code = create_instructions_from_expression(expr);
            for (kliter_t(instruction_list) *it = kl_begin(expr_code); it != kl_end(expr_code); it = kl_next(it)) {
                *kl_pushp(instruction_list, parser->code) = kl_val(it);
            }

        } else {
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
            if (tItem->dataType != tableItem->dataType && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1054: %d, parse_next_token in parse_fn_declaration\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1061: %d, parse_next_token in parse_fn_declaration\n", result);
                return result;
            }

            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                result = parse_params(parser, item);
            if (result != EOK){
                if (debug) printf("r1068: %d, parse_params in parse_fn_declaration\n", result);
                return result;
            }
            if (kv_size(item->data) != parser->argsCounter1)
                return ESEM2;

            parser->argsCounter1 = 0;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1077: %d, parse_next_token in parse_fn_declaration\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            /***********3AK , JMP, LABEL, PARAMETRE, NULL***********/
        }
    } else if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {

        klist_t(token_list) *tokens = kl_init(token_list);
        while (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            token_t *cpy = calloc(1, sizeof(token_t));
            copy_token(cpy, parser->token);
            *kl_pushp(token_list, tokens) = cpy;
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1093: %d, parse_next_token in parse_fn_declaration\n", result);
                return result;
            }
        }
        klist_t(expr_stack) *expr = build_expression(tokens);
        klist_t(instruction_list) *expr_code = create_instructions_from_expression(expr);
        for (kliter_t(instruction_list) *it = kl_begin(expr_code); it != kl_end(expr_code); it = kl_next(it)) {
            *kl_pushp(instruction_list, parser->code) = kl_val(it);
        }

        /*********************/
    } else return ESYN;
    return EOK;
}

result_t parse_build_in_fn(parser_t *parser) {
    result_t result = EOK;
    char *hName;
    hTabItem *tItem = NULL;
    hTabItem *tItem1 = NULL;
    int var_offset=0;

    switch (parser->token->flags) {
        case LENGTH_FN:
            /******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != INT_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1125: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK){
                if (debug) printf("r1125: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
                hName = generate_var_name(parser->hInt);
                 printf("PUSH CONST TEXT %s\n", parser->token->data.sVal);
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
               /**********vyhladame polozku hName v TS*********/
                /****zkontroluje datovy typ --- musi byt string***/
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                } else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                printf("PUSH OFFSET: %d\n", var_offset);
            }
            else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1160: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1167: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            //printf("volanie funkcie length --- parameter: %s\n",  tItem1->value.c );
            /*************3AK -- LENGTH , pointer na polozku v ts, NULL ,NULL,***************/
            /********3AK --- MV , searchTS(#1), NULL, searchTS(assignVarName)******/


            break;

        case SUBSTR_FN:
            /******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != STRING_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1188: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;
            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1194: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
                printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
                 /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                } else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                printf("PUSH OFFSET: %d\n", var_offset);
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1220: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1227: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, INT_CONST)) {
               printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
                 /**********vyhladame polozku hName v TS*********/
                printf("step 1\n");
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                } else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                printf("step 2\n" );
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                printf("step 3\n");
                if (tItem1->dataType != INT_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                printf("PUSH OFFSET: %d\n", var_offset);
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1253: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1260: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, INT_CONST)) {
                printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != INT_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                 printf("PUSH OFFSET: %d\n", var_offset);
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1285: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1292: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;


            //printf("volanie funkcie substr--- parameter 1: %s ,parameter 2: %d, parameter 3: %d\n",  tItem1->value.c, tItem2->value.i, tItem3->value.i );
            /***********************3AK -- SUBSTR , p1 , p2 , p3******************************/
            /***********************3AK -- MV , searchTS(#1),NULL,searchTS(assignVarName)******/
            break;
        case CONCAT_FN:

            /******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != STRING_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1312: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1319: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }

            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
                printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
                 /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                printf("PUSH OFFSET: %d\n", var_offset);
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1346: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1353: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }

            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
               printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
               if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                 printf("PUSH OFFSET: %d\n", var_offset);
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1379: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1386: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            //printf("volanie funkcie concat--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);

            break;
        case FIND_FN:
            /******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != INT_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1403: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1410: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }

            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
                printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                printf("PUSH OFFSET: %d\n", var_offset);
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1437: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1444: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }

            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
                 printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem1->dataType != AUTO_KW)
                    return ESEM2;
                printf("PUSH OFFSET: %d\n", var_offset);
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1471: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1478: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            //	printf("volanie funkcie find--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);
            break;
        case SORT_FN:
            /******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != STRING_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1494: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1501: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }

            if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
                printf("PUSH CONST\n");
            } else if (TOKEN_IS(parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                        return ESEM;
                    else { var_offset = get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal); }
                }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem1->dataType != AUTO_KW){
                    return ESEM2;
                    printf("PUSH OFFSET: %d\n", var_offset);

                }
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1528: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) {
                if (debug) printf("r1535: %d, parse_next_token in parse_build_in_fn\n", result);
                return result;
            }
            if (!TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            break;
    }
    return result;
}

result_t parse_params(parser_t *parser, tItemPtr item) {
    result_t result = EOK;
    char *hName;
    hTabItem *tableItem;
    int var_offset=0;

    if (TOKEN_IS(parser->token, ID_TYPE)) {
        hTabItem *tItem1;

        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;
        if ((hName = varSearch(&parser->varList, parser->token->data.sVal)) == NULL) {
            if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token->data.sVal)) == NULL)
                return ESEM;
            else { var_offset= get_param_offset(&parser->paramList, parser->fName, parser->token->data.sVal);}
        }else { var_offset= get_var_offset(&parser->varList, parser->token->data.sVal);}
        if ((tItem1 = searchItem(parser->table, hName)) == NULL)
            return ESYS;

        if ((tableItem->dataType != AUTO_KW) && (tableItem->dataType != tItem1->dataType) && (tItem1->dataType != AUTO_KW))
            return ESEM2;
        printf("PUSH PARAM ID: %s OFFSET: %d\n", parser->token->data.sVal,var_offset);

        /****************parameter tItem1*****************/
    }
    else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, INT_CONST)) {
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;

        if (tableItem->dataType != INT_KW && tableItem->dataType != AUTO_KW)
            return ESEM2;

       printf("PUSH PARAM %d\n",parser->token->data.iVal);

        /**************************************/

    }
    else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, DOUBLE_CONST)) {
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;

        if (tableItem->dataType != DOUBLE_KW && tableItem->dataType != AUTO_KW)
            return ESEM2;

        printf("PUSH PARAM %lf\n",parser->token->data.dVal);

        /****************************************/

    }
    else if (TOKEN_HAS_TFLAG(parser->token, CONST_TYPE, TEXT_CONST)) {
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;

        if (tableItem->dataType != STRING_KW && tableItem->dataType != AUTO_KW)
            return ESEM2;

       printf("PUSH PARAM %s\n",parser->token->data.sVal );

        /**************************************/
    }
    else return ESEM2;

    parser->argsCounter1++;

    if ((result = parser_next_token(parser)) != EOK) { return result; }
    if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL)) {
        if ((result = parser_next_token(parser)) != EOK) { return result; }
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
    hTabItem *tItem;
    if (!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))
        return ESEM4;

    if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r1582: %d, parse_next_token in parse_fn_args\n", result);
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
            if (debug) printf("r1629: %d, init_data_var in parse_fn_args\n", result);
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
        if (debug) printf("r1720: %d, parse_next_token in parse_build_in_fn\n", result);
        return result;
    }

    if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
        return EOK;
    }
    else if (TOKEN_HAS_TFLAG(parser->token, SMBL_TYPE, COMMA_SMBL)) {
        if ((result = parser_next_token(parser)) != EOK) {
            if (debug) printf("r1729: %d, parse_next_token in parse_build_in_fn\n", result);
            return result;
        }
        result = parse_fn_args(parser, item);
    }
    else return ESYN;
    return result;
}
