//
//  parser.c
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#include "parser.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define TYPE_ASTNODE 1
#define TYPE_TOKEN 2

typedef struct node {
    int type;
    union {
        ASTNode node;
        Token token;
    };
} Node;

typedef struct node_list {
    Node value;
    struct node_list* next;
} NodeList;

// Returns a pointer containing the ASTNode data if a match and returns 0 (null pointer) otherwise
typedef void* (*GrammarRuleCheckFunction)(NodeList* tokens);

typedef struct grammar_rule {
    char* name;
    GrammarRuleCheckFunction check;
} GrammarRule;

GrammarRule gr(char* name, GrammarRuleCheckFunction check) {
    GrammarRule rule;
    rule.name = name;
    rule.check = check;
    return rule;
}

void* GR_number(NodeList* nodes) {
    if (nodes != 0) {
        if (nodes->value.type == TYPE_TOKEN) {
            Token token = nodes->value.token;
            if (strcmp(token.name, "number") == 0) {
                return token.value;
            }
        }
    }
    return 0;
}

GrammarRule* get_grammar_rules(int* grammar_rule_count) {
    GrammarRule GRAMMAR_RULES[] = {
        gr("number", GR_number)
    };
    GrammarRule* grammar_rules = (GrammarRule*) malloc(sizeof(GRAMMAR_RULES));
    memcpy(grammar_rules, GRAMMAR_RULES, sizeof(GRAMMAR_RULES));
    *grammar_rule_count = sizeof(GRAMMAR_RULES)/sizeof(GrammarRule);
    return grammar_rules;
}

NodeList* to_node_list(TokenList* tokens) {
    if (tokens == 0) {
        return 0;
    }
    else {
        NodeList* nodes = (NodeList*) malloc(sizeof(NodeList));
        
        Node node;
        node.type = TYPE_TOKEN;
        node.token = tokens->token;
        
        nodes->node = node;
        nodes->next = to_node_list(tokens->next);
        
        return nodes;
    }
}

int node_list_length(NodeList* nodes) {
    int length = 0;
    
    while (nodes != 0) {
        nodes = nodes->next;
        length++;
    }
    
    return length;
}

ASTNode parse_grammar(TokenList* tokens, GrammarRule* grammar_rules, int grammar_rule_count) {
    
    NodeList* nodes = to_node_list(tokens);
    
    while ((nodes->next != 0) || (nodes->next.value.type != TYPE_ASTNODE)) {
        for (int i = 0; i < node_list_length(nodes); i++) {
            for (int j = 0; j < node_list_length(nodes) - i; j++) {
                /*
                 var matchWindow = nodes.slice(i, i+j+1);
                 for (ruleName in GRAMMAR_RULES) {
                    var obj = checkGrammarRule(ruleName, matchWindow);
                    if (obj != false) {
                        nodes = nodes.slice(0, i).concat(nodes.slice(i+j+1));
                        nodes.splice(i, 0, new ASTNode(ruleName, obj));
                        break;
                    }
                 }
                 */
            }
        }
    }
    
    return node;
}

ASTNode parse(TokenList* tokens) {
    int grammar_rule_count;
    GrammarRule* grammar_rules = get_grammar_rules(&grammar_rule_count);
    return parse_grammar(tokens, grammar_rules, grammar_rule_count);
}