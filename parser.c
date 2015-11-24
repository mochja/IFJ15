#include <stdio.h>
#include "scaner.h"
#include "parser.h"


tresult result;

tresult parse(){
/**incializacia  ---- overenie prazdnosti zdrojaku */
 
	/********MISSING: incializacia tabulky***********/
	/********MISSING: incializacia pola 3 adres. kodu*/

	result = EOK;
	token = nextToken();


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


	/******************MISSING: zrusenie tabulky **********************/
	/******************MISSING: zrusenie pola 3 adr. kodu**************/

	return result;

}


tresult program(){
/**overim prvy token**/
	if(token.type != INT || token.type != DOUBLE|| token.type != STRING)
		return ESYN;

	result = function();


	if(result != EOK)
		return result;

	token = nextToken();

	if(token.type != END_OF_FILE)
		return ESYN;

	return result;

/****ak prvy token nie je datovy typ --> error ****/

/**ak je to datovy typ vola sa function ***/


/*po analyze vsetkcyh funkci sa skontroluje ci posledna funkcia bola main---ak ano EOK ak nie error -------posledny by mal byt EOF **/

}


tresult function(){
/** precita dalsi tokenn***/

	/****ak bol novey token klucove slovo main a predchadzajuci bol int prechadza do bloku s definovanim funkcie main**/

		/*main *****
			{
				/***nacita tokeny -- ocakava () a potom { ----> ak to nedostane jedna sa o chybu****
				/***vola sa funkcia Body ktora analyzuje telo funkcie*-***\
				/***po uspesnom navrate ocakava token } ***
			}
		
		**********/


	/*ak bol token ID  skontroluje sa  ---- > ci uz nebola takato funkcia nahodou deklarovana atd--- ak je to v poriadku prejde do bloku s definovanim novej funkcie**/
	/* ina funkcia ******
		{
			/***nacita token ocakava ( --- potom mozne parametre --- dalsi token ) a { ----------->  v pripade neuspechu error********
			/***vola sa funkica Body ktora analyzuje telo funkcie *********
			/*** po uspesnom navrate sa ocakavva token }

			/***nacita dalsi token ---> ak nieje EOK  je to chyba -- ak je skontroluje ci je tento token datovy typ
			/***ak je datovy typ vola sa rekurzivne funkcia function----> .....=function();
			/***ak LEX navratil EOK ale nejdena sa o datovy typ tak je to syntakticka chyba**
		}
	*/
	/**ak po datovom type ktorym bol prvy token, nedostaneme ID alebo klucove slovo main ----> error**/


	int fType = token.type;

	token = nextToken();

	if(token.type == MAIN || token.type == INT){
		/***********telo funkcie main**************/

			char *fLabel = token.data.s;

			/********MISSSING: kontrola ts, vlozenie do ts **********************/



			token = nextToken();

			if(token.type != LEFT_CULUM) // (
				return ESYN;

			token = nextToken();

			if(token.type != RIGHT_CULUM)	// )
				return ESYN;

			token = nextToken();	

			if(token.type != LEFT_VINCULUM) // {
				return ESYN;

			/********MISSING: vlozenie 3AK -- label zaciatku funkcie main********/

			token = nextToken();

			if(token.result != EOK)
				return token.result;

			if(token.type != RIGHT_VINCULUM)		//ak nie je funkcia prazdna
				result = body();
													/// po main uz ziadne dalsie rekurzivne volanie ---- main je posledna funkcia v programe
	}
	else if(token.type = ID){
		/*****************telo funkcie*************/

			char *fLabel = token.data.s;

			/********MISSSING: kontrola ts, vlozenie do ts **********************/

			token = nextToken();

			if(token.type != LEFT_CULUM)
				return ESYN;

			result=args();    //parsovanie argumentov funkcie

			if(result != EOK)
				return result;
								//uz sa nekontroluje token ")" pretoze funkcia args sa vrati vzdy uspesne ukoncena tymto tokenom
			token=nextToken();
			if(token.type != LEFT_VINCULUM)
				return ESYN;

			/********MISSING: vlozenie 3AK -- label zaciatku funkcie*******/
			token=nextToken();

			if(token.result != EOK)
				return token.result;

			if(token.type != RIGHT_VINCULUM)		//ak nie je funkcia prazdna
				result = body();

			if(result != EOK)
				return result;

			/********MISSING: vlozenie 3AK -- navrat z funkcie*******/

			token=nextToken();

			if(token.result != EOK)	//lexikalna alebo systemova chyba
				return token.result;

			if(token.type == INT || token.type == DOUBLE || token.type == STRING)	///mala by nasledovat dalsia funkcia
				result = function();		//rekurzivne volanie pre spracovanie dalsej funkcie
			else
				return ESYN;	//ak nie je ziadna dalsia funkcia je to chyba
	}
	else result = ESYN;


return result;


}


tresult body(){
/******telo funkcie********/

}

tresult declaration(){
	/**deklaracia premennych ----- vola sa na zacitku kazdeho bloku ****/

}

tresult args(){
	/******spracovanie argumentov funkcii********/	
}
