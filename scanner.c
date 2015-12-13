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

#include "scanner.h"

/*********************zoznam stavov***********************************/
enum {
    STATE_START = 0,            //pociatocny stav
    STATE_K_OR_ID,              //klucove slovo alebo ID
    STATE_DEV_OR_COM,           // / alebo // alebo /*
    STATE_LINE_COM,             // //
    STATE_BLOCK_COM,            // /*
    STATE_BLOCK_COM_END,        // */
    STATE_EQ_OR_AS,             // = alebo ==
    STATE_L_ARR_LQ,             // < alebo <<
    STATE_R_ARR_MQ,             // > alebo >>
    STATE_NOT,                  // !=
    STATE_NUMBER,               // cislo int
    STATE_D_NUMBER,             // cislo double
    STATE_D_NUMBER2,            // cislo double
    STATE_E,                    // exponent
    STATE_E2,                   // cislo s exponentom
    STATE_STRING,               // retazec
    STATE_ESCAPE,               // escape sekvencia napr. \n
    STATE_X,                    // escape sekvencia \xDD , kde dd je hex. cislo
};

result_t init_scanner(scanner_t *s, char *source) {

    if ((s->source = malloc(sizeof(char) * (strlen(source) + 1))) == NULL) {
        return ESYS;
    }

    strcpy(s->source, source);
    s->line = 1;
    s->pos = 0;

    return EOK;
}

void scanner_dispose(scanner_t *s) {
    free(s->source);
}

