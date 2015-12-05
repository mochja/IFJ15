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
    return scanner_get_next_token(&parser->scanner, &parser->token);
}

char __nbuffer[64];
char *generate_var_name(int number) {
    sprintf(__nbuffer, "$%d", number);
    return __nbuffer;
}

result_t parser_run(parser_t *parser) {
/**overim prvy parser->token**/
    result_t result;

    if ((result = parser_next_token(parser)) != EOK)
        return result;

    if (!TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))
        return ESYN;

    if ((result = parse_fn(parser)) != EOK) {
        return result;
    }

    // Overime ci sme naozaj na konci
    if (parser_next_token(parser) != EEOF) {
        return ESYN;
    }

    return result;
}


result_t parse_fn(parser_t *parser) {
    result_t result = EOK;

    int fType = parser->token.flags;

    if ((result = parser_next_token(parser)) != EOK) {
        return result;
    }

    if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, MAIN_KW) && fType == INT_KW) {
        /***********telo funkcie main**************/
        parser->fName = "main";

        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL)) // (
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))    // )
            return ESYN;

        tItemPtr item;
        if ((item = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(item->data);

        item->functionId = parser->fName;
        insertLast(item, &parser->paramList);

        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))// {
            return ESYN;

        /********MISSING: vlozenie 3AK -- label zaciatku funkcie main********/
        printf("LABEL MAIN\n");

        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_VINCULUM_SMBL))        //ak nie je funkcia prazdna
            result = parse_fn_body(parser);

        if (result != EOK)
            return result;

        return EOK;                /// po main uz ziadne dalsie rekurzivne volanie ---- main je posledna funkcia v programe
    }
    else if (TOKEN_IS(&parser->token, ID_TYPE)) {
        /*****************telo funkcie*************/
        parser->fName = ZVAL_GET_STRING(&parser->token.data);
        char *fLabel = parser->token.data.sVal;
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
            return result;
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
            result = parse_fn_args(parser, item);    //parsovanie argumentov funkcie

        if (result != EOK)
            return result;

        if ((tableItem = searchItem(parser->table, parser->fName)) == NULL)
            return ESYS;

        if (parser->fDeclared) {
            if (tableItem->params != parser->argsCounter)
                return ESEM;
        } else if (!parser->fDeclared) {
            tableItem->params = parser->argsCounter;
            insertLast(item, &parser->paramList);
        }

        parser->argsCounter = 0;
        //uz sa nekontroluje parser->token ")" pretoze funkcia parse_args sa vrati vzdy uspesne ukoncena tymto parser->tokenom
        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }

        if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL) && tableItem->isDefined == false) {
            tableItem->isDefined = true;
            /********MISSING: vlozenie 3AK -- label zaciatku funkcie*******/
            printf("LABEL F %s\n", fLabel);
            if ((result = parser_next_token(parser)) != EOK) { return result; }


            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_VINCULUM_SMBL))        //ak nie je funkcia prazdna
                result = parse_fn_body(parser);

            if (result != EOK)
                return result;

            /********MISSING: vlozenie 3AK -- navrat z funkcie*******/
            printf("LABEL ENDF %s\n", fLabel);
        }
        else if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL) && tableItem->isDefined == true)
            return ESEM;
        else if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            result = EOK;
        else
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) { return result; }


        if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE,
                            INT_KW | DOUBLE_KW | STRING_KW))    ///mala by nasledovat dalsia funkcia
            result = parse_fn(parser);        //rekurzivne volanie pre spracovanie dalsej funkcie
        else if (TOKEN_IS(&parser->token, EOF_TYPE))
            return ESEM;
        else
            return ESYN;    //ak nie je ziadna dalsia funkcia je to chyba
    } else if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, MAIN_KW) && fType != INT_KW)
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

    if (result != EOK)
        return result;

    insertLast(varBlock, &parser->varList);
    parser->argsCounter = 0;

    result = parse_list(parser);

    return result;
}

