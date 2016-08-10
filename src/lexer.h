//
//  lexer.h
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#ifndef ____lexer__
#define ____lexer__

typedef struct token {
    char* name;
    char* value;
    int size;
} Token;

typedef struct tokenlist {
    Token token;
    struct tokenlist* next;
} TokenList;

TokenList* lex(char* code);

#endif /* defined(____lexer__) */
