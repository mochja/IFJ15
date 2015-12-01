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

#include <stdio.h>
#include <stdbool.h>
#include "scaner.h"
#include "parser.h"
#include "list.h"
#include <malloc.h>
#include <string.h>
#include "hash.h"

tresult result=EOK;
tvarList varList;
tvarList paramList;
char * fName;
int argsCounter = 0;
int argsCounter1 = 0;
bool fDeclared = false;
int hInt = 0;
int label = 1;
char buffer[20];
char * assignVarName=NULL;
tTable *table = NULL;

char * generateName(int number){

    sprintf(buffer, "$%d" , number);
    return buffer;
}

tresult parse(){
/**incializacia  ---- overenie prazdnosti zdrojaku */

    if((table = initHashTable(MAX_HTSIZE)) == NULL)
            return ESYS;

    /********MISSING: incializacia pola 3 adres. kodu*/
    listInit(&varList);
    listInit(&paramList);

    result = EOK;
    token = nextToken();
    if(token.result != EOK)
        return token.result;


    if(token.type == END_OF_FILE)
        return ESYN;
    else if(token.type == LEX_ERROR)
        return ELEX;
    else if(token.type == SYS_ERROR)
        return ESYS;
    else
        result = program();



    if(result == EOK);
    /******************MISSING: volanie interpreta*********************/


    listDispose(&varList);
    listDispose(&paramList);
    freeHashTable(table);
    /******************MISSING: zrusenie pola 3 adr. kodu**************/

    return result;

}


tresult program(){
/**overim prvy token**/

    if(token.type != INT && token.type != DOUBLE && token.type != STRING)
        return ESYN;

    result = function();

    if(result != EOK)
        return result;

    token = nextToken();
    if(token.result != EOK && token.result!= EEOF)
        return token.result;

    if(token.type != END_OF_FILE)
        return ESYN;

    return result;

/****ak prvy token nie je datovy typ --> error ****/

/**ak je to datovy typ vola sa function ***/


/*po analyze vsetkcyh funkci sa skontroluje ci posledna funkcia bola main---ak ano EOK ak nie error -------posledny by mal byt EOF **/

}


tresult function(){


    int fType = token.type;

    token = nextToken();
    if(token.result != EOK)
        return token.result;
    if(token.type == MAIN && fType == INT){
        /***********telo funkcie main**************/
            fName = "main";

            token = nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != LEFT_CULUM) // (
                return ESYN;

            token = nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != RIGHT_CULUM)   // )
                return ESYN;

            tItemPtr item;
            if((item = malloc(sizeof(struct tItem))) == NULL)
                return ESYS;
            if((item->data = malloc(sizeof(struct Data))) == NULL)
                return ESYS;

            item->functionId = fName;
            insertLast(item,&paramList);

            token = nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != LEFT_VINCULUM) // {
                return ESYN;

            /********MISSING: vlozenie 3AK -- label zaciatku funkcie main********/
            printf("LABEL MAIN\n");

            token = nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != RIGHT_VINCULUM)        //ak nie je funkcia prazdna
                result = body();

            if(result != EOK)
                return result;

            return EOK;             /// po main uz ziadne dalsie rekurzivne volanie ---- main je posledna funkcia v programe
    }
    else if(token.type = ID){
        /*****************telo funkcie*************/
            fName = token.data.s;
            char *fLabel = token.data.s;
            fDeclared = false;
                /********MISSSING: kontrola ts**********************/

                hTabItem * tableItem;
                if( (tableItem = searchItem(table, fName)) != NULL ){
                    fDeclared = true;
                }
                else{
                    tableItem=createNewItem();
                    tableItem->name = fName;
                    tableItem->dataType = fType;
                    tableItem->isDefined = false;
                    insertHashTable(table, tableItem);
                }
                /***ak uz je v tabulke nastavy sa fDeclared na true a pokracuje dalej*/
                /**ak nie je v TS tak sa do nej vlozi nazov funkcie a jej typ*/

            tItemPtr item;
            if(fDeclared){
                item=paramList.First;
                while(item != NULL){
                    if(!strcmp(item->functionId, fName))
                        break;
                    item=item->next;
                }
                if(item == NULL)
                    return ESYS;

            }
            else if(!fDeclared){
                if((item = malloc(sizeof(struct tItem))) == NULL)
                    return ESYS;

                if((item->data = malloc(sizeof(struct Data))) == NULL)
                    return ESYS;
                item->functionId = fName;
            }

            token = nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != LEFT_CULUM)
                return ESYN;

            token = nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != RIGHT_CULUM)
                result=args(item);    //parsovanie argumentov funkcie

            if(result != EOK)
                return result;

            if( (tableItem = searchItem(table, fName)) == NULL )
                return ESYS;

            if(fDeclared){
                if(tableItem->params != argsCounter)
                    return ESEM;
            }else if(!fDeclared){
                tableItem->params = argsCounter;
                insertLast(item,&paramList);
            }

            argsCounter = 0;
                                //uz sa nekontroluje token ")" pretoze funkcia args sa vrati vzdy uspesne ukoncena tymto tokenom
            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == LEFT_VINCULUM && tableItem->isDefined==false)
            {
                tableItem->isDefined=true;
                /********MISSING: vlozenie 3AK -- label zaciatku funkcie*******/
                printf("LABEL F %s\n", fLabel);
                token=nextToken();

                if(token.result != EOK)
                    return token.result;

                if(token.type != RIGHT_VINCULUM)        //ak nie je funkcia prazdna
                    result = body();

                if(result != EOK)
                    return result;

                /********MISSING: vlozenie 3AK -- navrat z funkcie*******/
                printf("LABEL ENDF %s\n", fLabel );
            }
            else if(token.type == LEFT_VINCULUM && tableItem->isDefined==true)
                return ESEM;
            else if(token.type == SEMICOLON)
                result=EOK;
            else
                return ESYN;

            token=nextToken();
            if(token.result != EOK) //lexikalna alebo systemova chyba
                return token.result;

            if(token.type == INT || token.type == DOUBLE || token.type == STRING)   ///mala by nasledovat dalsia funkcia
                result = function();        //rekurzivne volanie pre spracovanie dalsej funkcie
            else if(token.type == END_OF_FILE)
                return ESEM;
            else
                return ESYN;    //ak nie je ziadna dalsia funkcia je to chyba
    }else if(token.type == MAIN && fType != INT)
        return ESEM;
    else result = ESYN;

