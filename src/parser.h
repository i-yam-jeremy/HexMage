//
//  parser.h
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#ifndef ____parser__
#define ____parser__

#include "lexer.h"

struct ast_node_list;

typedef struct ast_node {
    struct ast_node_list* children;
    char* name;
    void* data;
} ASTNode;

typedef struct ast_node_list {
    ASTNode node;
    struct ast_node_list* next;
} ASTNodeList;

ASTNode parse(TokenList* tokens);

#endif /* defined(____parser__) */
