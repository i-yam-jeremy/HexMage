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
    TokenType type;
    regex_t regex;
} TokenDef;

TokenDef td(TokenType type,  char* regex) {
    TokenDef def;
    def.type = type;
    int err = regcomp(&def.regex, regex, 0);
    
    if (err) {
        fprintf(stderr, "Could not compile regex for token definition '%d'.\n", type);
        exit(1);
    }
    
    return def;
}

TokenDef* get_token_definitions(int* token_definition_count) {
    TokenDef TOKEN_DEFS[] = {
        td(identifier, "[A-Za-z][A-Za-z0-9_]*"),
        td(number, "[0-9][0-9]*\\.*[0-9]*"), // also matches "1...0" which I don't think will cause any problems, but it looks very weird
        td(whitespace, "[ \t][ \t]*"),
        td(new_line, "\n"),
        td(l_curly, "{"),
        td(r_curly, "}"),
        td(l_square, "\\["),
        td(r_square, "\\]"),
        td(l_paren, "("),
        td(r_paren, ")"),
        td(plus, "\\+"),
        td(dash, "\\-"),
        td(star, "\\*"),
        td(forward_slash, "\\/"),
        td(percent, "\\%"),
        td(r_arrow, "\\>"),
        td(l_arrow, "\\<"),
        td(equals, "\\="),
        td(dot, "\\."),
        td(comma, "\\,"),
        td(semicolon, "\\;"),
        td(colon, "\\:")
    };
    TokenDef* token_definitions = (TokenDef*) malloc(sizeof(TOKEN_DEFS));
    memcpy(token_definitions, TOKEN_DEFS, sizeof(TOKEN_DEFS));
    *token_definition_count = sizeof(TOKEN_DEFS)/sizeof(TokenDef);
    return token_definitions;
}

Token get_next_token(char* code, TokenDef* token_definitions, int token_definition_count, int line, int column) {
    for (int i = 0; i < token_definition_count; i++) {
        TokenDef def = token_definitions[i];
        regmatch_t matches[1];
        int err = regexec(&def.regex, code, 1, matches, 0);
        if (!err) {
            regmatch_t match = matches[0];
            if (match.rm_so == 0) {
                Token t;
                t.type = def.type;
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
    
    fprintf(stderr, "Invalid token:%d:%d: \n", line, column);
    exit(1);
}

TokenList* lex(char* code) {
    int token_definition_count;
    TokenDef* token_definitions = get_token_definitions(&token_definition_count);
    TokenList* tokens = (TokenList*) malloc(sizeof(TokenList));
    tokens->next = 0;
    TokenList* current = tokens;
    
    int line = 1;
    int column = 1;
    
    while (code[0] != '\0') { // while (strlen(code) > 0) { // but more efficient
        Token token = get_next_token(code, token_definitions, token_definition_count, line, column);
        if (token.type == new_line) {
            line++;
            column = 0;
        }
        else {
            column += token.size;
        }
        code += token.size;
        if ((token.type != whitespace) && (token.type != new_line)) {
            current->next = (TokenList*) malloc(sizeof(TokenList));
            current->next->token = token;
            current->next->next = 0;
            current = current->next;
        }
    }
    
    return tokens->next;
}