/**********************prikazy v jednom bloku************************************/
result_t parse_list(parser_t *parser) {
    result_t result = EOK;

    /*******************ID <priradenie> ; <parse_list>************************/
    if (TOKEN_IS(&parser->token, ID_TYPE)) {
        char *hName = NULL;
        if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
            if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                return ESEM;
        }
        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        parser->assignVarName = hName;
        result = parse_assign(parser);

        if (result != EOK)
            return result;

        printf("\tMV %s VYRAZ\n", hName);
        /*vlozenie 3AK na priradenie vysledku z funkcie parse_assign do hName*/
    }

        /***************** { <deklaracia> <parse_list> ****************************/
    else if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL)) {

        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        result = parse_fn_declaration(parser, varBlock);
        parser->argsCounter = 0;

        if (result != EOK)
            return result;

        insertLast(varBlock, &parser->varList);
        printf(".....NEW BLOCK.....\n");
        result = parse_list(parser);

        if (result != EOK)
            return result;
    }

        /*************** } <parse_list> ***********************/
    else if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_VINCULUM_SMBL)) {
        printf("....END BLOCK....\n");
        deleteLast(&parser->varList);
        return EOK;
    }
        /*********** if ( <EXP> ) { <list> } else { <parse_list> }*******************/
    else if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, IF_KW)) {

        int elseLabel = parser->label;
        int endLabel = ++parser->label;
        parser->label++;

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;


        /*********hack vyrazu ******/
        if ((result = parser_next_token(parser)) != EOK) { return result; }
        while (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) { return result; }
        }
        /********hack vyrazu********/


        /*vyhodnotenie vyrazu*/

        printf("EXP\n");
        printf("JMPF %d\n", elseLabel);
        /**vlozenie 3AK - podmieneny skok ak podmienka nebola splnena na elseLabel**/

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;
        printf("....NEW BLOCK....\n");
        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        if ((result = parser_next_token(parser)) != EOK) { return result; }

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

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (!TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, ELSE_KW))
            return ESYN;
        /******vlozenie 3AK - elseLabel*******/
        printf("LABEL ELSE %d\n", elseLabel);

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;
        printf("....NEW BLOCK....\n");
        tItemPtr varBlock2;
        if ((varBlock2 = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock2->data);

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        result = parse_fn_declaration(parser, varBlock2);
        parser->argsCounter = 0;
        if (result != EOK)
            return result;

        insertLast(varBlock2, &parser->varList);
        result = parse_list(parser);

        if (result != EOK)
            return result;
        /**vlozenie 3AK - endLabel***/
        printf("LABEL END %d\n", endLabel);
    }

        /** for ( <deklaracia> ; <vyraz> ; priradenie) <parse_list>**/
    else if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, FOR_KW)) {
        int expLabel = parser->label;
        int assignLabel = ++parser->label;
        int beginLabel = ++parser->label;
        int endLabel = ++parser->label;
        parser->label++;
        char *hName = NULL;
        char *hNameID = NULL;
        hTabItem *tItem;

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
            return ESYN;

        /**deklaracia riadiacej premennej**/
        if ((result = parser_next_token(parser)) != EOK) { return result; }


        if (!TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW | AUTO_KW))
            return ESYN;

        int varType = parser->token.flags;

        if ((result = parser_next_token(parser)) != EOK) { return result; }


        if (!TOKEN_IS(&parser->token, ID_TYPE))
            return ESYN;

        tItemPtr varBlock;
        if ((varBlock = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock->data);

        hName = generate_var_name(parser->hInt++);

        tData data;
        if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token.data), hName)) != EOK) {
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

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, ASSIGN_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) { return result; }

            /*********hack vyrazu ******/
            if ((result = parser_next_token(parser)) != EOK) { return result; }
            while (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
                if ((result = parser_next_token(parser)) != EOK) { return result; }
            }
            /********hack vyrazu********/

            /**vlozenie hodnoty riadiacej premennej podla vysledku parse_assign()**/
            /*******3AK, MV , #1, NULL, hName******/
        } else if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            return ESYN;
        }


        /**vlozenie 3AK  - label expLabel**/
        printf("EXP LABEL %d\n", expLabel);
        printf("EXP\n");


        /*****hack*********/
        if ((result = parser_next_token(parser)) != EOK) { return result; }
        while (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {                ///mala by sa volat precedencna analyza
            if ((result = parser_next_token(parser)) != EOK) { return result; }
        }


        /**vyhodnotenie vyrazu**/
        printf("JMPT BEGIN %d\n", beginLabel);
        printf("JMPF END %d\n", endLabel);
        /**vlozenie 3AK - skok na beginLabel ak je vyraz true**/
        /**vlozenie 3AK - skok na endLabel ak je vyraz false**/

        printf("LABEL ASSIGNLABEL %d\n", assignLabel);
        /**vlozenie 3AK - label assignLabel**/
        if ((result = parser_next_token(parser)) != EOK) { return result; }
        if (!TOKEN_IS(&parser->token, ID_TYPE))
            return ESYN;
        if ((hNameID = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
            if ((hNameID = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                return ESEM;
        }
        if ((result = parser_next_token(parser)) != EOK) { return result; }
        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;
        if ((result = parser_next_token(parser)) != EOK) { return result; }

        /*****hack*********/
        while (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {                ///mala by sa volat precedencna analyza
            if ((result = parser_next_token(parser)) != EOK) { return result; }
        }

        printf("ASSING EXP\n");
        /**vyhodnotenie vyrazu**/
        printf("MV %s EXPRESULT\n", hNameID);
        /**vlozenie 3AK priradnie do premennej hNameID**/
        printf("JMP EXPLABEL %d\n", expLabel);
        /**vlozenie 3AK - skok na label expLabel**/

        if ((result = parser_next_token(parser)) != EOK) { return result; }
        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_VINCULUM_SMBL))
            return ESYN;

        tItemPtr varBlock2;
        if ((varBlock2 = calloc(1, sizeof(struct tItem))) == NULL)
            return ESYS;
        kv_init(varBlock2->data);

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        result = parse_fn_declaration(parser, varBlock2);
        parser->argsCounter = 0;
        if (result != EOK)
            return result;

        insertLast(varBlock2, &parser->varList);

        /**vlozenie 3AK - label beginLabel**/
        printf("LABEL BEGIN %d \n", beginLabel);
        result = parse_list(parser);

        if (result != EOK)
            return result;
        deleteLast(&parser->varList);
        printf("JMP ASSIGNLABEL %d\n", assignLabel);
        /**vlozenie 3AK - skok na assignLabel**/
        printf("LABEL END %d\n", endLabel);
        /**vlozenie 3AK - label endLabel**/

    }

        /**cin |>> ID|*n ;**/
    else if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, CIN_KW)) {
        char *hName = NULL;

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, DBL_ARR_RIGHT_SMBL))
            return ESYN;

        while (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, DBL_ARR_RIGHT_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_IS(&parser->token, ID_TYPE))
                return ESEM4;
            if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                    return ESEM;
            }
            /**vygenerovanie 3AK - nacitanie zo SV do premennej hName**/
            printf("\tCIN STDIN %s\n", hName);

            if ((result = parser_next_token(parser)) != EOK) { return result; }
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            return ESYN;
    }

        /**cout |<< term |*n  ---- term==ID || TEXT || INT || DOUBLE**/
    else if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, COUT_KW)) {
        char *hName = NULL;

        if ((result = parser_next_token(parser)) != EOK) { return result; }
        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, DBL_ARR_LEFT_SMBL))
            return ESYN;

        while (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, DBL_ARR_LEFT_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_IS(&parser->token, ID_TYPE)) {
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                /**vlozenie 3AK - vypis na STDOUT z premennej hName**/
                printf("\tCOUNT STDOUT %s\n", hName);
            }
            else if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, INT_CONST)) {
                printf("\tCOUNT STDOUT %d\n", parser->token.data.iVal);
            }/**vlozenie 3AK - vypis na STDOUT cislo parser->token.data.i**/
            else if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, DOUBLE_CONST)) {
                printf("\tCOUNT STDOUT %f\n", parser->token.data.dVal);
            }/**vlozenie 3AK - vypis na STDOUT cislo parser->token.data.d**/
            else if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                printf("\tCOUNT STDOUT %s\n", parser->token.data.sVal);
            }/**vlozenie 3AK - vypis na STDOUT retazec parser->token.data.s**/
            else return ESEM4;


            if ((result = parser_next_token(parser)) != EOK) { return result; }
        }
        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
            return ESYN;
    }

        /**return <EXP> ;**/
    else if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, RETURN_KW)) {

        /*********hack**********/
        if ((result = parser_next_token(parser)) != EOK) { return result; }
        while (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) { return result; }
        }
        /*********************/

        printf("RETURN\n");
        /****vyhdontenie vyrazu***/
        /**vloznenie 3AK - skos s5**/
    }
    else if (TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW | AUTO_KW)) {
        /***deklaracia mimo zaciatku bloku**/
        result = parse_adv_declaration(parser);
        if (result != EOK)
            return result;
    }
    else return ESYN;

    if ((result = parser_next_token(parser)) != EOK) { return result; }
    result = parse_list(parser);
    return result;
}

