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

tresult init_parser(t_Parser *parser){
	
	listInit(&parser->varList);
	listInit(&parser->paramList);
	parser->fName=NULL;
	parser->argsCounter = 0;
	parser->argsCounter1 = 0;
	parser->fDeclared = false;
	parser->hInt = 0;
	parser->label = 1;
	parser->assignVarName=NULL;

	if((parser->table = initHashTable(MAX_HTSIZE)) == NULL)
		return ESYS;

return EOK;
}

char * generateName(int number,t_Parser * parser){
	sprintf(parser->buffer, "$%d" , number);
	return parser->buffer;
}

tresult parse(t_Parser * parser){
/**incializacia  ---- overenie prazdnosti zdrojaku **/
 tresult result=EOK;
	

	/********MISSING: incializacia pola 3 adres. kodu*/

	parser->token = nextToken();
	if(TOKEN_IS(parser->token,EOF_TYPE))
		return ESYN;
	if(parser->token.result != EOK)
		return parser->token.result;

	result = program(parser);
	
	if(result == EOK);
	/******************MISSING: volanie interpreta*********************/


	listDispose(&parser->varList);
	listDispose(&parser->paramList);
	freeHashTable(parser->table);
	/******************MISSING: zrusenie pola 3 adr. kodu**************/

	return result;

}


tresult program(t_Parser * parser){
/**overim prvy parser->token**/
tresult result=EOK;

	if(!TOKEN_HAS_TFLAG(parser->token, KW_TYPE, INT_KW | DOUBLE_KW | STRING_KW ))
		return ESYN;

	result = function(parser);

	if(result != EOK)
		return result;

	parser->token = nextToken();
	if(parser->token.result != EOK && parser->token.result != EEOF)
		return parser->token.result;

	if(!TOKEN_IS(parser->token,EOF_TYPE))
		return ESYN;

	return result;

/****ak prvy parser->token nie je datovy typ --> error ****/

/**ak je to datovy typ vola sa function ***/


/*po analyze vsetkcyh funkci sa skontroluje ci posledna funkcia bola main---ak ano EOK ak nie error -------posledny by mal byt EOF **/

}


