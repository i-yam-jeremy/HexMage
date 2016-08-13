//
//  lexer.h
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#ifndef ____lexer__
#define ____lexer__

typedef enum token_type {
    identifier,
    number,
    whitespace,
    new_line,
    l_curly,
    r_curly,
    l_square,
    r_square,
    l_paren,
    r_paren,
    plus,
    dash,
    star,
    forward_slash,
    percent,
    r_arrow,
    l_arrow,
    equals,
    dot,
    comma,
    semicolon,
    colon
} TokenType;

typedef struct token {
    TokenType type;
    char* value;
    int size;
} Token;

typedef struct token_list {
    Token token;
    struct token_list* next;
} TokenList;

TokenList* lex(char* code);

#endif /* defined(____lexer__) */