return result;
}


tresult body(){
/******telo funkcie********/
    if(result != EOK)
        return result;

    tItemPtr varBlock;
        if((varBlock = malloc(sizeof(struct tItem))) == NULL)
            return ESYS;

        if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
            return ESYS;

    result=declaration(varBlock);

    if(result != EOK)
        return result;

    insertLast(varBlock,&varList);
    argsCounter = 0;

    result=list();

    return result;
}

/**********************prikazy v jednom bloku************************************/
tresult list(){


        /*******************ID <priradenie> ; <list>************************/
        if(token.type == ID){
            char * hName=NULL;
            if((hName = varSearch(&varList,token.data.s)) == NULL){
                if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                    return ESEM;
            }
            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != ASSIGN)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            assignVarName=hName;
            result=assign();

            if(result != EOK)
                return result;

            printf("\tMV %s VYRAZ\n", hName);
            /*vlozenie 3AK na priradenie vysledku z funkcie assign do hName*/
        }

        /***************** { <deklaracia> <list> ****************************/
        else if(token.type == LEFT_VINCULUM){
            tItemPtr varBlock;
                if((varBlock = malloc(sizeof(struct tItem))) == NULL)
                    return ESYS;

                if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
                    return ESYS;
            token=nextToken();

            if(token.result != EOK)
                return token.result;

            result=declaration(varBlock);
            argsCounter = 0;

            if(result != EOK)
                return result;
            insertLast(varBlock,&varList);
            printf(".....NEW BLOCK.....\n");
            result=list();

            if(result != EOK)
                return result;
        }

        /*************** } <list> ***********************/
        else if(token.type == RIGHT_VINCULUM){
            printf("....END BLOCK....\n");
            deleteLast(&varList);
                return EOK;
        }
        /*********** if ( <EXP> ) { <list> } else { <list> }*******************/
        else if(token.type == IF){

            int elseLabel = label;
            int endLabel = ++label;
            label++;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != LEFT_CULUM)
                return ESYN;


            /*********hack vyrazu ******/
            token=nextToken();
            if(token.result != EOK)                 ///mala by sa volat precedencna analyza
                return token.result;
            while(token.type != RIGHT_CULUM){
                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }
            /********hack vyrazu********/


            /*vyhodnotenie vyrazu*/

            printf("EXP\n");
            printf("JMPF %d\n", elseLabel);
            /**vlozenie 3AK - podmieneny skok ak podmienka nebola splnena na elseLabel**/

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != LEFT_VINCULUM)
                return ESYN;
            printf("....NEW BLOCK....\n");
            tItemPtr varBlock;
                if((varBlock = malloc(sizeof(struct tItem))) == NULL)
                    return ESYS;

                if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
                    return ESYS;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            result=declaration(varBlock);
            argsCounter = 0;
            if(result != EOK)
                return result;

            insertLast(varBlock,&varList);
            result=list();

            if(result != EOK)
                return result;
            printf("JMP %d\n", endLabel);
            /*vlozenie 3AK - skok na endLabel*/

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != ELSE)
                return ESYN;
            /******vlozenie 3AK - elseLabel*******/
            printf("LABEL ELSE %d\n", elseLabel );

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != LEFT_VINCULUM)
                return ESYN;
            printf("....NEW BLOCK....\n");
            tItemPtr varBlock2;
                if((varBlock2 = malloc(sizeof(struct tItem))) == NULL)
                    return ESYS;

                if((varBlock2->data = malloc(sizeof(struct Data))) == NULL)
                    return ESYS;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            result=declaration(varBlock2);
            argsCounter = 0;
            if(result != EOK)
                return result;

            insertLast(varBlock2,&varList);
            result=list();

            if(result != EOK)
                return result;
            /**vlozenie 3AK - endLabel***/
            printf("LABEL END %d\n",  endLabel);
        }

        /** for ( <deklaracia> ; <vyraz> ; priradenie) <list>**/
        else if(token.type == FOR){
            int expLabel = label;
            int assignLabel = ++label;
            int beginLabel = ++label;
            int endLabel = ++label;
            label++;
            char * hName = NULL;
            char * hNameID = NULL;
            hTabItem * tItem;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != LEFT_CULUM)
                return ESYN;

            /**deklaracia riadiacej premennej**/
            token=nextToken();

            if(token.result != EOK)
                return token.result;

            if(token.type != INT && token.type != DOUBLE && token.type != STRING && token.type != AUTO)
                return ESYN;

            int varType = token.type;

            token=nextToken();

            if(token.result != EOK)
                return token.result;

            if(token.type != ID)
                return ESYN;

            tItemPtr varBlock;
                if((varBlock = malloc(sizeof(struct tItem))) == NULL)
                    return ESYS;

                if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
                    return ESYS;

            hName=generateName(hInt);
            hInt++;
            varBlock->data[0].id=token.data.s;
            varBlock->data[0].hid=malloc(strlen(hName) + 1);
            strcpy(varBlock->data[0].hid,hName);
            insertLast(varBlock,&varList);

            /*vlozenie riadiacje premennej do TS*/
            tItem=createNewItem();
            tItem->name=malloc(strlen(hName) + 1);
            strcpy(tItem->name,hName);
            tItem->dataType=varType;
            insertHashTable(table,tItem);

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == ASSIGN){
                token=nextToken();
                if(token.result != EOK)
                    return token.result;

            /*********hack vyrazu ******/
            token=nextToken();
            if(token.result != EOK)                 ///mala by sa volat precedencna analyza
                return token.result;
            while(token.type != SEMICOLON){
                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }
            /********hack vyrazu********/

            /**vlozenie hodnoty riadiacej premennej podla vysledku assign()**/
            /*******3AK, MV , #1, NULL, hName******/
            }else if(token.type != SEMICOLON){
                return ESYN;
            }


            /**vlozenie 3AK  - label expLabel**/
            printf("EXP LABEL %d\n", expLabel );
            printf("EXP\n" );


            /*****hack*********/
            token=nextToken();
            if(token.result != EOK)
                return token.result;
            while(token.type != SEMICOLON){             ///mala by sa volat precedencna analyza
                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }


            /**vyhodnotenie vyrazu**/
            printf("JMPT BEGIN %d\n", beginLabel );
            printf("JMPF END %d\n",  endLabel);
            /**vlozenie 3AK - skok na beginLabel ak je vyraz true**/
            /**vlozenie 3AK - skok na endLabel ak je vyraz false**/

            printf("LABEL ASSIGNLABEL %d\n", assignLabel);
            /**vlozenie 3AK - label assignLabel**/
            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != ID)
                return ESYN;
            if((hNameID = varSearch(&varList,token.data.s)) == NULL){
                if((hNameID = paramSearch(&paramList, fName, token.data.s)) == NULL)
                return ESEM;
            }
            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != ASSIGN)
                return ESYN;
            token=nextToken();
            if(token.result != EOK)
                return token.result;

            /*****hack*********/
            while(token.type != RIGHT_CULUM){               ///mala by sa volat precedencna analyza
                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }



            printf("ASSING EXP\n");
            /**vyhodnotenie vyrazu**/
            printf("MV %s EXPRESULT\n",hNameID);
            /**vlozenie 3AK priradnie do premennej hNameID**/
            printf("JMP EXPLABEL %d\n", expLabel);
            /**vlozenie 3AK - skok na label expLabel**/

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != LEFT_VINCULUM)
                return ESYN;

            tItemPtr varBlock2;
                if((varBlock2 = malloc(sizeof(struct tItem))) == NULL)
                    return ESYS;

                if((varBlock2->data = malloc(sizeof(struct Data))) == NULL)
                    return ESYS;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            result=declaration(varBlock2);
            argsCounter = 0;
            if(result != EOK)
                return result;

            insertLast(varBlock2,&varList);

            /**vlozenie 3AK - label beginLabel**/
            printf("LABEL BEGIN %d \n", beginLabel );
            result=list();

            if(result != EOK)
                return result;
            deleteLast(&varList);
            printf("JMP ASSIGNLABEL %d\n" , assignLabel);
            /**vlozenie 3AK - skok na assignLabel**/
            printf("LABEL END %d\n", endLabel);
            /**vlozenie 3AK - label endLabel**/

        }

        /**cin |>> ID|*n ;**/
        else if(token.type == CIN){
        char * hName = NULL;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != DBL_ARR_RIGHT)
                return ESYN;

            while(token.type == DBL_ARR_RIGHT){
                token=nextToken();
                if(token.result != EOK)
                    return token.result;
                if(token.type != ID)
                    return ESEM4;
                if((hName = varSearch(&varList,token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                /**vygenerovanie 3AK - nacitanie zo SV do premennej hName**/
                printf("\tCIN STDIN %s\n", hName);

                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }

            if(token.type != SEMICOLON)
                return ESYN;
        }

        /**cout |<< term |*n  ---- term==ID || TEXT || INT || DOUBLE**/
        else if(token.type == COUT){
            char * hName=NULL;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != DBL_ARR_LEFT)
                return ESYN;

            while(token.type == DBL_ARR_LEFT){
                token=nextToken();
                if(token.result != EOK)
                    return token.result;

                if(token.type == ID){
                    if((hName = varSearch(&varList,token.data.s)) == NULL){
                        if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                            return ESEM;
                    }
                    /**vlozenie 3AK - vypis na STDOUT z premennej hName**/
                    printf("\tCOUNT STDOUT %s\n",hName);
                }
                else if(token.type == INT_NUM)
                    {printf("\tCOUNT STDOUT %d\n",token.data.i);}/**vlozenie 3AK - vypis na STDOUT cislo token.data.i**/
                else if(token.type == DOUBLE_NUM)
                    {printf("\tCOUNT STDOUT %f\n",token.data.d);}/**vlozenie 3AK - vypis na STDOUT cislo token.data.d**/
                else if(token.type == TEXT)
                    {printf("\tCOUNT STDOUT %s\n",token.data.s);}/**vlozenie 3AK - vypis na STDOUT retazec token.data.s**/
                else return ESEM4;


                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }
            if(token.type != SEMICOLON)
                return ESYN;
        }

        /**return <EXP> ;**/
        else if(token.type == RETURN){

            /*********hack**********/
            token=nextToken();
            if(token.result != EOK)
                return token.result;                    ///precedencna
            while(token.type != SEMICOLON){
                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }
            /*********************/

            printf("RETURN\n");
            /****vyhdontenie vyrazu***/
            /**vloznenie 3AK - skos s5**/
        }
        else if(token.type == INT || token.type == DOUBLE || token.type == STRING || token.type == AUTO ){
            /***deklaracia mimo zaciatku bloku**/
            result=advDeclaration();
            if(result != EOK)
                return result;
        }
        else return ESYN;

        token=nextToken();
        if(token.result != EOK)
            return token.result;
        result=list();
    return result;
}