tresult function(t_Parser * parser){
tresult result=EOK;

	int fType = parser->token.flags;

	parser->token = nextToken();
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


tresult body(t_Parser * parser){
/******telo funkcie********/
tresult result=EOK;

	tItemPtr varBlock;
		if((varBlock = malloc(sizeof(struct tItem))) == NULL)
			return ESYS;

		if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
			return ESYS;

	result=declaration(varBlock,parser);

	if(result != EOK)
		return result;

	insertLast(varBlock,&parser->varList);
	parser->argsCounter = 0;

	result=list(parser);

	return result;
}

/**********************prikazy v jednom bloku************************************/
tresult list(t_Parser * parser){
tresult result=EOK;
	
		/*******************ID <priradenie> ; <list>************************/
		if(TOKEN_IS(parser->token,ID_TYPE)){
			char * hName=NULL;
			if((hName = varSearch(&parser->varList,parser->token.data.sVal)) == NULL){
				if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
					return ESEM;
			}
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,ASSIGN_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			parser->assignVarName=hName;
			result=assign(parser);

			if(result != EOK)
				return result;

			printf("\tMV %s VYRAZ\n", hName);
			/*vlozenie 3AK na priradenie vysledku z funkcie assign do hName*/
		}

		/***************** { <deklaracia> <list> ****************************/
		else if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_VINCULUM_SMBL)){
			tItemPtr varBlock;
				if((varBlock = malloc(sizeof(struct tItem))) == NULL)
					return ESYS;

				if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
					return ESYS;
			parser->token=nextToken();
			
			if(parser->token.result != EOK)
				return parser->token.result;

			result=declaration(varBlock,parser);
			parser->argsCounter = 0;

			if(result != EOK)
				return result;
			insertLast(varBlock,&parser->varList);
			printf(".....NEW BLOCK.....\n");
			result=list(parser);

			if(result != EOK)
				return result;
		}

		/*************** } <list> ***********************/
		else if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_VINCULUM_SMBL)){ 
			printf("....END BLOCK....\n");	
			deleteLast(&parser->varList);
				return EOK;	
		}
		/*********** if ( <EXP> ) { <list> } else { <list> }*******************/
		else if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,IF_KW)){

			int elseLabel = parser->label;
			int endLabel = ++parser->label;
			parser->label++;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
				return ESYN;
			

			/*********hack vyrazu ******/
			parser->token=nextToken();
			if(parser->token.result != EOK)					///mala by sa volat precedencna analyza
				return parser->token.result;
			while(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL)){
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}
			/********hack vyrazu********/


			/*vyhodnotenie vyrazu*/

			printf("EXP\n");
			printf("JMPF %d\n", elseLabel);
			/**vlozenie 3AK - podmieneny skok ak podmienka nebola splnena na elseLabel**/

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_VINCULUM_SMBL))
				return ESYN;
			printf("....NEW BLOCK....\n");
			tItemPtr varBlock;
				if((varBlock = malloc(sizeof(struct tItem))) == NULL)
					return ESYS;

				if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
					return ESYS;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			result=declaration(varBlock,parser);
			parser->argsCounter = 0;
			if(result != EOK)
				return result;

			insertLast(varBlock,&parser->varList);
			result=list(parser);

			if(result != EOK)
				return result;
			printf("JMP %d\n", endLabel);
			/*vlozenie 3AK - skok na endLabel*/

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,KW_TYPE,ELSE_KW))
				return ESYN;
			/******vlozenie 3AK - elseLabel*******/
			printf("LABEL ELSE %d\n", elseLabel );

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_VINCULUM_SMBL))
				return ESYN;
			printf("....NEW BLOCK....\n");
			tItemPtr varBlock2;
				if((varBlock2 = malloc(sizeof(struct tItem))) == NULL)
					return ESYS;

				if((varBlock2->data = malloc(sizeof(struct Data))) == NULL)
					return ESYS;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			result=declaration(varBlock2,parser);			
			parser->argsCounter = 0;
			if(result != EOK)
				return result;

			insertLast(varBlock2,&parser->varList);
			result=list(parser);

			if(result != EOK)
				return result;
			/**vlozenie 3AK - endLabel***/
			printf("LABEL END %d\n",  endLabel);
		}

		/** for ( <deklaracia> ; <vyraz> ; priradenie) <list>**/
		else if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,FOR_KW)){
			int expLabel = parser->label;
			int assignLabel = ++parser->label;
			int beginLabel = ++parser->label;
			int endLabel = ++parser->label;
			parser->label++;
			char * hName = NULL;
			char * hNameID = NULL;
			hTabItem * tItem;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
				return ESYN;

			/**deklaracia riadiacej premennej**/
			parser->token=nextToken();

			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,KW_TYPE,INT_KW|DOUBLE_KW|STRING_KW|AUTO_KW))
				return ESYN;

			int varType = parser->token.flags;

			parser->token=nextToken();

			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_IS(parser->token,ID_TYPE))
				return ESYN;
			
			tItemPtr varBlock;
				if((varBlock = malloc(sizeof(struct tItem))) == NULL)
					return ESYS;

				if((varBlock->data = malloc(sizeof(struct Data))) == NULL)
					return ESYS;

			hName=generateName(parser->hInt,parser);
			parser->hInt++;
			varBlock->data[0].id=parser->token.data.sVal;
			varBlock->data[0].hid=malloc(strlen(hName) + 1);
			strcpy(varBlock->data[0].hid,hName);
			insertLast(varBlock,&parser->varList);

			/*vlozenie riadiacje premennej do TS*/
			tItem=createNewItem();
			tItem->name=malloc(strlen(hName) + 1);
			strcpy(tItem->name,hName);
			tItem->dataType=varType;
			insertHashTable(parser->table,tItem);

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,ASSIGN_SMBL)){
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			/*********hack vyrazu ******/
			parser->token=nextToken();
			if(parser->token.result != EOK)					///mala by sa volat precedencna analyza
				return parser->token.result;
			while(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}
			/********hack vyrazu********/

			/**vlozenie hodnoty riadiacej premennej podla vysledku assign()**/
			/*******3AK, MV , #1, NULL, hName******/
			}else if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
				return ESYN;
			}


			/**vlozenie 3AK  - label expLabel**/
			printf("EXP LABEL %d\n", expLabel );
			printf("EXP\n" );


			/*****hack*********/
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			while(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){				///mala by sa volat precedencna analyza
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}


			/**vyhodnotenie vyrazu**/
			printf("JMPT BEGIN %d\n", beginLabel );
			printf("JMPF END %d\n",  endLabel);
			/**vlozenie 3AK - skok na beginLabel ak je vyraz true**/
			/**vlozenie 3AK - skok na endLabel ak je vyraz false**/

			printf("LABEL ASSIGNLABEL %d\n", assignLabel);
			/**vlozenie 3AK - label assignLabel**/
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_IS(parser->token,ID_TYPE))
				return ESYN;
			if((hNameID = varSearch(&parser->varList,parser->token.data.sVal)) == NULL){
				if((hNameID = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
				return ESEM;
			}
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,ASSIGN_SMBL))
				return ESYN;
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			
			/*****hack*********/
			while(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL)){				///mala by sa volat precedencna analyza
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}


			
			printf("ASSING EXP\n");
			/**vyhodnotenie vyrazu**/
			printf("MV %s EXPRESULT\n",hNameID);
			/**vlozenie 3AK priradnie do premennej hNameID**/
			printf("JMP EXPLABEL %d\n", expLabel);
			/**vlozenie 3AK - skok na label expLabel**/

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_VINCULUM_SMBL))
				return ESYN;

			tItemPtr varBlock2;
				if((varBlock2 = malloc(sizeof(struct tItem))) == NULL)
					return ESYS;

				if((varBlock2->data = malloc(sizeof(struct Data))) == NULL)
					return ESYS;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			result=declaration(varBlock2,parser);		
			parser->argsCounter = 0;
			if(result != EOK)
				return result;

			insertLast(varBlock2,&parser->varList);

			/**vlozenie 3AK - label beginLabel**/
			printf("LABEL BEGIN %d \n", beginLabel );
			result=list(parser);

			if(result != EOK)
				return result;
			deleteLast(&parser->varList);
			printf("JMP ASSIGNLABEL %d\n" , assignLabel);
			/**vlozenie 3AK - skok na assignLabel**/
			printf("LABEL END %d\n", endLabel);
			/**vlozenie 3AK - label endLabel**/

		}

		/**cin |>> ID|*n ;**/
		else if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,CIN_KW)){
		char * hName = NULL;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,DBL_ARR_RIGHT_SMBL))
				return ESYN;

			while(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,DBL_ARR_RIGHT_SMBL)){
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
				if(!TOKEN_IS(parser->token,ID_TYPE))
					return ESEM4;
				if((hName = varSearch(&parser->varList,parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				/**vygenerovanie 3AK - nacitanie zo SV do premennej hName**/
				printf("\tCIN STDIN %s\n", hName);

				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
				return ESYN;
		}

		/**cout |<< term |*n  ---- term==ID || TEXT || INT || DOUBLE**/
		else if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,COUT_KW)){
			char * hName=NULL;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,DBL_ARR_LEFT_SMBL))
				return ESYN;

			while(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,DBL_ARR_LEFT_SMBL)){
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
				
				if(TOKEN_IS(parser->token,ID_TYPE)){
					if((hName = varSearch(&parser->varList,parser->token.data.sVal)) == NULL){
						if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
							return ESEM;
					}
					/**vlozenie 3AK - vypis na STDOUT z premennej hName**/
					printf("\tCOUNT STDOUT %s\n",hName);
				}
				else if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,INT_CONST))
					{printf("\tCOUNT STDOUT %d\n",parser->token.data.iVal);}/**vlozenie 3AK - vypis na STDOUT cislo parser->token.data.i**/
				else if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,DOUBLE_CONST))
					{printf("\tCOUNT STDOUT %f\n",parser->token.data.dVal);}/**vlozenie 3AK - vypis na STDOUT cislo parser->token.data.d**/
				else if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST))
					{printf("\tCOUNT STDOUT %s\n",parser->token.data.sVal);}/**vlozenie 3AK - vypis na STDOUT retazec parser->token.data.s**/
				else return ESEM4;


				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
				return ESYN;
		}

		/**return <EXP> ;**/
		else if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,RETURN_KW)){

			/*********hack**********/
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;					///precedencna
			while(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}
			/*********************/

			printf("RETURN\n");
			/****vyhdontenie vyrazu***/
			/**vloznenie 3AK - skos s5**/
		}
		else if(TOKEN_HAS_TFLAG(parser->token,KW_TYPE,INT_KW|DOUBLE_KW|STRING_KW|AUTO_KW)){
			/***deklaracia mimo zaciatku bloku**/
			result=advDeclaration(parser);
			if(result != EOK)
				return result;
		}
		else return ESYN;

		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;
		result=list(parser);
	return result;
}