result_t parse_adv_declaration(parser_t *parser) {
    result_t result = EOK;
    int varType = parser->token.flags;
    char *hName;
    hTabItem *tItem;

    if ((result = parser_next_token(parser)) != EOK) { return result; }

    if (!TOKEN_IS(&parser->token, ID_TYPE))
        return ESYN;

    if ((result = list_foo_bar(parser->varList.Last, ZVAL_GET_STRING(&parser->token.data))) != EOK) {
        return result;
    }

    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) != NULL)
        return ESEM;

    hName = generate_var_name(parser->hInt++);

    tData data;
    if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token.data), hName)) != EOK) {
        return result;
    }
    item_append_data(parser->varList.Last, data);

    /***vytvorenie novej polozky do TS****/
    tItem = createNewItem();
    tItem->name = calloc(1, strlen(hName) + 1);
    strcpy(tItem->name, hName);
    tItem->dataType = varType;
    insertHashTable(parser->table, tItem);

    if (varType == AUTO_KW) {
        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }

        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }
        parser->assignVarName = hName;
        result = parse_assign(parser);

        if (result != EOK)
            return result;
        /*******3AK , MV , #1 ,NULL, hName******/

    } else {
        if ((result = parser_next_token(parser)) != EOK) { return result; }

        if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            return EOK;
        } else if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, ASSIGN_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                return result;
            }
            parser->assignVarName = hName;
            result = parse_assign(parser);

            if (result != EOK)
                return result;

            return EOK;

            /*******3AK , MV , #1 ,NULL, hName******/
        } else return ESYN;
    }
    return result;
}

