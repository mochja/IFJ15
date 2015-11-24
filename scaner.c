#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include "scaner.h"


#define ALLOC_SIZE 		8

FILE *source;

/*********************zoznam stavov***********************************/
enum{
	STATE_START = 0,     		//pociatocny stav
	STATE_K_OR_ID,				//klucove slovo alebo ID
	STATE_DEV_OR_COM,			// / alebo // alebo /*
	STATE_LINE_COM,				// //
	STATE_BLOCK_COM,			// /*
	STATE_BLOCK_COM_END,		// */
	STATE_EQ_OR_AS,				// = alebo ==
	STATE_L_ARR_LQ,				// < alebo <<
	STATE_R_ARR_MQ,				// > alebo >>
	STATE_NOT,					// !=
	STATE_NUMBER,				// cislo int
	STATE_D_NUMBER,				// cislo double
	STATE_D_NUMBER2,			// cislo double
	STATE_E,					// exponent
	STATE_E2,					// cislo s exponentom
	STATE_STRING,				// retazec
	STATE_ESCAPE,				// escape sekvencia napr. \n
	STATE_X,					// escape sekvencia \xDD , kde dd je hex. cislo
};

/***************************inicializacia stringu*************************************/
int initString(string *s){
	if((s->str  =(char*) malloc(ALLOC_SIZE)) == NULL)
		return SYS_ERROR;
	s->str[0]='\0';
	s->length = 0;
	s->allocSize = ALLOC_SIZE;
	
}

/***********************prida znak do retazca**************************************
********************realokuje v pripade ak je retazec plny ************************/
int strAdd(string *s, char c){

	if(s->length+1 >= s->allocSize){
		if((s->str = (char*)realloc(s->str,s->length + ALLOC_SIZE)) == NULL){
			free(s);
			return SYS_ERROR;
		}
		s->allocSize = s->length + ALLOC_SIZE;
	}

	s->str[s->length] = c;
	s->str[++s->length] = '\0';
}

struct T_Token nextToken()
{
	int state = STATE_START;
	int c;
	int lineNumber = 1;

	string arr;
	arr.str = NULL;
	
	struct T_Token token;
	token.type = BASIC;
	token.result = EOK;

