/**
 * Copyright (c) 2015, Team Unknown,
 *                     Ján Mochňak,    <xmochn00@vutbr.cz>
 *                     Tibor Dudlák,   <xdudla00@vutbr.cz>
 *                     Dávid Prexta,   <xprext00@vutbr.cz>
 *                     Martin Krajňák, <xkrajn02@vutbr.cz>
 *                     Patrik Segedy,  <xseged00@vutbr.cz>
 *
 * This source code is licensed under the MIT license found in the
 * license.txt file in the root directory of this source tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "parser.h"

static char* read_source_file(const char *filename) {
    FILE *f;
    f = fopen(filename, "r");

    if (f == NULL) {
        puts("could not read a file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);

    char *source;
    if ((source = malloc(size * sizeof(char))) != NULL) {
        fread(source, 1, size, f);
    }

    fclose(f);

    return source;
}

int main(int argc, char *argv[])
{
    result_t res = EOK;

    if (argc < 2) {
        // print help, usage...
        printf("usage %s source_file\n", argv[0]);
        return 1;
    }

    char *source = read_source_file(argv[1]);

//    parser_t parser;
//    if ((res = init_parser(&parser, source)) != EOK) {
//        printf("Parser Init failed\n");
//    } else {
//        res = parse(&parser);
//    }

    free(source);
    return res;
}