tresult advDeclaration(t_Parser * parser){
	tresult result=EOK;
	int varType = parser->token.flags;
	char * hName;
	hTabItem * tItem;

	parser->token=nextToken();
	if(parser->token.result != EOK)
		return parser->token.result;

	if(!TOKEN_IS(parser->token,ID_TYPE))
		return ESYN;

	int i = 0;
	while(parser->varList.Last->data[i].id != NULL){
		if(!strcmp(parser->token.data.sVal,parser->varList.Last->data[i].id))
			return ESEM;
		i++;
	}

	if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) != NULL)
		return ESEM;

	hName=generateName(parser->hInt,parser);
	parser->hInt++;

	if((parser->varList.Last->data = realloc(parser->varList.Last->data, sizeof(struct Data) * (i+1) )) == NULL)
		return ESYS;
	parser->varList.Last->data[i].id=parser->token.data.sVal;
	parser->varList.Last->data[i].hid=malloc(strlen(hName) + 1);
	strcpy(parser->varList.Last->data[i].hid,hName);

	/***vytvorenie novej polozky do TS****/
	tItem=createNewItem();
	tItem->name=malloc(strlen(hName) + 1);
	strcpy(tItem->name,hName);
	tItem->dataType=varType;
	insertHashTable(parser->table,tItem);

	if(varType == AUTO_KW){
		parser->token = nextToken();

		if(parser->token.result != EOK)
			return parser->token.result;

		if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,ASSIGN_SMBL))
			return ESYN;

		parser->token = nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;
		parser->assignVarName=hName;
		result=assign(parser);

		if(result != EOK)
			return result;
		/*******3AK , MV , #1 ,NULL, hName******/

	}else{
		parser->token=nextToken();

		if(parser->token.result != EOK)
			return parser->token.result;

		if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
			return EOK;
		}else if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,ASSIGN_SMBL)){
				parser->token = nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
				parser->assignVarName=hName;
				result=assign(parser);
				
				if(result != EOK)
					return result;

				return EOK;

		/*******3AK , MV , #1 ,NULL, hName******/
		}else return ESYN;
	}

}

