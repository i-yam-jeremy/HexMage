//
//  lexer.c
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#include "lexer.h"

#include <regex.h>
#include <string.h>
#include <stdio.h>

typedef struct tokendef {
    char* name;
    regex_t regex;
} TokenDef;

TokenDef td(char* name,  char* regex) {
    TokenDef def;
    def.name = name;
    int err = regcomp(&def.regex, regex, 0);
    
    if (err) {
        fprintf(stderr, "Could not compile regex for token definition '%s'.", name);
        exit(1);
    }
    
    return def;
}

TokenDef* get_token_definitions(int* token_definition_count) {
    TokenDef TOKEN_DEFS[] = {
        td("identifier", "[A-Za-z][A-Za-z0-9_]*"),
        td("whitespace", "[ \t]+")
    };
    *token_definition_count = sizeof(TOKEN_DEFS)/sizeof(TokenDef);
    return TOKEN_DEFS;
}

Token get_next_token(char* code, TokenDef* token_definitions, int token_definition_count) {
    Token t;
    t.name = "Test";
    t.value = "testvalue"; //TODO FIXME
    return t;
}

TokenList* lex(char* code) {
    int token_definition_count;
    TokenDef* token_definitions = get_token_definitions(&token_definition_count);
    TokenList* tokens = 0;
    while (code[0] != '\0') { // while (strlen(code) > 0) // but more efficient
        Token token = get_next_token(code, token_definitions, token_definition_count);
        code += strlen(token.value);
        TokenList* new_tokens = (TokenList*) malloc(sizeof(TokenList));
        new_tokens->token = token;
        new_tokens->next = tokens;
        tokens = new_tokens;
    }
    tokens;
}
