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
#include <stdlib.h>

typedef struct tokendef {
    char* name;
    regex_t regex;
} TokenDef;

TokenDef td(char* name,  char* regex) {
    TokenDef def;
    def.name = name;
    int err = regcomp(&def.regex, regex, 0);
    
    if (err) {
        fprintf(stderr, "Could not compile regex for token definition '%s'.\n", name);
        exit(1);
    }
    
    return def;
}

TokenDef* get_token_definitions(int* token_definition_count) {
    TokenDef TOKEN_DEFS[] = {
        td("identifier", "[A-Za-z][A-Za-z0-9_]*"),
        td("number", "[0-9][0-9]*\\.*[0-9]*"), // also matches "1...0" which I don't think will cause any problems, but it looks very weird
        td("whitespace", "[ \t][ \t]*"),
        td("\n", "\n"),
        td("{", "{"),
        td("}", "}"),
        td("[", "\\["),
        td("]", "\\]"),
        td("(", "("),
        td(")", ")"),
        td("+", "\\+"),
        td("-", "\\-"),
        td("*", "\\*"),
        td("/", "\\/"),
        td("%", "\\%"),
        td(">", "\\>"),
        td("<", "\\<"),
        td("=", "\\="),
        td(".", "\\."),
        td(",", "\\,"),
        td(";", "\\;"),
        td(":", "\\:")
    };
    TokenDef* token_definitions = (TokenDef*) malloc(sizeof(TOKEN_DEFS));
    memcpy(token_definitions, TOKEN_DEFS, sizeof(TOKEN_DEFS));
    *token_definition_count = sizeof(TOKEN_DEFS)/sizeof(TokenDef);
    return token_definitions;
}

Token get_next_token(char* code, TokenDef* token_definitions, int token_definition_count) {
    Token t;
    
    for (int i = 0; i < token_definition_count; i++) {
        TokenDef def = token_definitions[i];
        regmatch_t matches[1];
        int err = regexec(&def.regex, code, 1, matches, 0);
        if (!err) {
            regmatch_t match = matches[0];
            if (match.rm_so == 0) {
                t.name = def.name;
                t.value = code;
                t.size = match.rm_eo - match.rm_so;
                return t;
            }
            else {
                continue;
            }
        }
        else if (err == REG_NOMATCH) {
            continue;
        }
        else {
            fprintf(stderr, "Regex error: %d\n", err);
        }
    }
    
    return t;
}

TokenList* lex(char* code) {
    int token_definition_count;
    TokenDef* token_definitions = get_token_definitions(&token_definition_count);
    TokenList* tokens = (TokenList*) malloc(sizeof(TokenList));
    TokenList* current = tokens;
    
    while (code[0] != '\0') { // while (strlen(code) > 0) // but more efficient
        Token token = get_next_token(code, token_definitions, token_definition_count);
        code += token.size;
        current->next = (TokenList*) malloc(sizeof(TokenList));
        current->next->token = token;
        current = current->next;
    }
    
    return tokens->next;
}