tresult declaration(tItemPtr varBlock,t_Parser * parser){
	/**deklaracia premennych ----- vola sa na zacitku kazdeho bloku ****/
	tresult result=EOK;
	int varType = parser->token.flags;
	char * hName;
	hTabItem * tItem;

	if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,INT_KW|DOUBLE_KW|STRING_KW|AUTO_KW))
		return EOK;

	parser->token=nextToken();

	if(parser->token.result != EOK)
		return parser->token.result;

	if(!TOKEN_IS(parser->token,ID_TYPE))
		return ESYN;

	/***vlozenie do bloku***/

	int i = 0;
	while(varBlock->data[i].id != NULL){
		if(!strcmp(parser->token.data.sVal, varBlock->data[i].id))
			return ESEM;
		i++;
	}
	
	if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) != NULL)
		return ESEM;

	hName=generateName(parser->hInt,parser);
	parser->hInt++;

	
	if((varBlock->data = realloc(varBlock->data, sizeof(struct Data) * (parser->argsCounter+1) )) == NULL)
		return ESYS;
	varBlock->data[parser->argsCounter].id=parser->token.data.sVal;
	varBlock->data[parser->argsCounter].hid=malloc(strlen(hName) + 1);
	strcpy(varBlock->data[parser->argsCounter].hid,hName);
	parser->argsCounter=parser->argsCounter+1;

	/***vytvorenie novej polozky do TS****/
	tItem=createNewItem();
	tItem->name=malloc(strlen(hName) + 1);
	strcpy(tItem->name,hName);
	tItem->dataType=varType;
	insertHashTable(parser->table,tItem);

	if(varType == AUTO_KW){

		parser->token = nextToken();

		if(parser->token.result != EOK)
			return parser->token.result;

		if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,ASSIGN_SMBL))
			return ESYN;
		
		parser->token = nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;
		parser->assignVarName=hName;
		result=assign(parser);

		if(result != EOK)
			return result;
		
		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;
		result = declaration(varBlock,parser);

		/*******3AK , MV , #1 ,NULL, hName******/

	}else{ //typ int, double alebo string
		
		parser->token=nextToken();

		if(parser->token.result != EOK)
			return parser->token.result;

		if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
			parser->token = nextToken();

			if(parser->token.result != EOK)
				return parser->token.result;
			result = declaration(varBlock,parser);
		}
		else if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,ASSIGN_SMBL)){
				
			parser->token = nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			parser->assignVarName=hName;

			result=assign(parser);

			if(result != EOK)
				return result;
				
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			result = declaration(varBlock,parser);

		}else return ESYN;
	}
}

