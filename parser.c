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

static inline void generate_var_name(char *dest, const int number) {
    sprintf(dest, "$%d" , number);
}

result_t init_parser(parser_t *parser, const char *source)
{
    if ((parser->source = malloc((strlen(source) + 1) * sizeof(char))) == NULL) {
        return ESYS;
    }

    strcpy(parser->source, source);

	listInit(&parser->varList);
	listInit(&parser->paramList);

	parser->fName = NULL;
	parser->argsCounter = 0;
	parser->argsCounter1 = 0;
	parser->fDeclared = false;
	parser->hInt = 0;
	parser->label = 1;
	parser->assignVarName = NULL;

	if((parser->table = initHashTable(MAX_HTSIZE)) == NULL)
		return ESYS;

    return EOK;
}

result_t parse(parser_t * parser)
{
    result_t result = EOK;

    /********MISSING: incializacia pola 3 adres. kodu*/

    if((result = program(parser)) == EOK) {

    }

    /******************MISSING: volanie interpreta*********************/

    listDispose(&parser->varList);
    listDispose(&parser->paramList);
    freeHashTable(parser->table);
    /******************MISSING: zrusenie pola 3 adr. kodu**************/

    return result;
}

result_t program(parser_t *parser)
{
    result_t result = EOK;
    token_t token = nextToken(parser);

    if (TOKEN_IS(&token, EOF_TYPE)) {
        return ESYN;
    }

    if (token.result != EOK) {
        return token.result;
    }

    if (!TOKEN_HAS_TFLAG(&token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW )) {
        return ESYN;
    }

    if ((result = function(parser)) != EOK) {
        return result;
    }

    // TODO: Free token....
    token = nextToken(parser);
    if (token.result != EOK && token.result != EEOF) {
        return parser->token.result;
    }

    if(!TOKEN_IS(&token, EOF_TYPE)) {
        return ESYN;
    }

    return result;

    /****ak prvy parser->token nie je datovy typ --> error ****/

    /** ak je to datovy typ vola sa function ***/

    /*po analyze vsetkcyh funkci sa skontroluje ci posledna funkcia bola main---ak ano EOK ak nie error -------posledny by mal byt EOF **/
}

result_t function(parser_t *parser)
{
    result_t result = EOK;
    token_t token;

    unsigned int fType = parser->token.flags;

    token = nextToken(parser);
    if(parser->token.result != EOK)
        return parser->token.result;
    if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,MAIN_KW) && fType == INT_KW){
        /***********telo funkcie main**************/
        parser->fName = "main";

        parser->token = nextToken();
        if(parser->token.result != EOK)
            return parser->token.result;

        if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL)) // (
            return ESYN;

        parser->token = nextToken();
        if(parser->token.result != EOK)
            return parser->token.result;

        if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))	// )
            return ESYN;

        tItemPtr item;
        if((item = malloc(sizeof(struct tItem))) == NULL)
            return ESYS;
        if((item->data = malloc(sizeof(struct Data))) == NULL)
            return ESYS;

        item->functionId = parser->fName;
        insertLast(item,&parser->paramList);

        parser->token = nextToken();
        if(parser->token.result != EOK)
            return parser->token.result;

        if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_VINCULUM_SMBL))// {
            return ESYN;

        /********MISSING: vlozenie 3AK -- label zaciatku funkcie main********/
        printf("LABEL MAIN\n");

        parser->token = nextToken();
        if(parser->token.result != EOK)
            return parser->token.result;

        if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_VINCULUM_SMBL))		//ak nie je funkcia prazdna
            result = body(parser);

        if(result != EOK)
            return result;

        return EOK;				/// po main uz ziadne dalsie rekurzivne volanie ---- main je posledna funkcia v programe
    }
    else if(TOKEN_IS(parser->token,ID_TYPE)){
        /*****************telo funkcie*************/
        parser->fName = parser->token.data.sVal;
        char *fLabel = parser->token.data.sVal;
        parser->fDeclared = false;
        /********MISSSING: kontrola ts**********************/

        hTabItem * tableItem;
        if( (tableItem = searchItem(parser->table, parser->fName)) != NULL ){
            parser->fDeclared = true;
        }
        else{
            tableItem=createNewItem();
            tableItem->name = parser->fName;
            tableItem->dataType = fType;
            tableItem->isDefined = false;
            insertHashTable(parser->table, tableItem);
        }
        /***ak uz je v tabulke nastavy sa fDeclared na true a pokracuje dalej*/
        /**ak nie je v TS tak sa do nej vlozi nazov funkcie a jej typ*/

        tItemPtr item;
        if(parser->fDeclared){
            item=parser->paramList.First;
            while(item != NULL){
                if(!strcmp(item->functionId, parser->fName))
                    break;
                item=item->next;
            }
            if(item == NULL)
                return ESYS;

        }
        else if(!parser->fDeclared){
            if((item = malloc(sizeof(struct tItem))) == NULL)
                return ESYS;

            if((item->data = malloc(sizeof(struct Data))) == NULL)
                return ESYS;
            item->functionId = parser->fName;
        }

        parser->token = nextToken();
        if(parser->token.result != EOK)
            return parser->token.result;

        if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
            return ESYN;

        parser->token = nextToken();
        if(parser->token.result != EOK)
            return parser->token.result;

        if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))
            result=args(item,parser);    //parsovanie argumentov funkcie

        if(result != EOK)
            return result;

        if( (tableItem = searchItem(parser->table, parser->fName)) == NULL )
            return ESYS;

        if(parser->fDeclared){
            if(tableItem->params != parser->argsCounter)
                return ESEM;
        }else if(!parser->fDeclared){
            tableItem->params = parser->argsCounter;
            insertLast(item,&parser->paramList);
        }

        parser->argsCounter = 0;
        //uz sa nekontroluje parser->token ")" pretoze funkcia args sa vrati vzdy uspesne ukoncena tymto parser->tokenom
        parser->token=nextToken();
        if(parser->token.result != EOK)
            return parser->token.result;

        if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_VINCULUM_SMBL) && tableItem->isDefined==false)
        {
            tableItem->isDefined=true;
            /********MISSING: vlozenie 3AK -- label zaciatku funkcie*******/
            printf("LABEL F %s\n", fLabel);
            parser->token=nextToken();

            if(parser->token.result != EOK)
                return parser->token.result;

            if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_VINCULUM_SMBL))		//ak nie je funkcia prazdna
                result = body(parser);

            if(result != EOK)
                return result;

            /********MISSING: vlozenie 3AK -- navrat z funkcie*******/
            printf("LABEL ENDF %s\n", fLabel );
        }
        else if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_VINCULUM_SMBL) && tableItem->isDefined==true)
            return ESEM;
        else if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
            result=EOK;
        else
            return ESYN;

        parser->token=nextToken();
        if(parser->token.result != EOK)	//lexikalna alebo systemova chyba
            return parser->token.result;


        if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,INT_KW|DOUBLE_KW|STRING_KW))	///mala by nasledovat dalsia funkcia
            result = function(parser);		//rekurzivne volanie pre spracovanie dalsej funkcie
        else if(TOKEN_IS(parser->token,EOF_TYPE))
            return ESEM;
        else
            return ESYN;	//ak nie je ziadna dalsia funkcia je to chyba
    }else if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,MAIN_KW) && fType != INT_KW)
        return ESEM;
    else result = ESYN;

    return result;
}