tresult advDeclaration(){
    int varType = token.type;
    char * hName;
    hTabItem * tItem;

    token=nextToken();
    if(token.result != EOK)
        return token.result;

    if(token.type != ID)
        return ESYN;

    int i = 0;
    while(varList.Last->data[i].id != NULL){
        if(!strcmp(token.data.s,varList.Last->data[i].id))
            return ESEM;
        i++;
    }

    if((hName = paramSearch(&paramList, fName, token.data.s)) != NULL)
        return ESEM;

    hName=generateName(hInt);
    hInt++;

    if((varList.Last->data = realloc(varList.Last->data, sizeof(struct Data) * (i+1) )) == NULL)
        return ESYS;
    varList.Last->data[i].id=token.data.s;
    varList.Last->data[i].hid=malloc(strlen(hName) + 1);
    strcpy(varList.Last->data[i].hid,hName);

    /***vytvorenie novej polozky do TS****/
    tItem=createNewItem();
    tItem->name=malloc(strlen(hName) + 1);
    strcpy(tItem->name,hName);
    tItem->dataType=varType;
    insertHashTable(table,tItem);

    if(varType == AUTO){
        token = nextToken();

        if(token.result != EOK)
            return token.result;

        if(token.type != ASSIGN)
            return ESYN;

        token = nextToken();
        if(token.result != EOK)
            return token.result;
        assignVarName=hName;
        result=assign();

        if(result != EOK)
            return result;
        /*******3AK , MV , #1 ,NULL, hName******/

    }else{
        token=nextToken();

        if(token.result != EOK)
            return token.result;

        if(token.type == SEMICOLON){
            return EOK;
        }else if(token.type == ASSIGN){
                token = nextToken();
                if(token.result != EOK)
                    return token.result;
                assignVarName=hName;
                result=assign();

                if(result != EOK)
                    return result;

                return EOK;

        /*******3AK , MV , #1 ,NULL, hName******/
        }else return ESYN;
    }

}