tresult assign(t_Parser * parser){
	tresult result=EOK;

	if(TOKEN_HAS_TFLAG(parser->token,FN_TYPE,LENGTH_FN|SUBSTR_FN|CONCAT_FN|FIND_FN|SORT_FN)){
		result=buildInF(parser);
	}
	else if(TOKEN_IS(parser->token,ID_TYPE)){
	hTabItem *tableItem;
		if((tableItem=searchItem(parser->table,parser->token.data.sVal)) == NULL){
			/*********hack**********/				///precedencna
			while(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
				parser->token=nextToken();
				if(parser->token.result != EOK)
					return parser->token.result;
			}
			/*********************/
		}else{
			/**volanie uzivatelskej funkcie**/
		tItemPtr item;
		item=parser->paramList.First;
			while(item != NULL){
				if(!strcmp(item->functionId, parser->token.data.sVal))
					break;
				item=item->next;
			}
			if(item == NULL)
				return ESYS;
		hTabItem * tItem;


		if((tItem = searchItem(parser->table,parser->assignVarName)) == NULL)
			return ESYS;
		if(tItem->dataType != tableItem->dataType && tItem->dataType!=AUTO_KW)
			return ESEM2;

		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;
		if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
			return ESYN;

		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;	

		if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))
			result=params(item,parser);
		if(result != EOK)
			return result;

		if(tableItem->params != parser->argsCounter1)
			return ESEM3;
		parser->argsCounter1=0;


		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;
		if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
			return ESYN;

		/***********3AK , JMP, LABEL, PARAMETRE, NULL***********/
		}
	}else if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
		
		/*********hack**********/				///precedencna
		while(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL)){
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
		}
		/*********************/
	}else return ESEM;
return EOK;
}