result_t parse_fn_declaration(parser_t *parser, tItemPtr varBlock) {
    /**deklaracia premennych ----- vola sa na zacitku kazdeho bloku ****/
    result_t result = EOK;
    int varType = parser->token.flags;
    char *hName;
    hTabItem *tItem;

    if (!TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW | AUTO_KW))
        return EOK;

    if ((result = parser_next_token(parser)) != EOK) { return result; }


    if (!TOKEN_IS(&parser->token, ID_TYPE))
        return ESYN;

    /***vlozenie do bloku***/

    if ((result = list_foo_bar(varBlock, ZVAL_GET_STRING(&parser->token.data))) != EOK) {
        return result;
    }

    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) != NULL)
        return ESEM;

    hName = generate_var_name(parser->hInt);
    parser->hInt++;

    tData data;
    if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token.data), hName)) != EOK) {
        return result;
    }
    item_append_data(varBlock, data);

    /***vytvorenie novej polozky do TS****/
    tItem = createNewItem();
    tItem->name = calloc(1, strlen(hName) + 1);
    strcpy(tItem->name, hName);
    tItem->dataType = varType;
    insertHashTable(parser->table, tItem);

    if (varType == AUTO_KW) {
        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }


        if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, ASSIGN_SMBL))
            return ESYN;

        if ((result = parser_next_token(parser)) != EOK) {
            return result;
        }
        parser->assignVarName = hName;
        result = parse_assign(parser);

        if (result != EOK)
            return result;

        if ((result = parser_next_token(parser)) != EOK) { return result; }
        result = parse_fn_declaration(parser, varBlock);

        /*******3AK , MV , #1 ,NULL, hName******/

    } else { //typ int, double alebo string

        if ((result = parser_next_token(parser)) != EOK) { return result; }

        printf("LOCAL VAR\n");

        if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) {
                return result;
            }

            result = parse_fn_declaration(parser, varBlock);
        }
        else if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, ASSIGN_SMBL)) {

            if ((result = parser_next_token(parser)) != EOK) {
                return result;
            }

            parser->assignVarName = hName;

            result = parse_assign(parser);

            if (result != EOK)
                return result;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            result = parse_fn_declaration(parser, varBlock);

        } else return ESYN;
    }
    return result;
}

result_t parse_assign(parser_t *parser) {
    result_t result = EOK;

    if (TOKEN_HAS_TFLAG(&parser->token, FN_TYPE, LENGTH_FN | SUBSTR_FN | CONCAT_FN | FIND_FN | SORT_FN)) {
        result = parse_build_in_fn(parser);
    }
    else if (TOKEN_IS(&parser->token, ID_TYPE)) {
        hTabItem *tableItem;
        if ((tableItem = searchItem(parser->table, parser->token.data.sVal)) == NULL) {
            /*********hack**********/                ///precedencna
            while (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
                if ((result = parser_next_token(parser)) != EOK) { return result; }
            }
            /*********************/
        } else {
            /**volanie uzivatelskej funkcie**/
            tItemPtr item;
            item = parser->paramList.First;
            while (item != NULL) {
                if (!strcmp(item->functionId, parser->token.data.sVal))
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

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                result = parse_params(parser, item);
            if (result != EOK)
                return result;

            if (tableItem->params != parser->argsCounter1)
                return ESEM3;
            parser->argsCounter1 = 0;


            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            /***********3AK , JMP, LABEL, PARAMETRE, NULL***********/
        }
    } else if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {

        /*********hack**********/                ///precedencna
        while (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL)) {
            if ((result = parser_next_token(parser)) != EOK) { return result; }
        }
        /*********************/
    } else return ESEM;
    return EOK;
}

