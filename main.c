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
        fprintf(stderr, "Could not find file %s\n", filename);
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
        fprintf(stderr, "Nou mor memory.\n");
        fclose(f);
        return NULL;
    }

    fclose(f);

    return source;
}

int main(int argc, char *argv[])
{
    debug_print("%s\n", " ");

    result_t res;

    if (argc < 2) {
        // print help, usage...
        printf("usage %s source_file\n", argv[0]);
        return ESYS; // ?
    }

    char *source;
    if ((source = read_source_file(argv[1])) == NULL) {
        return ESYS;
    }

    parser_t parser;
    res = init_parser(&parser, source);
    free(source);

    if (res == EOK) {
        if ((res = parser_run(&parser)) != EOK) {
            parser_dispose(&parser);
            return res;
        }

        vm_t vm;
        if ((res = vm_init(&vm, parser.code)) != EOK) {
            parser_dispose(&parser);
            return res;
        }

        if ((res = parser_dispose(&parser)) != EOK) {
            return res;
        }

        if ((res = vm_exec(&vm)) != EOK) {
            vm_dispose(&vm);
            return res;
        }

        if ((res = vm_dispose(&vm)) == EOK) {
             return res;
        }
    }

    return res;
}