tresult buildInF(t_Parser * parser){
	tresult result=EOK;
	char * hName;
	hTabItem * tItem;
	hTabItem * tItem1;
	hTabItem * tItem2;
	hTabItem * tItem3;

	switch(parser->token.flags)
	{
		case LENGTH_FN:
			/******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
			if((tItem = searchItem(parser->table,parser->assignVarName)) == NULL)
				return ESYS;
			if(tItem->dataType != INT_KW && tItem->dataType != AUTO_KW)
				return ESEM2;
			
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem1=createNewItem();
				tItem1->name=malloc(strlen(hName) + 1);
				strcpy(tItem1->name,hName);
				tItem1->dataType=STRING_KW;
				tItem1->sVal = parser->token.data.sVal;
				insertHashTable(parser->table,tItem1);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
				/**********vyhladame polozku hName v TS*********/
				/****zkontroluje datovy typ --- musi byt string***/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem1 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem1->dataType != STRING_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}
			else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
				return ESYN;

			//printf("volanie funkcie length --- parameter: %s\n",  tItem1->value.c );
			/*************3AK -- LENGTH , pointer na polozku v ts, NULL ,NULL,***************/
			/********3AK --- MV , searchTS(#1), NULL, searchTS(assignVarName)******/


		break;
		
		case SUBSTR_FN:
			/******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
			if((tItem = searchItem(parser->table,parser->assignVarName)) == NULL)
				return ESYS;
			if(tItem->dataType != STRING_KW && tItem->dataType != AUTO_KW)
				return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem1=createNewItem();
				tItem1->name=malloc(strlen(hName) + 1);
				strcpy(tItem1->name,hName);
				tItem1->dataType=STRING_KW;
				tItem1->sVal = parser->token.data.sVal;
				insertHashTable(parser->table,tItem1);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
				/**********vyhladame polozku hName v TS*********/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem1 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem1->dataType != STRING_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,COMMA_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,INT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem2=createNewItem();
				tItem2->name=malloc(strlen(hName) + 1);
				strcpy(tItem2->name,hName);
				tItem2->dataType=INT_KW;
				tItem2->iVal = parser->token.data.iVal;
				insertHashTable(parser->table,tItem2);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
			/**********vyhladame polozku hName v TS*********/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}

				if((tItem2 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem2->dataType != INT_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,COMMA_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
	

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,INT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem3=createNewItem();
				tItem3->name=malloc(strlen(hName) + 1);
				strcpy(tItem3->name,hName);
				tItem3->dataType=INT_KW;
				tItem3->iVal = parser->token.data.iVal;
				insertHashTable(parser->table,tItem3);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
			/**********vyhladame polozku hName v TS*********/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem2 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem2->dataType != INT_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
				return ESYN;	

			
			//printf("volanie funkcie substr--- parameter 1: %s ,parameter 2: %d, parameter 3: %d\n",  tItem1->value.c, tItem2->value.i, tItem3->value.i );
			/***********************3AK -- SUBSTR , p1 , p2 , p3******************************/
			/***********************3AK -- MV , searchTS(#1),NULL,searchTS(assignVarName)******/
		break;
		case CONCAT_FN:

		/******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
			if((tItem = searchItem(parser->table,parser->assignVarName)) == NULL)
				return ESYS;
			if(tItem->dataType != STRING_KW && tItem->dataType != AUTO_KW)
				return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem1=createNewItem();
				tItem1->name=malloc(strlen(hName) + 1);
				strcpy(tItem1->name,hName);
				tItem1->dataType=STRING_KW;
				tItem1->sVal = parser->token.data.sVal;
				insertHashTable(parser->table,tItem1);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
			/**********vyhladame polozku hName v TS*********/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem1 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem1->dataType != STRING_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,COMMA_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
			/**********vyhladame polozku hName v TS*********/
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem2=createNewItem();
				tItem2->name=malloc(strlen(hName) + 1);
				strcpy(tItem2->name,hName);
				tItem2->dataType=STRING_KW;
				tItem2->sVal = parser->token.data.sVal;
				insertHashTable(parser->table,tItem2);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem2 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem2->dataType != STRING_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
				return ESYN;

			//printf("volanie funkcie concat--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);

		break;
		case FIND_FN: 
		/******************Skontrolovat typ assignVarName v TS - musi byt INT**********************/
			if((tItem = searchItem(parser->table,parser->assignVarName)) == NULL)
				return ESYS;
			if(tItem->dataType != INT_KW && tItem->dataType != AUTO_KW)
				return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem1=createNewItem();
				tItem1->name=malloc(strlen(hName) + 1);
				strcpy(tItem1->name,hName);
				tItem1->dataType=STRING_KW;
				tItem1->sVal = parser->token.data.sVal;
				insertHashTable(parser->table,tItem1);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
				/**********vyhladame polozku hName v TS*********/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem1 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem1->dataType != STRING_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,COMMA_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem2=createNewItem();
				tItem2->name=malloc(strlen(hName) + 1);
				strcpy(tItem2->name,hName);
				tItem2->dataType=STRING_KW;
				tItem2->sVal = parser->token.data.sVal;
				insertHashTable(parser->table,tItem2);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
				/**********vyhladame polozku hName v TS*********/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem2 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem2->dataType != STRING_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
				return ESYN;

	//	printf("volanie funkcie find--- parameter 1: %s ,parameter 2: %s\n",  tItem1->value.c, tItem2->value.c);
		break;
		case SORT_FN: 
		/******************Skontrolovat typ assignVarName v TS - musi byt STRING**********************/
			if((tItem = searchItem(parser->table,parser->assignVarName)) == NULL)
				return ESYS;
			if(tItem->dataType != STRING_KW && tItem->dataType != AUTO_KW)
				return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,LEFT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;

			if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
				hName = generateName(parser->hInt,parser);
				parser->hInt++;
				tItem1=createNewItem();
				tItem1->name=malloc(strlen(hName) + 1);
				strcpy(tItem1->name,hName);
				tItem1->dataType=STRING_KW;
				tItem1->sVal = parser->token.data.sVal;
				insertHashTable(parser->table,tItem1);
			}else if(TOKEN_IS(parser->token,ID_TYPE)){
			/**********vyhladame polozku hName v TS*********/
				if((hName = varSearch(&parser->varList, parser->token.data.sVal)) == NULL){
					if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
						return ESEM;
				}
				if((tItem1 = searchItem(parser->table,hName)) == NULL)
					return ESYS;
				if(tItem1->dataType != STRING_KW && tItem2->dataType!=AUTO_KW)
					return ESEM2;
			}else return ESEM2;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL))
				return ESYN;

			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,SEMICOLON_SMBL))
				return ESYN;

		break;
	}
};