result_t parse_build_in_fn(parser_t *parser) {
    result_t result = EOK;
    char *hName;
    hTabItem *tItem = NULL;
    hTabItem *tItem1 = NULL;
    hTabItem *tItem2 = NULL;
    hTabItem *tItem3 = NULL;

    switch (parser->token.flags) {
        case LENGTH_FN:
            /******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != INT_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem1 = createNewItem();
                tItem1->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem1->name, hName);
                tItem1->dataType = STRING_KW;
                tItem1->sVal = parser->token.data.sVal;
                insertHashTable(parser->table, tItem1);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                /****zkontroluje datovy typ --- musi byt string***/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            }
            else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
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

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem1 = createNewItem();
                tItem1->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem1->name, hName);
                tItem1->dataType = STRING_KW;
                tItem1->sVal = parser->token.data.sVal;
                insertHashTable(parser->table, tItem1);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, INT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem2 = createNewItem();
                tItem2->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem2->name, hName);
                tItem2->dataType = INT_KW;
                tItem2->iVal = parser->token.data.iVal;
                insertHashTable(parser->table, tItem2);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }

                if ((tItem2 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem2->dataType != INT_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }


            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, INT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem3 = createNewItem();
                tItem3->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem3->name, hName);
                tItem3->dataType = INT_KW;
                tItem3->iVal = parser->token.data.iVal;
                insertHashTable(parser->table, tItem3);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem2 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem2->dataType != INT_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
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

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem1 = createNewItem();
                tItem1->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem1->name, hName);
                tItem1->dataType = STRING_KW;
                tItem1->sVal = parser->token.data.sVal;
                insertHashTable(parser->table, tItem1);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                /**********vyhladame polozku hName v TS*********/
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem2 = createNewItem();
                tItem2->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem2->name, hName);
                tItem2->dataType = STRING_KW;
                tItem2->sVal = parser->token.data.sVal;
                insertHashTable(parser->table, tItem2);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem2 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem2->dataType != STRING_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            //printf("volanie funkcie concat--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);

            break;
        case FIND_FN:
            /******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != INT_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem1 = createNewItem();
                tItem1->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem1->name, hName);
                tItem1->dataType = STRING_KW;
                tItem1->sVal = parser->token.data.sVal;
                insertHashTable(parser->table, tItem1);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, COMMA_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem2 = createNewItem();
                tItem2->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem2->name, hName);
                tItem2->dataType = STRING_KW;
                tItem2->sVal = parser->token.data.sVal;
                insertHashTable(parser->table, tItem2);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem2 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem2->dataType != STRING_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            //	printf("volanie funkcie find--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);
            break;
        case SORT_FN:
            /******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
            if ((tItem = searchItem(parser->table, parser->assignVarName)) == NULL)
                return ESYS;
            if (tItem->dataType != STRING_KW && tItem->dataType != AUTO_KW)
                return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, LEFT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }

            if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
                hName = generate_var_name(parser->hInt);
                parser->hInt++;
                tItem1 = createNewItem();
                tItem1->name = calloc(1, strlen(hName) + 1);
                strcpy(tItem1->name, hName);
                tItem1->dataType = STRING_KW;
                tItem1->sVal = parser->token.data.sVal;
                insertHashTable(parser->table, tItem1);
            } else if (TOKEN_IS(&parser->token, ID_TYPE)) {
                /**********vyhladame polozku hName v TS*********/
                if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
                    if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                        return ESEM;
                }
                if ((tItem1 = searchItem(parser->table, hName)) == NULL)
                    return ESYS;
                if (tItem1->dataType != STRING_KW && tItem2->dataType != AUTO_KW)
                    return ESEM2;
            } else return ESEM2;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL))
                return ESYN;

            if ((result = parser_next_token(parser)) != EOK) { return result; }
            if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, SEMICOLON_SMBL))
                return ESYN;

            break;
    }
    return result;
}

