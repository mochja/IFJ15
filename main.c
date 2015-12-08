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
#include "vm.h"

char* read_source_file(const char *filename) {
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
    if ((source = malloc((size + 1) * sizeof(char))) != NULL) {
        fread(source, 1, size, f);
        source[size] = '\0';
    } else {
        puts("asdf.");
        fclose(f);
        return NULL;
    }

    fclose(f);

    return source;
}

int main(int argc, char *argv[])
{
    result_t res;

    if (argc < 2) {
        // print help, usage...
        printf("usage %s source_file\n", argv[0]);
        return ESYS; // ?
    }

    char *source;
    if (( source = read_source_file(argv[1])) == NULL){ return ESYS; } // if file does not exist it is error

    parser_t parser;
    res = init_parser(&parser, source);
    free(source);

    if (res == EOK) {

        if ((res = parser_run(&parser)) != EOK) {
            destroy_parser(&parser);
            return res;
        }

        vm_t vm;

        vm_init(&vm, parser.code);
        vm_exec(&vm);
        vm_dispose(&vm);
    }

    return res;
}