tresult declaration(tItemPtr varBlock){
    /**deklaracia premennych ----- vola sa na zacitku kazdeho bloku ****/
    int varType = token.type;
    char * hName;
    hTabItem * tItem;

    if(token.type != INT && token.type != DOUBLE && token.type != STRING && token.type != AUTO)
        return EOK;

    token=nextToken();

    if(token.result != EOK)
        return token.result;

    if(token.type != ID)
        return ESYN;

    /***vlozenie do bloku***/

    int i = 0;
    while(varBlock->data[i].id != NULL){
        if(!strcmp(token.data.s, varBlock->data[i].id))
            return ESEM;
        i++;
    }

    if((hName = paramSearch(&paramList, fName, token.data.s)) != NULL)
        return ESEM;

    hName=generateName(hInt);
    hInt++;


    if((varBlock->data = realloc(varBlock->data, sizeof(struct Data) * (argsCounter+1) )) == NULL)
        return ESYS;
    varBlock->data[argsCounter].id=token.data.s;
    varBlock->data[argsCounter].hid=malloc(strlen(hName) + 1);
    strcpy(varBlock->data[argsCounter].hid,hName);
    argsCounter=argsCounter+1;

    /***vytvorenie novej polozky do TS****/
    tItem=createNewItem();
    tItem->name=malloc(strlen(hName) + 1);
    strcpy(tItem->name,hName);
    tItem->dataType=varType;
    insertHashTable(table,tItem);

    if(varType == AUTO){

        token = nextToken();

        if(token.result != EOK)
            return token.result;

        if(token.type != ASSIGN)
            return ESYN;

        token = nextToken();
        if(token.result != EOK)
            return token.result;
        assignVarName=hName;
        result=assign();

        if(result != EOK)
            return result;

        token=nextToken();
        if(token.result != EOK)
            return token.result;
        result = declaration(varBlock);

        /*******3AK , MV , #1 ,NULL, hName******/

    }else{ //typ int, double alebo string

        token=nextToken();

        if(token.result != EOK)
            return token.result;

        if(token.type == SEMICOLON){
            token = nextToken();

            if(token.result != EOK)
                return token.result;
            result = declaration(varBlock);
        }
        else if(token.type == ASSIGN){

            token = nextToken();
            if(token.result != EOK)
                return token.result;

            assignVarName=hName;

            result=assign();

            if(result != EOK)
                return result;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            result = declaration(varBlock);

        }else return ESYN;
    }
}