result_t parse_params(parser_t *parser, tItemPtr item) {
    result_t result = EOK;
    char *hName;
    hTabItem *tableItem;

    if (TOKEN_IS(&parser->token, ID_TYPE)) {
        hTabItem *tItem1;

        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;
        if ((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL) {
            if ((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
                return ESEM;
        }
        if ((tItem1 = searchItem(parser->table, hName)) == NULL)
            return ESYS;

        if ((tableItem->dataType != AUTO_KW) && (tableItem->dataType != tItem1->dataType) && (tItem1->dataType != AUTO_KW))
            return ESEM2;
        printf("ID ide\n");

        /****************parameter tItem1*****************/
    }
    else if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, INT_CONST)) {
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;

        if (tableItem->dataType != INT_KW && tableItem->dataType != AUTO_KW)
            return ESEM2;

        hTabItem *tItem1 = createNewItem();
        hName = generate_var_name(parser->hInt);
        parser->hInt++;
        tItem1->name = calloc(1, strlen(hName) + 1);
        strcpy(tItem1->name, hName);
        tItem1->dataType = INT_KW;
        tItem1->iVal = parser->token.data.iVal;
        insertHashTable(parser->table, tItem1);

        /**************************************/

    }
    else if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, DOUBLE_CONST)) {
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;

        if (tableItem->dataType != DOUBLE_KW && tableItem->dataType != AUTO_KW)
            return ESEM2;

        hTabItem *tItem1 = createNewItem();
        hName = generate_var_name(parser->hInt);
        parser->hInt++;
        tItem1->name = calloc(1, strlen(hName) + 1);
        strcpy(tItem1->name, hName);
        tItem1->dataType = DOUBLE_KW;
        tItem1->dVal = parser->token.data.dVal;
        insertHashTable(parser->table, tItem1);

        /****************************************/

    }
    else if (TOKEN_HAS_TFLAG(&parser->token, CONST_TYPE, TEXT_CONST)) {
        if ((tableItem = searchItem(parser->table, kv_A(item->data, parser->argsCounter1).hid)) == NULL)
            return ESYS;

        if (tableItem->dataType != STRING_KW && tableItem->dataType != AUTO_KW)
            return ESEM2;

        hTabItem *tItem1 = createNewItem();
        hName = generate_var_name(parser->hInt++);
        tItem1->name = calloc(1, strlen(hName) + 1);
        strcpy(tItem1->name, hName);
        tItem1->dataType = STRING_KW;
        tItem1->sVal = calloc(1, sizeof(char) * (1 + strlen(ZVAL_GET_STRING(&parser->token.data))));
        strcpy(tItem1->sVal, ZVAL_GET_STRING(&parser->token.data));
        insertHashTable(parser->table, tItem1);

        /**************************************/
    }
    else return ESEM2;

    parser->argsCounter1++;

    if ((result = parser_next_token(parser)) != EOK) { return result; }
    if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, COMMA_SMBL)) {
        if ((result = parser_next_token(parser)) != EOK) { return result; }
        result = parse_params(parser, item);
    }
    else if (!TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
        return EOK;
    }
    else return ESYN;

    return result;
}

result_t parse_fn_args(parser_t *parser, tItemPtr item) {
    /******spracovanie argumentov funkcii********/
    result_t result;

    int varType = parser->token.flags;
    hTabItem *tItem;
    if (!TOKEN_HAS_TFLAG(&parser->token, KW_TYPE, INT_KW|DOUBLE_KW|STRING_KW))
        return ESYN;

    if ((result = parser_next_token(parser)) != EOK) { return result; }

    if (!TOKEN_IS(&parser->token, ID_TYPE))
        return ESYN;

    if (parser->fDeclared) {
        char *hName = NULL;
        int i = 0;
        while (i < kv_size(item->data)) {
            if (!strcmp(parser->token.data.sVal, kv_A(item->data, i).id)) {
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
            if (!strcmp(parser->token.data.sVal, kv_A(item->data, i).id))
                return ESEM;
            i++;
        }

        tData data;
        if ((result = init_data_var(&data, ZVAL_GET_STRING(&parser->token.data), hName)) != EOK) {
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
    if ((result = parser_next_token(parser)) != EOK) { return result; }

    if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, RIGHT_CULUM_SMBL)) {
        return EOK;
    }
    else if (TOKEN_HAS_TFLAG(&parser->token, SMBL_TYPE, COMMA_SMBL)) {
        if ((result = parser_next_token(parser)) != EOK) { return result; }
        result = parse_fn_args(parser, item);
    }
    else return ESYN;
    return result;
}