	while(1){

		c = getc(source);

		/**************koniec suboru *********************/
		if(c == EOF){	
			token.type = END_OF_FILE;
			token.result = EEOF;
			return token;
		}

		/**************novy riadok ***********************/
		if( c == '\n'){
			lineNumber++;
		}

		switch(state){
			case STATE_START:

				/***********mezdera, biely znak*****************/
				if(isspace(c)) 
					state = STATE_START;

				/*************Pismeno alebo znak '_'***********/
				else if( isalpha(c) || c == '_') 
					{
						if(initString(&arr) ==  SYS_ERROR){
							token.result = ESYS; 
							token.type = SYS_ERROR;
						}
						
						if(strAdd(&arr,c) == SYS_ERROR){
							token.type = SYS_ERROR;
							token.result = ESYS;
						}

						state = STATE_K_OR_ID;
					}

				/************cislo *****************************/
				else if(isdigit(c))
					{
						if(initString(&arr) == SYS_ERROR){ 
							token.type = SYS_ERROR;
							token.result = ESYS;
						}
						
						if(strAdd(&arr,c) == SYS_ERROR){
							token.type = SYS_ERROR;
							token.result = ESYS;
						}

						state = STATE_NUMBER;
					}
				/*******************string*************************/
				else if(c == '"')
					{
						if(initString(&arr) == SYS_ERROR){ 
							token.type = SYS_ERROR;
							token.result = ESYS;
						}

						state = STATE_STRING;
					}

				/*************komentar alebo delenie ***************/
				else if( c == '/'){
					state = STATE_DEV_OR_COM;
				}

				else if( c == '='){
					state = STATE_EQ_OR_AS;
				}

				else if ( c == '<'){
					state = STATE_L_ARR_LQ;
				}

				else if ( c == '>'){
					state = STATE_R_ARR_MQ;
				}

				else if ( c == '!'){
					state = STATE_NOT;
				}

				else if ( c == ';' ){
					token.type = SEMICOLON;
				}

				else if( c == '('){
					token.type = LEFT_CULUM;
				}

				else if( c == ')'){
					token.type = RIGHT_CULUM;
				}

				else if( c == '{'){
					token.type = LEFT_VINCULUM;
				}

				else if( c == '}'){
					token.type = RIGHT_VINCULUM;
				}

				else if( c == ','){
					token.type = COMMA;
				}

				else if( c == '.'){
					token.type = DOT;
				}

				else if( c == '+'){
					token.type = PLUS;
				}

				else if( c == '-'){
					token.type = MINUS;
				}

				else if( c == '*'){
					token.type = MULTIPLY;
				}



				else {
					fprintf(stderr,"LEX Error: Line: %d , Unknown token: '%c'\n", c,lineNumber);
					token.type = LEX_ERROR;
					token.result = ELEX;
				}

			break;

			/*****klucove slovlo alebo ID **********/
			case STATE_K_OR_ID:	 
				if(isalnum(c) || c == '_'){  // ak je c pismeno, cislo alebo znak _ prida sa do retazca
					if(strAdd(&arr,c) == SYS_ERROR)
						token.type = SYS_ERROR;
						token.result = ESYS;
				}

				/*ak je c niektory z povolenych znakov*/
				else if( c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == ',' ||
						 c == '.' || c == '=' || c == '<' || c == '>' || c == '!' || c == '+' || 
						 c == '-' || c == '*' || c == '/' || c == '"' || isspace(c) || c == EOF)
				{
					if(c == '\n'){
						lineNumber--;
					}

					ungetc(c, source);
					
					/*kontrola ci retazec nie je klucove slovo */
					if(!strcmp(arr.str,"auto"))		{token.type = AUTO; 	free(arr.str);} else
					if(!strcmp(arr.str,"cin"))		{token.type = CIN; 		free(arr.str);}	else
					if(!strcmp(arr.str,"cout"))		{token.type = COUT;		free(arr.str);}	else					
					if(!strcmp(arr.str,"double"))	{token.type = DOUBLE;	free(arr.str);}	else
					if(!strcmp(arr.str,"else"))		{token.type = ELSE;		free(arr.str);}	else
					if(!strcmp(arr.str,"for"))		{token.type = FOR;		free(arr.str);}	else
					if(!strcmp(arr.str,"if"))		{token.type = IF;		free(arr.str);}	else					
					if(!strcmp(arr.str,"int"))		{token.type = INT; 		free(arr.str);}	else
					if(!strcmp(arr.str,"return"))	{token.type = RETURN;	free(arr.str);}	else
					if(!strcmp(arr.str,"string"))	{token.type = STRING;	free(arr.str);}	else
					if(!strcmp(arr.str,"main"))		{token.type = MAIN; 	free(arr.str);}	else
					if(!strcmp(arr.str,"length"))	{token.type = LENGTH;	free(arr.str);}	else
					if(!strcmp(arr.str,"substr"))	{token.type = SUBSTR;	free(arr.str);}	else
					if(!strcmp(arr.str,"concat"))	{token.type = CONCAT;	free(arr.str);}	else
					if(!strcmp(arr.str,"find"))		{token.type = FIND;		free(arr.str);}	else
					if(!strcmp(arr.str,"sort"))		{token.type = SORT; 	free(arr.str);}	else

					{
						token.type = ID; //ak retazec nie je klucove slovo jedna sa o ID
						token.data.s = arr.str;	//nastavy sa pointer na retazec - uvolni sa mimo tejto funkcie
					}

				}
				else{ // v pripade nepovoleneho znaku dochadza k chybe
					fprintf(stderr,"LEX Error: Line: %d , Unknown token: '%c'\n", c, lineNumber);
					token.type = LEX_ERROR;
					token.result = ELEX;
				};
			break;

			/*****komentar alebo delenie*******/
			case STATE_DEV_OR_COM:  
				if(c == '/'){
					state = STATE_LINE_COM;
				}
				else if(c == '*'){
					state = STATE_BLOCK_COM;
				}
				else{
					if(c == '\n'){
						lineNumber--;
					}
					ungetc(c,source);
					token.type = DEVIDE;
				}
			break;

			/***********riadkovy komentar**********/
			case STATE_LINE_COM: 
				if(c == '\n'){
					state = STATE_START;
				}
				else if(c == EOF){
					token.type = END_OF_FILE;
					token.result = EEOF;
				}
			break;

			/**********blokovy komentar ************/

			case STATE_BLOCK_COM: 
				if(c == '*'){
					state = STATE_BLOCK_COM_END;
				}
				else if(c == EOF){
					token.type = END_OF_FILE;
					token.result = EEOF;
				}	
			break;

			/*********koniec blokoveho komentara ******/
			case STATE_BLOCK_COM_END: 
				if(c == '/'){
					state = STATE_START;
				}
				else 
				if(c == EOF){
					token.type = END_OF_FILE;
					token.result = EEOF;
				}
				else{
					if(c == '\n'){
						lineNumber--;
					}
					state = STATE_BLOCK_COM;
					ungetc(c,source);
				}
			break;

			/************* = alebo == ***********************/
			case STATE_EQ_OR_AS: 
				if(c == '='){
					token.type = EQUALS;
				}
				else{
					if(c == '\n'){
						lineNumber--;
					}
					ungetc(c,source);
					token.type = ASSIGN;
				}
			break;

			/***************< alebo << alebo <= **************/
			case STATE_L_ARR_LQ: 
				if(c == '<')
					token.type = DBL_ARR_LEFT;
				else if ( c == '=')
					token.type = LESS_OR_EQUAL;
				else{
					if(c == '\n'){
						lineNumber--;
					}
					ungetc(c,source);
					token.type = LEFT_ARROW;
				}
			break;

			/*************** > alebo >> alebo >= **************/
			case STATE_R_ARR_MQ: 
				if(c == '>')
					token.type = DBL_ARR_RIGHT;
				else if( c == '=')
					token.type = MORE_OR_EQUAL;
				else{
					if(c == '\n'){
						lineNumber--;
					}
					ungetc(c,source);
					token.type = RIGHT_ARROW;
				}
			break;

			/********************* !=  *********************************/
			case STATE_NOT: 
				if( c == '=')
					token.type = NOT_EQUAL;
				else{ 
					token.type = LEX_ERROR;
					token.result = ELEX;
				}
			break;

			/*************************cislo******************************/
			case STATE_NUMBER: 

				if(isdigit(c)){							//ak je znak cislo prida sa do retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
				}

				else if(c == '.'){						//ak je znak '.' prechadza sa do stavu cisla double, pridame ro retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_D_NUMBER;
				}

				else if(c == 'E' || c == 'e'){			// ak je c E alebo e prechadza sa do stavu cisla s exp , pridame do retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_E;
				}

				// c je niektory z povloenych znakov
				else if( c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == ',' ||
						 c == '=' || c == '<' || c == '>' || c == '!' || c == '+' || c == '-' || 
						 c == '*' || c == '/' || c == '"' || isspace(c))
				{
					if(c == '\n'){
						lineNumber--;
					}

					ungetc(c,source);

					if(sscanf(arr.str, "%d" , &token.data.i) != 1){ // konverzia retazca na cislo typu int
						fprintf(stderr,"Converting error: cant convert %s to Int\n", arr.str);
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					else{
						token.type = INT_NUM;
					}
					free(arr.str);
				}
				else{	//Lex. chyba pri zadani nepovolenych znakov
					fprintf(stderr,"LEX Error: Line: %d , Unknown token: '%c'\n", c, lineNumber);
					free(arr.str);
					token.type = LEX_ERROR;
					token.result = ELEX;
				}
			break;

			/******** prvy stav cisla double - sem sa dostaneme napr s cislom 1. ********
			******************v tomto stave ocakavame iba cislo**************************/
			case STATE_D_NUMBER: 
				if(isdigit(c)){			// ak je znak c cislo pokracujeme do stavu STATE_D_NUMBER2 , pridame do retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_D_NUMBER2;
				}
				else{					// ak je c nieje cislo jedna sa o Lex. chybu
					fprintf(stderr,"LEX Error: Line: %d , Unknown token: '%c'\n", c , lineNumber);
					token.type = LEX_ERROR;
					token.result = ELEX;
					free(arr.str);
				}
			break;

			/***************************druhy stav cisla double****************************************/
			case STATE_D_NUMBER2: 
				if(c == 'E' || c == 'e'){		// znak c je pismeno e alebo E - cislo double s exponentom , pridame do retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_E;
				}

				else if(isdigit(c)){			// znak c je cislo - prida sa do retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
				}

				//c je niektory z povolenych znakov
				else if( c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == ',' ||
						 c == '=' || c == '<' || c == '>' || c == '!' || c == '+' || c == '-' || 
						 c == '*' || c == '/' || c == '"' || isspace(c))
				{
					if(c == '\n'){
						lineNumber--;
					}

					ungetc(c,source);

					if(sscanf(arr.str, "%lf" , &token.data.d) != 1){ // konverzia retazca na cislo typu double
						fprintf(stderr,"Converting error: cant convert %s to Double\n", arr.str);
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					else{
						token.type = DOUBLE_NUM;
					}
					free(arr.str);
				}
				else{  //ak je c nepovoleny znak - Lex chyba
					fprintf(stderr,"LEX Error: Line: %d , Unknown token: '%c'\n", c, lineNumber);
					token.type = LEX_ERROR;
					token.result = ELEX;
					free(arr.str);
				}
			break;

			/*****stav cisla s exponentom,  sem sa dostavame napr s cislom 1e alebo 1,1e*******/
			case STATE_E: 
				if(c == '-' || c == '+' || isdigit(c)){	//nasledujuci znak musi byt bud cislo, + alebo -
					if(strAdd(&arr,c) == SYS_ERROR){	//prida sa do retazca
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_E2;
				}
				else{	//v pripade nepovoleneho znaku - Lex. chyba
					fprintf(stderr, "LEX Error: Line: %d , Unknown token: '%c'\n", c, lineNumber);
					token.type = LEX_ERROR;
					token.result = ELEX;
					free(arr.str);
				}
			break;

			/*****************pokracovanie cisla s exp ********************************/
			case STATE_E2: 
				if(isdigit(c)){		//ak je c cislo prida sa do retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}

				}

				//ak je c povoleny znak
				else if( c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == ',' ||
						 c == '=' || c == '<' || c == '>' || c == '!' || c == '+' || c == '-' || 
						 c == '*' || c == '/' || c == '"' || isspace(c))
				{
					if(c == '\n'){
						lineNumber--;
					}

					ungetc(c,source);

					if(sscanf(arr.str, "%lf" , &token.data.d) != 1){ // konverzia retazca na cislo typu double s exp
						fprintf(stderr,"Converting error: cant convert %s to Double\n", arr.str);
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					else{
						token.type = DOUBLE_NUM;
					}
					free(arr.str);
				}
				else{ // nepovoleny znak - Lex chyba
					fprintf(stderr, "LEX Error: Line: %d , Unknown token: '%c'\n", c, lineNumber);
					free(arr.str);
					token.type = LEX_ERROR;
					token.result = ELEX;
				}	
			break;

			/******************STRING******************************/
			case STATE_STRING: 
				if(c == '"'){  //ak c je znak " znamena to ukoncenie retazca - nastavy sa token a pointer na string
					token.type = TEXT;
					token.data.s = arr.str; // nastavi pointer na retazec - uvolni sa mimo tejto funkcie
				}
				else if( c == '\\'){ 		//znak \ - pokracuje sa do escape sekvencie
					state = STATE_ESCAPE;
				}
				else if(c == EOF || c == '\0')			// koniec suboru alebo retazca - Lex chyba 
				{
					fprintf(stderr, "LEX Error: Line: %d , Unknown token: '%c'\n", c,lineNumber);
					free(arr.str);
					token.type = LEX_ERROR;
					token.result = ELEX;
				}
				else{           // prida znak do retazca
					if(strAdd(&arr,c) == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
				}	
			break;

			/************** ESCAPE SEKVENCIA NAPR \n**********************/
			case STATE_ESCAPE:
				if(c == 'x'){		// ak je znak za  \ x prejdeme do stavu STATE_X
					state = STATE_X;
				}
				else if(c == 'n'){	//c je znak n pridame do retazca '\n'
					if(strAdd(&arr,'\n') == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_STRING;
				}

				else if(c == 't'){	//c je znak t pridame do retazca '\t'
					if(strAdd(&arr,'\t') == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_STRING;
				}

				else if(c == '\\'){	//c je znak \ pridame do retazca '\\'
					if(strAdd(&arr,'\\') == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_STRING;
				}

				else if(c == '\"'){	//c je znak " pridame do retazca '\"'
					if(strAdd(&arr,'\"') == SYS_ERROR){
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_STRING;
				}
				else{	//iny znak znamena Lex. chybu
					fprintf(stderr, "LEX Error: Line: %d , Unknown token: '%c'\n", c, lineNumber);
					free(arr.str);
					token.type = LEX_ERROR;
					token.result = ELEX;
				}
			break;

			/************************* \X ***********************************************/
			/*********nasledujuce 2 znaky ocakavame v rozsahu 0-1 alebo a-f || A-F*******/
			case STATE_X:
			{
				char s;		
				char ascii[2]; //pole o 2 prvkoch do ktoreho ulozime 2 nacitane znaky
				int counter;

					for(counter = 0 ; counter < 2 ; counter++){ //nacitaju sa 2 znaky
						if((c < 30 && c > 39) && (c < 41 && c > 46) && (c < 61 && c > 66)){ // kontorla podmienky znaku
							fprintf(stderr, "LEX Error: Line: %d , Unknown token: '%c'\n", c, lineNumber);
							free(arr.str);
							token.type = LEX_ERROR;
							token.result = ELEX;
						}
						else{//ak bol znak spravny prida sa do pola ascii[];
							ascii[counter] = c;
							c = getc(source); // nacita novy znak
						}
					}

					if(c == '\n'){
						lineNumber--;
					}

					//unget z dovodu nacitania o jedneho znaku viac po prejdeni cyklom
					ungetc(c,source);

					s = strtol(ascii,NULL,16); // retazec ascii sa zmeni na hex cislo a tento znak sa prida do premennej s

					if(strAdd(&arr,s) == SYS_ERROR){ //znak s sa vlozi do retazca
						token.type = SYS_ERROR;
						token.result = ESYS;
					}
					state = STATE_STRING;}
			break;
		}

	/*ak sa token nastavi je vrateny*/
	if(token.type != BASIC)
		return token;
	}
}