tresult assign(){
    if(token.type == LENGTH || token.type == SUBSTR || token.type == CONCAT || token.type == FIND || token.type == SORT){
        result=buildInF();
    }
    else if(token.type == ID){
    hTabItem *tableItem;
        if((tableItem=searchItem(table,token.data.s)) == NULL){
            /*********hack**********/               ///precedencna
            while(token.type != SEMICOLON){
                token=nextToken();
                if(token.result != EOK)
                    return token.result;
            }
            /*********************/
        }else{
            /**volanie uzivatelskej funkcie**/
        tItemPtr item;
        item=paramList.First;
            while(item != NULL){
                if(!strcmp(item->functionId, token.data.s))
                    break;
                item=item->next;
            }
            if(item == NULL)
                return ESYS;
        hTabItem * tItem;


        if((tItem = searchItem(table,assignVarName)) == NULL)
            return ESYS;
        if(tItem->dataType != tableItem->dataType && tItem->dataType!=AUTO)
            return ESEM2;

        token=nextToken();
        if(token.result != EOK)
            return token.result;
        if(token.type != LEFT_CULUM)
            return ESYN;

        token=nextToken();
        if(token.result != EOK)
            return token.result;

        if(token.type != RIGHT_CULUM)
            result=params(item);
        if(result != EOK)
            return result;

        if(tableItem->params != argsCounter1)
            return ESEM3;
        argsCounter1=0;


        token=nextToken();
        if(token.result != EOK)
            return token.result;
        if(token.type != SEMICOLON)
            return ESYN;

        /***********3AK , JMP, LABEL, PARAMETRE, NULL***********/
        }
    }else if(token.type != SEMICOLON){

        /*********hack**********/               ///precedencna
        while(token.type != SEMICOLON){
            token=nextToken();
            if(token.result != EOK)
                return token.result;
        }
        /*********************/
    }else return ESEM;
return EOK;
}

