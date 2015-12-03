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

#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>
#include "scaner.h"
#include "scaner.h"
#include "list.h"
#include "hash.h"

typedef struct t_parser{
	tvarList varList;
	tvarList paramList;
	char * fName;
	int argsCounter;
	int argsCounter1;
	bool fDeclared;
	int hInt;
	int label;
	char buffer[20];
	char * assignVarName;
	tTable *table;
}t_Parser;


struct T_Token token;

tresult init_parser(t_Parser *parser);
tresult parse();
tresult program();
tresult function();
tresult body();
tresult declaration();
tresult advDeclaration();
tresult args();
tresult buildInF();
tresult params();
tresult assign();
tresult list();

#endif // PARSER_H_