tresult params(tItemPtr item,t_Parser * parser){
tresult result=EOK;
char * hName;
hTabItem * tableItem;

	if(TOKEN_IS(parser->token,ID_TYPE)){
		hTabItem * tItem1;

		if((tableItem=searchItem(parser->table,item->data[parser->argsCounter1].hid)) == NULL)
			return ESYS;
		if((hName = varSearch(&parser->varList,parser->token.data.sVal)) == NULL){
			if((hName = paramSearch(&parser->paramList, parser->fName, parser->token.data.sVal)) == NULL)
				return ESEM;
		}
		if((tItem1=searchItem(parser->table,hName)) == NULL)
			return ESYS;

		if(tableItem->dataType != AUTO_KW && tableItem->dataType != tItem1->dataType && tItem1->dataType!=AUTO_KW)
			return ESEM2;
		printf("ID ide\n");

		/****************parameter tItem1*****************/
	}
	else if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,INT_CONST)){
		if((tableItem=searchItem(parser->table,item->data[parser->argsCounter1].hid)) == NULL)
			return ESYS;
		
		if(tableItem->dataType != INT_KW && tableItem->dataType != AUTO_KW)
			return ESEM2;

		hTabItem * tItem1 = createNewItem();
		hName=generateName(parser->hInt,parser);
		parser->hInt++;
		tItem1->name=malloc(strlen(hName) + 1);
		strcpy(tItem1->name,hName);
		tItem1->dataType = INT_KW;
		tItem1->iVal = parser->token.data.iVal;
		insertHashTable(parser->table,tItem1);

		/**************************************/

	}
	else if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,DOUBLE_CONST)){
		if((tableItem=searchItem(parser->table,item->data[parser->argsCounter1].hid)) == NULL)
			return ESYS;

		if(tableItem->dataType != DOUBLE_KW && tableItem->dataType != AUTO_KW)
			return ESEM2;

		hTabItem * tItem1 = createNewItem();
		hName=generateName(parser->hInt,parser);
		parser->hInt++;
		tItem1->name=malloc(strlen(hName) + 1);
		strcpy(tItem1->name,hName);
		tItem1->dataType = DOUBLE_KW;
		tItem1->dVal = parser->token.data.dVal;
		insertHashTable(parser->table,tItem1);

		/****************************************/

	}
	else if(TOKEN_HAS_TFLAG(parser->token,CONST_TYPE,TEXT_CONST)){
		if((tableItem=searchItem(parser->table,item->data[parser->argsCounter1].hid)) == NULL)
			return ESYS;

		if(tableItem->dataType != STRING_KW && tableItem->dataType != AUTO_KW)
			return ESEM2;

		hTabItem * tItem1 = createNewItem();
		hName=generateName(parser->hInt,parser);
		parser->hInt++;
		tItem1->name=malloc(strlen(hName) + 1);
		strcpy(tItem1->name,hName);
		tItem1->dataType = STRING_KW;
		tItem1->sVal = parser->token.data.sVal;
		insertHashTable(parser->table,tItem1);

		/**************************************/
	}
	else return ESEM2;

	parser->argsCounter1++;

	parser->token=nextToken();
	if(parser->token.result != EOK)
		return parser->token.result;
	if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,COMMA_SMBL)){
		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;
		result=params(item,parser);
	}
	else if(!TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL)){
		return EOK;
	}
	else return ESYN;
}