tresult buildInF(){

    char * hName;
    hTabItem * tItem;
    hTabItem * tItem1;
    hTabItem * tItem2;
    hTabItem * tItem3;

    switch(token.type)
    {
        case LENGTH:
            /******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
            if((tItem = searchItem(table,assignVarName)) == NULL)
                return ESYS;
            if(tItem->dataType != INT && tItem->dataType != AUTO)
                return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != LEFT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == TEXT){
                hName = generateName(hInt);
                hInt++;
                tItem1=createNewItem();
                tItem1->name=malloc(strlen(hName) + 1);
                strcpy(tItem1->name,hName);
                tItem1->dataType=STRING;
                tItem1->value.c = token.data.s;
                insertHashTable(table,tItem1);
            }else if(token.type == ID){
                /**********vyhladame polozku hName v TS*********/
                /****zkontroluje datovy typ --- musi byt string***/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem1 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem1->dataType != STRING && tItem2->dataType!=AUTO)
                    return ESEM2;
            }
            else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != RIGHT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type != SEMICOLON)
                return ESYN;

            //printf("volanie funkcie length --- parameter: %s\n",  tItem1->value.c );
            /*************3AK -- LENGTH , pointer na polozku v ts, NULL ,NULL,***************/
            /********3AK --- MV , searchTS(#1), NULL, searchTS(assignVarName)******/


        break;

        case SUBSTR:
            /******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
            if((tItem = searchItem(table,assignVarName)) == NULL)
                return ESYS;
            if(tItem->dataType != STRING && tItem->dataType != AUTO)
                return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != LEFT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == TEXT){
                hName = generateName(hInt);
                hInt++;
                tItem1=createNewItem();
                tItem1->name=malloc(strlen(hName) + 1);
                strcpy(tItem1->name,hName);
                tItem1->dataType=STRING;
                tItem1->value.c = token.data.s;
                insertHashTable(table,tItem1);
            }else if(token.type == ID){
                /**********vyhladame polozku hName v TS*********/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem1 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem1->dataType != STRING && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != COMMA)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == INT_NUM){
                hName = generateName(hInt);
                hInt++;
                tItem2=createNewItem();
                tItem2->name=malloc(strlen(hName) + 1);
                strcpy(tItem2->name,hName);
                tItem2->dataType=INT;
                tItem2->value.i = token.data.i;
                insertHashTable(table,tItem2);
            }else if(token.type == ID){
            /**********vyhladame polozku hName v TS*********/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }

                if((tItem2 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem2->dataType != INT && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != COMMA)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;


            if(token.type == INT_NUM){
                hName = generateName(hInt);
                hInt++;
                tItem3=createNewItem();
                tItem3->name=malloc(strlen(hName) + 1);
                strcpy(tItem3->name,hName);
                tItem3->dataType=INT;
                tItem3->value.i = token.data.i;
                insertHashTable(table,tItem3);
            }else if(token.type == ID){
            /**********vyhladame polozku hName v TS*********/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem2 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem2->dataType != INT && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != RIGHT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != SEMICOLON)
                return ESYN;


            //printf("volanie funkcie substr--- parameter 1: %s ,parameter 2: %d, parameter 3: %d\n",  tItem1->value.c, tItem2->value.i, tItem3->value.i );
            /***********************3AK -- SUBSTR , p1 , p2 , p3******************************/
            /***********************3AK -- MV , searchTS(#1),NULL,searchTS(assignVarName)******/
        break;
        case CONCAT:

        /******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
            if((tItem = searchItem(table,assignVarName)) == NULL)
                return ESYS;
            if(tItem->dataType != STRING && tItem->dataType != AUTO)
                return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != LEFT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == TEXT){
                hName = generateName(hInt);
                hInt++;
                tItem1=createNewItem();
                tItem1->name=malloc(strlen(hName) + 1);
                strcpy(tItem1->name,hName);
                tItem1->dataType=STRING;
                tItem1->value.c = token.data.s;
                insertHashTable(table,tItem1);
            }else if(token.type == ID){
            /**********vyhladame polozku hName v TS*********/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem1 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem1->dataType != STRING && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != COMMA)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == TEXT){
            /**********vyhladame polozku hName v TS*********/
                hName = generateName(hInt);
                hInt++;
                tItem2=createNewItem();
                tItem2->name=malloc(strlen(hName) + 1);
                strcpy(tItem2->name,hName);
                tItem2->dataType=STRING;
                tItem2->value.c = token.data.s;
                insertHashTable(table,tItem2);
            }else if(token.type == ID){
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem2 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem2->dataType != STRING && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != RIGHT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != SEMICOLON)
                return ESYN;

            //printf("volanie funkcie concat--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);

        break;
        case FIND:
        /******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
            if((tItem = searchItem(table,assignVarName)) == NULL)
                return ESYS;
            if(tItem->dataType != INT && tItem->dataType != AUTO)
                return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != LEFT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == TEXT){
                hName = generateName(hInt);
                hInt++;
                tItem1=createNewItem();
                tItem1->name=malloc(strlen(hName) + 1);
                strcpy(tItem1->name,hName);
                tItem1->dataType=STRING;
                tItem1->value.c = token.data.s;
                insertHashTable(table,tItem1);
            }else if(token.type == ID){
                /**********vyhladame polozku hName v TS*********/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem1 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem1->dataType != STRING && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != COMMA)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == TEXT){
                hName = generateName(hInt);
                hInt++;
                tItem2=createNewItem();
                tItem2->name=malloc(strlen(hName) + 1);
                strcpy(tItem2->name,hName);
                tItem2->dataType=STRING;
                tItem2->value.c = token.data.s;
                insertHashTable(table,tItem2);
            }else if(token.type == ID){
                /**********vyhladame polozku hName v TS*********/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem2 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem2->dataType != STRING && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != RIGHT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != SEMICOLON)
                return ESYN;

    //  printf("volanie funkcie find--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);
        break;
        case SORT:
        /******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
            if((tItem = searchItem(table,assignVarName)) == NULL)
                return ESYS;
            if(tItem->dataType != STRING && tItem->dataType != AUTO)
                return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != LEFT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;

            if(token.type == TEXT){
                hName = generateName(hInt);
                hInt++;
                tItem1=createNewItem();
                tItem1->name=malloc(strlen(hName) + 1);
                strcpy(tItem1->name,hName);
                tItem1->dataType=STRING;
                tItem1->value.c = token.data.s;
                insertHashTable(table,tItem1);
            }else if(token.type == ID){
            /**********vyhladame polozku hName v TS*********/
                if((hName = varSearch(&varList, token.data.s)) == NULL){
                    if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                        return ESEM;
                }
                if((tItem1 = searchItem(table,hName)) == NULL)
                    return ESYS;
                if(tItem1->dataType != STRING && tItem2->dataType!=AUTO)
                    return ESEM2;
            }else return ESEM2;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != RIGHT_CULUM)
                return ESYN;

            token=nextToken();
            if(token.result != EOK)
                return token.result;
            if(token.type != SEMICOLON)
                return ESYN;

        break;
    }
};