result_t scanner_get_next_token(scanner_t *scanner, token_t *ddest)
{
    int state = STATE_START;
    char c[2];
    c[1] = '\0';

    char buff[2048];
    buff[0] = '\0';

    token_t token;
    token_t *dest = &token;
    dest->type = EOF_TYPE;
    dest->flags = 0;

    while ((c[0] = *(scanner->source + scanner->pos++)) != '\0') {

        unsigned char *cx = (unsigned char *) c;

        /************** novy riadok ***********************/
        if (*cx == '\n') {
            scanner->line++;
        }

        switch (state) {
            case STATE_START:

                /***********mezdera, biely znak*****************/
                if (isspace(*cx)) {
                    state = STATE_START;
                }

                /*************Pismeno alebo znak '_'***********/
                else if (isalpha(*cx) || *cx == '_') {
                    strcpy(buff, c);
                    state = STATE_K_OR_ID;
                }

                /************cxislo *****************************/
                else if (isdigit(*cx)) {
                    strcpy(buff, c);
                    state = STATE_NUMBER;
                }
                /*******************string*************************/
                else if (*cx == '"') {
                    buff[0] = '\0';
                    state = STATE_STRING;
                }

                /*************komentar alebo delenie ***************/
                else if (*cx == '/') {
                    state = STATE_DEV_OR_COM;
                }

                else if (*cx == '=') {
                    state = STATE_EQ_OR_AS;
                }

                else if (*cx == '<') {
                    state = STATE_L_ARR_LQ;
                }

                else if (*cx == '>') {
                    state = STATE_R_ARR_MQ;
                }

                else if (*cx == '!') {
                    state = STATE_NOT;
                }

                else if (*cx == ';') {
                    token_set_symbol(dest, SEMICOLON_SMBL);
                }

                else if (*cx == '(') {
                    token_set_symbol(dest, LEFT_CULUM_SMBL);
                }

                else if (*cx == ')') {
                    token_set_symbol(dest, RIGHT_CULUM_SMBL);
                }

                else if (*cx == '{') {
                    token_set_symbol(dest, LEFT_VINCULUM_SMBL);
                }

                else if (*cx == '}') {
                    token_set_symbol(dest, RIGHT_VINCULUM_SMBL);
                }

                else if (*cx == ',') {
                    token_set_symbol(dest, COMMA_SMBL);
                }

                else if (*cx == '+') {
                    token_set_symbol(dest, PLUS_SMBL);
                }

                else if (*cx == '-') {
                    token_set_symbol(dest, MINUS_SMBL);

                }

                else if (*cx == '*') {
                    token_set_symbol(dest, MULTIPLY_SMBL);
                }

                else {
                    fprintf(stderr, "LEX Error: Line: %lu , Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }

            break;

            /*****klucove slovlo alebo ID **********/
            case STATE_K_OR_ID:
                if (isalnum(*cx) || *cx == '_') {  // ak je c pismeno, cislo alebo znak _ prida sa do retazca
                    strcat(buff, c);
                }

                /*ak je c niektory z povolenych znakov*/
                else if( *cx == ';' || *cx == '(' || *cx == ')' || *cx == '{' || *cx == '}' || *cx == ',' ||
                         *cx == '.' || *cx == '=' || *cx == '<' || *cx == '>' || *cx == '!' || *cx == '+' ||
                         *cx == '-' || *cx == '*' || *cx == '/' || *cx == '"' || isspace(*cx) || *cx == EOF)
                {
                    if (*cx == '\n') {
                        scanner->line--;
                    }

                    *cx = (unsigned char) *(scanner->source + --scanner->pos);

                    /*kontrola ci retazec nie je klucove slovo */
                    if(!strcmp(buff, "auto"))    { token_set_kw(dest, AUTO_KW);   } else
                    if(!strcmp(buff, "cin"))     { token_set_kw(dest, CIN_KW);    } else
                    if(!strcmp(buff, "cout"))    { token_set_kw(dest, COUT_KW);   } else
                    if(!strcmp(buff, "double"))  { token_set_kw(dest, DOUBLE_KW); } else
                    if(!strcmp(buff, "else"))    { token_set_kw(dest, ELSE_KW);   } else
                    if(!strcmp(buff, "for"))     { token_set_kw(dest, FOR_KW);    } else
                    if(!strcmp(buff, "if"))      { token_set_kw(dest, IF_KW);     } else
                    if(!strcmp(buff, "int"))     { token_set_kw(dest, INT_KW);    } else
                    if(!strcmp(buff, "return"))  { token_set_kw(dest, RETURN_KW); } else
                    if(!strcmp(buff, "string"))  { token_set_kw(dest, STRING_KW); } else
                    if(!strcmp(buff, "main"))    { token_set_kw(dest, MAIN_KW);   } else
                    if(!strcmp(buff, "length"))  { token_set_fn(dest, LENGTH_FN); } else
                    if(!strcmp(buff, "substr"))  { token_set_fn(dest, SUBSTR_FN); } else
                    if(!strcmp(buff, "concat"))  { token_set_fn(dest, CONCAT_FN); } else
                    if(!strcmp(buff, "find"))    { token_set_fn(dest, FIND_FN);   } else
                    if(!strcmp(buff, "sort"))    { token_set_fn(dest, SORT_FN);   } else
                    {
                        token_set_var(dest, buff);
                    }
                }
                else { // v pripade nepovoleneho znaku dochadza k chybe
                    fprintf(stderr,"LEX Error: Line: %lu , Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                };
            break;

            /*****komentar alebo delenie*******/
            case STATE_DEV_OR_COM:
                if (*cx == '/') {
                    state = STATE_LINE_COM;
                }
                else if (*cx == '*') {
                    state = STATE_BLOCK_COM;
                }
                else {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);
                    token_set_symbol(dest, DEVIDE_SMBL);
                }
            break;

            /***********riadkovy komentar**********/
            case STATE_LINE_COM:
                if (*cx == '\n') {
                    state = STATE_START;
                }
            break;

            /**********blokovy komentar ************/

            case STATE_BLOCK_COM:
                if (*cx == '*') {
                    state = STATE_BLOCK_COM_END;
                }
            break;

            /*********koniec blokoveho komentara ******/
            case STATE_BLOCK_COM_END:
                if (*cx == '/') {
                    state = STATE_START;
                }
                else {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);
                    state = STATE_BLOCK_COM;
                }
            break;

            /************* = alebo == ***********************/
            case STATE_EQ_OR_AS:
                if (*cx == '=') {
                    token_set_symbol(dest, EQUALS_SMBL);
                }
                else {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);
                    token_set_symbol(dest, ASSIGN_SMBL);
                }
            break;

            /***************< alebo << alebo <= **************/
            case STATE_L_ARR_LQ:
                if (*cx == '<')
                    token_set_symbol(dest, DBL_ARR_LEFT_SMBL);
                else if (*cx == '=')
                    token_set_symbol(dest, LESS_OR_EQUAL_SMBL);
                else {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);
                    token_set_symbol(dest, LEFT_ARROW_SMBL);
                }
            break;

            /*************** > alebo >> alebo >= **************/
            case STATE_R_ARR_MQ:
                if (*cx == '>')
                    token_set_symbol(dest, DBL_ARR_RIGHT_SMBL);
                else if (*cx == '=')
                    token_set_symbol(dest, MORE_OR_EQUAL_SMBL);
                else {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);
                    token_set_symbol(dest, RIGHT_ARROW_SMBL);
                }
            break;

            /********************* !=  *********************************/
            case STATE_NOT:
                if(*cx == '=')
                    token_set_symbol(dest, NOT_EQUAL_SMBL);
                else{
                    return ELEX;
                }
            break;

            /*************************cxislo******************************/
            case STATE_NUMBER:

                if (isdigit(*cx)) {                         //ak je znak cislo prida sa do retazca
                    strcat(buff, c);
                }

                else if (*cx == '.') {                      //ak je znak '.' prechadza sa do stavu cisla double, pridame ro retazca
                    strcat(buff, c);
                    state = STATE_D_NUMBER;
                }

                else if(*cx == 'E' || *cx == 'e') {          // ak je c E alebo e prechadza sa do stavu cisla s exp , pridame do retazca
                    strcat(buff, c);
                    state = STATE_E;
                }

                // c je niektory z povloenych znakov
                else if( *cx == ';' || *cx == '(' || *cx == ')' || *cx == '{' || *cx == '}' || *cx == ',' ||
                         *cx == '=' || *cx == '<' || *cx == '>' || *cx == '!' || *cx == '+' || *cx == '-' ||
                         *cx == '*' || *cx == '/' || *cx == '"' || isspace(*cx))
                {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);

                    token_set_int_const(dest, (int) strtol(buff, NULL, 10));
                }
                else {   //Lex. chyba pri zadani nepovolenych znakov
                    fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }
            break;

            /******** prvy stav cisla double - sem sa dostaneme napr s cislom 1. ********
            ******************v tomto stave ocakavame iba cislo**************************/
            case STATE_D_NUMBER:
                if (isdigit(*cx)) {         // ak je znak c cislo pokracujeme do stavu STATE_D_NUMBER2 , pridame do retazca
                    strcat(buff, c);
                    state = STATE_D_NUMBER2;
                }
                else {                   // ak je c nieje cislo jedna sa o Lex. chybu
                    fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }
            break;

            /***************************druhy stav cisla double****************************************/
            case STATE_D_NUMBER2:
                if (*cx == 'E' || *cx == 'e') {       // znak c je pismeno e alebo E - cislo double s exponentom , pridame do retazca
                    strcat(buff, c);
                    state = STATE_E;
                }

                else if (isdigit(*cx)) {            // znak c je cislo - prida sa do retazca
                    strcat(buff, c);
                }

                //c je niektory z povolenych znakov
                else if (*cx == ';' || *cx == '(' || *cx == ')' || *cx == '{' || *cx == '}' || *cx == ',' ||
                         *cx == '=' || *cx == '<' || *cx == '>' || *cx == '!' || *cx == '+' || *cx == '-' ||
                         *cx == '*' || *cx == '/' || *cx == '"' || isspace(*cx))
                {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);

                    token_set_double_const(dest, strtod(buff, NULL));
                }
                else {  //ak je c nepovoleny znak - Lex chyba
                    fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }
            break;

            /*****stav cisla s exponentom,  sem sa dostavame napr s cislom 1e alebo 1,1e*******/
            case STATE_E:
                if(*cx == '-' || *cx == '+' || isdigit(*cx)){ //nasledujuci znak musi byt bud cislo, + alebo -
                    strcat(buff, c);
                    state = STATE_E2;
                }
                else {   //v pripade nepovoleneho znaku - Lex. chyba
                    fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }
            break;

            /*****************pokracovanie cisla s exp ********************************/
            case STATE_E2:
                if (isdigit(*cx)) {     //ak je c cislo prida sa do retazca
                    strcat(buff, c);
                }

                //ak je c povoleny znak
                else if(*cx == ';' || *cx == '(' || *cx == ')' || *cx == '{' || *cx == '}' || *cx == ',' ||
                        *cx == '=' || *cx == '<' || *cx == '>' || *cx == '!' || *cx == '+' || *cx == '-' ||
                        *cx == '*' || *cx == '/' || *cx == '"' || isspace(*cx))
                {
                    if (*cx == '\n') {
                        scanner->line--;
                    }
                    *cx = (unsigned char) *(scanner->source + --scanner->pos);

                    token_set_double_const(dest, strtod(buff, NULL));
                }
                else { // nepovoleny znak - Lex chyba
                    fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }
            break;

            /******************STRING******************************/
            case STATE_STRING:
                if (*cx == '"') {  //ak c je znak " znamena to ukoncenie retazca - nastavy sa token a pointer na string
                    token_set_text_const(dest, buff);
                }
                else if (*cx == '\\') {        //znak \ - pokracuje sa do escape sekvencie
                    state = STATE_ESCAPE;
                }
                else if (*cx == EOF || *cx == '\0')          // koniec suboru alebo retazca - Lex chyba
                {
                    fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }
                else{           // prida znak do retazca
                    strcat(buff, c);
                }
            break;

            /************** ESCAPE SEKVENCIA NAPR \n**********************/
            case STATE_ESCAPE:
                if (*cx == 'x') {       // ak je znak za  \ x prejdeme do stavu STATE_X
                    state = STATE_X;
                }
                else if (*cx == 'n') {  //c je znak n pridame do retazca '\n'
                    strcat(buff, "\n");
                    state = STATE_STRING;
                }

                else if (*cx == 't') {  //c je znak t pridame do retazca '\t'
                    strcat(buff, "\t");
                    state = STATE_STRING;
                }

                else if (*cx == '\\') { //c je znak \ pridame do retazca '\\'
                    strcat(buff, "\\");
                    state = STATE_STRING;
                }

                else if (*cx == '"') { //c je znak " pridame do retazca '\"'
                    strcat(buff, "\"");
                    state = STATE_STRING;
                }
                else {   //iny znak znamena Lex. chybu
                    fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                    return ELEX;
                }
            break;

            /************************* \X ***********************************************/
            /*********nasledujuce 2 znaky ocakavame v rozsahu 0-1 alebo a-f || A-F*******/
            case STATE_X: {
                char ascii[3]; //pole o 2 prvkoch do ktoreho ulozime 2 nacitane znaky
                ascii[2] = '\0';

                for(int counter = 0 ; counter < 2 ; counter++){ //nacitaju sa 2 znaky
                    if ((*cx < 30 && *cx > 39) && (*cx < 41 && *cx > 46) && (*cx < 61 && *cx > 66)){ // kontorla podmienky znaku
                        fprintf(stderr, "LEX Error: Line: %lu, Unknown token: '%c'\n", scanner->line, *cx);
                        return ELEX;
                    }
                    else {//ak bol znak spravny prida sa do pola ascii[];
                        ascii[counter] = *cx;
                        *cx = (unsigned char) *(scanner->source + ++scanner->pos);
                    }
                }

                if (*cx == '\n') {
                    scanner->line--;
                }
                --scanner->pos;

                if (ascii[0] == '0' && ascii[1] == '0') {
                    return ELEX;
                }
                *cx = (unsigned char) strtol(ascii, NULL, 16);

                strcat(buff, c);
                state = STATE_STRING;
                break;
            }
        }

        if (dest->type != EOF_TYPE) {
            memcpy(ddest, dest, sizeof(token_t));
            return EOK;
        }
    }

    if (state != STATE_START) {
        return ELEX;
    }

    ddest->type = EOF_TYPE;
    ddest->flags = 0x00;

    debug_print("%s\n", "EOF<");
    return EEOF;
}
