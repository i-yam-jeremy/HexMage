//
//  hexmage.c
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#include "hexmage.h"

#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

char* read_text_file(char* path) {
    FILE* f = fopen(path, "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char* content = malloc(size+1);
        content[size] = '\0';
        fread(content, sizeof(char), size, f);
        fclose(f);
        return content;
    }
    else {
        fprintf(stderr, "Couldn't read file '%s'.", path);
        exit(1);
    }
}

ASTNode parse_file(char* path) {
    char* code = read_text_file(path);
    printf("[Done reading file '%s']\n", path);
    TokenList* tokens = lex(code);
    printf("[Done lexing '%s']\n", path);
    ASTNode ast = parse(tokens);
    printf("[Done parsing '%s']\n", path);
    return ast;
}


int HexMage_compile(char* path) {
    ASTNode ast = parse_file(path);
    
    return 0;
}