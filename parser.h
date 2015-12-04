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
#include "token.h"
#include "zval.h"

typedef struct {
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
	token_t token;

    char *source;
} parser_t;

result_t init_parser(parser_t *parser, const char *source);
result_t parse(parser_t *parser);
result_t program(parser_t *parser);
result_t function(parser_t *parser)

//result_t body();
//result_t declaration();
//result_t advDeclaration();
//result_t args();
//result_t buildInF();
//result_t params();
//result_t assign();
//result_t list();

#endif // PARSER_H_