tresult args(tItemPtr item,t_Parser * parser){
	/******spracovanie argumentov funkcii********/	
		tresult result=EOK;
		int varType = parser->token.flags;
		hTabItem * tItem;
		if(!TOKEN_HAS_TFLAG(parser->token,KW_TYPE,INT_KW|DOUBLE_KW|STRING_KW))
			return ESYN;

		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;

		if(TOKEN_IS(parser->token,ID_TYPE))
			return ESYN;

			if(parser->fDeclared){
				char * hName = NULL;
				int i = 0;
				while(item->data[i].id != NULL){
					if(!strcmp(parser->token.data.sVal, item->data[i].id))
					{
						hName = item->data[i].hid;
						break;
					}
					i++;
				}
				if(hName == NULL)
					return ESEM;

				if((tItem=searchItem(parser->table, hName)) == NULL){
					return ESYS;
				}
				/****kontrola typu a poradia parametru****/
				if(tItem->dataType != varType)
					return ESEM;
				if(tItem->paramPosition != parser->argsCounter)
					return ESEM;

			}
			else if(!parser->fDeclared){
				char * hName = generateName(parser->hInt,parser);
				parser->hInt++;
				int i = 0;

				while(item->data[i].id != NULL){
					if(!strcmp(parser->token.data.sVal, item->data[i].id))
						return ESEM;
					i++;
				}

				if((item->data = realloc(item->data, sizeof(struct Data) * (parser->argsCounter+1) )) == NULL){
					return ESYS;
				}
				item->data[parser->argsCounter].id=parser->token.data.sVal;
				item->data[parser->argsCounter].hid=malloc(strlen(hName) + 1);
				strcpy(item->data[parser->argsCounter].hid,hName);

				/*vlozi parameter do TS -nazov, typ, funkciu a poradie*/
				tItem=createNewItem();
				tItem->name=malloc(strlen(hName) + 1);
				strcpy(tItem->name,hName);
				tItem->dataType=varType;
				tItem->paramPosition=parser->argsCounter;
				insertHashTable(parser->table,tItem);
			}

		parser->argsCounter++;

		/*ak je dalsi parser->token , rekurzivne sa zavola funckia args a nacitava sa dalsi parameter*/
		/*ak je to zatvorka tak sa vracia EOK*/
		/*iny parser->token znamena chybu*/
		parser->token=nextToken();
		if(parser->token.result != EOK)
			return parser->token.result;

		if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,RIGHT_CULUM_SMBL)){
			return EOK;
		}
		else
		if(TOKEN_HAS_TFLAG(parser->token,SMBL_TYPE,COMMA_SMBL)){
			parser->token=nextToken();
			if(parser->token.result != EOK)
				return parser->token.result;
			result = args(item,parser);
		}
		else return ESYN;

}

int main(){
	tresult result=EOK;
	source = fopen("src","r");
	//printf("sadsa\n");

	t_Parser parser;
	result= init_parser(&parser);
	if(result != EOK)
		printf("Init fail\n");
	
	result = parse(&parser);
	printf("err code: %d" , result);
}	
