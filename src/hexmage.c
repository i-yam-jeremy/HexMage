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

int HexMage_compile(char* path) {
    char* code = read_text_file(path);
    printf("Done reading file\n");
    TokenList* tokens = lex(code);
    printf("Done lexing\n");
    ASTNode ast = parse(tokens);
    
    /*while (tokens != 0) {
        printf("{'%s', '%.*s'}\n", tokens->token.name, tokens->token.size, tokens->token.value);
        tokens = tokens->next;
    }*/
    
    return 0;
}