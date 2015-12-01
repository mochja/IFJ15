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

#include "scaner.h"

struct T_Token token;

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
