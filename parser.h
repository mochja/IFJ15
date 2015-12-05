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
#include "scanner.h"
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
	char * assignVarName;
	tTable *table;
    token_t token;
    scanner_t scanner;
} parser_t;

result_t init_parser(parser_t *parser, char *source);
result_t parser_run(parser_t *parser);

result_t parse_fn(parser_t *parser);
result_t parse_fn_body(parser_t *parser);
result_t parse_fn_args(parser_t *parser, tItemPtr item);
result_t parse_fn_declaration(parser_t *parser, tItemPtr varBlock);
result_t parse_list(parser_t *parser);
result_t parse_assign(parser_t *parser);
result_t parse_adv_declaration(parser_t *parser);
result_t parse_build_in_fn(parser_t *parser);
result_t parse_params(parser_t *parser, tItemPtr item);

result_t parser_next_token(parser_t *parser);

#endif // PARSER_H_