tresult params(tItemPtr item){
char * hName;
hTabItem * tableItem;

    if(token.type == ID){
        hTabItem * tItem1;

        if((tableItem=searchItem(table,item->data[argsCounter1].hid)) == NULL)
            return ESYS;
        if((hName = varSearch(&varList,token.data.s)) == NULL){
            if((hName = paramSearch(&paramList, fName, token.data.s)) == NULL)
                return ESEM;
        }
        if((tItem1=searchItem(table,hName)) == NULL)
            return ESYS;

        if(tableItem->dataType != AUTO && tableItem->dataType != tItem1->dataType && tItem1->dataType!=AUTO)
            return ESEM2;
        printf("ID ide\n");

        /****************parameter tItem1*****************/
    }
    else if(token.type == INT_NUM){
        if((tableItem=searchItem(table,item->data[argsCounter1].hid)) == NULL)
            return ESYS;
        printf("sadsa\n");
        if(tableItem->dataType != INT && tableItem->dataType != AUTO)
            return ESEM2;

        hTabItem * tItem1 = createNewItem();
        hName=generateName(hInt);
        hInt++;
        tItem1->name=malloc(strlen(hName) + 1);
        strcpy(tItem1->name,hName);
        tItem1->dataType = INT;
        tItem1->value.i = token.data.i;
        insertHashTable(table,tItem1);

        /**************************************/

    }
    else if(token.type == DOUBLE_NUM){
        if((tableItem=searchItem(table,item->data[argsCounter1].hid)) == NULL)
            return ESYS;

        if(tableItem->dataType != DOUBLE && tableItem->dataType != AUTO)
            return ESEM2;

        hTabItem * tItem1 = createNewItem();
        hName=generateName(hInt);
        hInt++;
        tItem1->name=malloc(strlen(hName) + 1);
        strcpy(tItem1->name,hName);
        tItem1->dataType = DOUBLE;
        tItem1->value.d = token.data.d;
        insertHashTable(table,tItem1);

        /****************************************/

    }
    else if(token.type == TEXT){
        if((tableItem=searchItem(table,item->data[argsCounter1].hid)) == NULL)
            return ESYS;

        if(tableItem->dataType != STRING && tableItem->dataType != AUTO)
            return ESEM2;

        hTabItem * tItem1 = createNewItem();
        hName=generateName(hInt);
        hInt++;
        tItem1->name=malloc(strlen(hName) + 1);
        strcpy(tItem1->name,hName);
        tItem1->dataType = STRING;
        tItem1->value.c = token.data.s;
        insertHashTable(table,tItem1);

        /**************************************/
    }
    else return ESEM2;

    argsCounter1++;

    token=nextToken();
    if(token.result != EOK)
        return token.result;
    if(token.type == COMMA){
        token=nextToken();
        if(token.result != EOK)
            return token.result;
        result=params(item);
    }
    else if(token.type == RIGHT_CULUM){
        return EOK;
    }
    else return ESYN;
}

