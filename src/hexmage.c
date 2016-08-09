//
//  hexmage.c
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#include "hexmage.h"

#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

char* read_file(char* path) {
    FILE* f = fopen(path, "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char* content = malloc(size);
        fread(content, sizeof(char), size, f);
        fclose(f);
        return content;
    }
    else {
        fprintf(stderr, "Couldn't read file '%s'.", path);
        exit(1);
    }
}

int HexMage_compile(char* path) {
    char* code = read_file(path);
    TokenList* tokens = lex(code);
    
    return 0;
}