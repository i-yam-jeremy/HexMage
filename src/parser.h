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

#define TYPE_ASTNODE 1
#define TYPE_TOKEN 2

struct node_list;

typedef enum ast_node_type {
    atom_expr,
    mult_expr,
    div_expr,
    mod_expr,
    add_expr,
    sub_expr,
    paren_expr,
    expression
} ASTNodeType;

typedef struct ast_node {
    struct node_list* children;
    int child_count;
    ASTNodeType type;
} ASTNode;

typedef struct node {
    int type;
    union {
        ASTNode ast_node;
        Token token;
    };
} Node;

typedef struct node_list {
    Node node;
    struct node_list* next;
    struct node_list* prev;
} NodeList;

ASTNode parse(TokenList* tokens);

#endif /* defined(____parser__) */