tresult args(tItemPtr item){
    /******spracovanie argumentov funkcii********/
        int varType = token.type;
        hTabItem * tItem;
        if(token.type != INT && token.type != DOUBLE && token.type != STRING)
            return ESYN;

        token=nextToken();
        if(token.result != EOK)
            return token.result;

        if(token.type != ID)
            return ESYN;

            if(fDeclared){
                char * hName = NULL;
                int i = 0;
                while(item->data[i].id != NULL){
                    if(!strcmp(token.data.s, item->data[i].id))
                    {
                        hName = item->data[i].hid;
                        break;
                    }
                    i++;
                }
                if(hName == NULL)
                    return ESEM;

                if((tItem=searchItem(table, hName)) == NULL){
                    return ESYS;
                }
                /****kontrola typu a poradia parametru****/
                if(tItem->dataType != varType)
                    return ESEM;
                if(tItem->paramPosition != argsCounter)
                    return ESEM;

            }
            else if(!fDeclared){
                char * hName = generateName(hInt);
                hInt++;
                int i = 0;

                while(item->data[i].id != NULL){
                    if(!strcmp(token.data.s, item->data[i].id))
                        return ESEM;
                    i++;
                }

                if((item->data = realloc(item->data, sizeof(struct Data) * (argsCounter+1) )) == NULL){
                    return ESYS;
                }
                item->data[argsCounter].id=token.data.s;
                item->data[argsCounter].hid=malloc(strlen(hName) + 1);
                strcpy(item->data[argsCounter].hid,hName);

                /*vlozi parameter do TS -nazov, typ, funkciu a poradie*/
                tItem=createNewItem();
                tItem->name=malloc(strlen(hName) + 1);
                strcpy(tItem->name,hName);
                tItem->dataType=varType;
                tItem->paramPosition=argsCounter;
                insertHashTable(table,tItem);
            }

        argsCounter++;

        /*ak je dalsi token , rekurzivne sa zavola funckia args a nacitava sa dalsi parameter*/
        /*ak je to zatvorka tak sa vracia EOK*/
        /*iny token znamena chybu*/
        token=nextToken();
        if(token.result != EOK)
            return token.result;

        if(token.type == RIGHT_CULUM){
            return EOK;
        }
        else
        if(token.type == COMMA){
            token=nextToken();
            if(token.result != EOK)
                return token.result;
            result = args(item);
        }
        else return ESYN;

}

int main(){

    source = fopen("src","r");
    //printf("sadsa\n");

    result = parse();
    printf("err code: %d" , result);
}
