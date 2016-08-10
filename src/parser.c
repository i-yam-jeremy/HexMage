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

// Returns true iff the grammar rule matches the nodes in the match window
typedef bool (*GrammarRuleCheckFunction)(NodeList* match_window, int match_window_size);

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

/*
 
 Token(number)
 
*/
bool GR_number(NodeList* match_window, int match_window_size) {
    if (match_window_size == 1) {
        if (match_window->node.type == TYPE_TOKEN) {
            Token token = match_window->node.token;
            if (strcmp(token.name, "number") == 0) {
                return true;
            }
        }
    }
    return false;
}

/*
 
 ASTNode(expression) Token(operator_token_name) ASTNode(expression)
 
*/
bool grammar_rule_binary_operator(NodeList* match_window, int match_window_size, char* operator_token_name) {
    if (match_window_size == 3) {
        if ((match_window->node.type == TYPE_ASTNODE) &&
            (strcmp(match_window->node.ast_node.name, "expression") == 0) &&
            (match_window->next->node.type == TYPE_TOKEN) &&
            (strcmp(match_window->next->node.token.name, operator_token_name) == 0) &&
            (match_window->next->next->node.type == TYPE_ASTNODE) &&
            (strcmp(match_window->next->next->node.ast_node.name, "expression") == 0)) {
            return true;
        }
    }
    return false;
}

/*
 
 ASTNode(expression) Token(+) ASTNode(expression)
 
 */
bool GR_addition_expr(NodeList* match_window, int match_window_size) {
    return grammar_rule_binary_operator(match_window, match_window_size, "+");
}

/*
 
 ASTNode(expression) Token(-) ASTNode(expression)
 
 */
bool GR_subtraction_expr(NodeList* match_window, int match_window_size) {
    return grammar_rule_binary_operator(match_window, match_window_size, "-");
}

/*
 
 ASTNode(expression) Token(*) ASTNode(expression)
 
 */
bool GR_multiplication_expr(NodeList* match_window, int match_window_size) {
    return grammar_rule_binary_operator(match_window, match_window_size, "*");
}

/*
 
 ASTNode(expression) Token(/) ASTNode(expression)
 
 */
bool GR_division_expr(NodeList* match_window, int match_window_size) {
    return grammar_rule_binary_operator(match_window, match_window_size, "/");
}

/*
 
 ASTNode(expression) Token(%) ASTNode(expression)
 
 */
bool GR_modulus_expr(NodeList* match_window, int match_window_size) {
    return grammar_rule_binary_operator(match_window, match_window_size, "%");
}

/*
 
 ASTNode(number|addition_expr|subtraction_expr|multiplication_expr|divionsion_expr|modulus_expr)
 
 */
bool GR_expression(NodeList* match_window, int match_window_size) {
    char* types[] = {
        "number",
        "addition_expr",
        "subtraction_expr",
        "multiplication_expr",
        "division_expr",
        "modulus_expr"
    };
    if (match_window_size == 1) {
        if (match_window->node.type == TYPE_ASTNODE) {
            for (int i = 0; i < sizeof(types)/sizeof(char*); i++) {
                if (strcmp(types[i], match_window->node.ast_node.name)) {
                    return true;
                }
            }
        }
    }
    return false;
}


GrammarRule* get_grammar_rules(int* grammar_rule_count) {
    GrammarRule GRAMMAR_RULES[] = {
        gr("number", GR_number),
        gr("addition_expr", GR_addition_expr),
        gr("subtraction_expr", GR_subtraction_expr),
        gr("multiplication_expr", GR_multiplication_expr),
        gr("division_expr", GR_division_expr),
        gr("modulus_expr", GR_modulus_expr),
        gr("expression", GR_expression)
    };
    GrammarRule* grammar_rules = (GrammarRule*) malloc(sizeof(GRAMMAR_RULES));
    memcpy(grammar_rules, GRAMMAR_RULES, sizeof(GRAMMAR_RULES));
    *grammar_rule_count = sizeof(GRAMMAR_RULES)/sizeof(GrammarRule);
    return grammar_rules;
}

NodeList* to_node_list(NodeList* prev, TokenList* tokens) {
    if (tokens == 0) {
        return 0;
    }
    else {
        NodeList* nodes = (NodeList*) malloc(sizeof(NodeList));
        
        Node node;
        node.type = TYPE_TOKEN;
        node.token = tokens->token;
        
        nodes->node = node;
        nodes->prev = prev;
        nodes->next = to_node_list(nodes, tokens->next);
        
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

NodeList* remove_nodes(NodeList* nodes, int items_to_remove) {
    
    NodeList* current = nodes;
    
    while ((current != 0) && (items_to_remove > 0)) {
        current = current->next;
        items_to_remove--;
    }
    
    if (nodes->prev) {
        nodes->prev->next = current;
        return 0; // If removing from not the beginning of list (which means there are previous items), just link items together
    }
    else {
        return current; // If removing from beginning of list, the list start pointer changes, so return the list start pointer
    }
}

void print_node_list(NodeList* nodes) {
    while (nodes != 0) {
        Node node = nodes->node;
        if (node.type == TYPE_ASTNODE) {
            ASTNode ast_node = node.ast_node;
            printf("ASTNode{'%s'}\n", ast_node.name);
        }
        else if (node.type == TYPE_TOKEN){
            Token token = node.token;
            printf("Token{'%s', '%.*s'}\n", token.name, token.size, token.value);
        }
        
        nodes = nodes->next;
    }
}

ASTNode parse_grammar(TokenList* tokens, GrammarRule* grammar_rules, int grammar_rule_count) {
    
    printf("Before converting to NodeList\n");
    NodeList* nodes = to_node_list(0, tokens);
    printf("Done converting to NodeList\n");
    
    NodeList* current = nodes;
    
    while ((nodes->next != 0) || (nodes->node.type != TYPE_ASTNODE)) {
        printf("--------------------------------\n");
        print_node_list(nodes);
        printf("--------------------------------\n");
        for (int i = 0; i < node_list_length(nodes); i++) {
            for (int j = 1; j < node_list_length(nodes) - i; j++) {
                //printf("i: %d, j: %d, nodes: %d\n", i, j, node_list_length(nodes));
                int match_window_size = j;
                NodeList* match_window_start = current;
                
                for (int g = 0; g < grammar_rule_count; g++) {
                    GrammarRule rule = grammar_rules[g];
                    bool match = rule.check(match_window_start, match_window_size);
                    
                    if (match) {
                        NodeList* n = remove_nodes(match_window_start, match_window_size);
                        if (n != 0) {
                            nodes = n;
                        }
                        
                        ASTNode ast_node;
                        ast_node.name = rule.name;
                        ast_node.children = match_window_start;
                        ast_node.child_count = match_window_size;
                        
                        Node node;
                        node.type = TYPE_ASTNODE;
                        node.ast_node = ast_node;
                        
                        NodeList* new_nodes = (NodeList*) malloc(sizeof(NodeList));
                        new_nodes->node = node;
                        new_nodes->prev = 0;
                        new_nodes->next = 0;
                        
                        if (match_window_start->prev == 0) { // match window is at start of list
                            new_nodes->prev = 0;
                            new_nodes->next = nodes;
                            nodes = new_nodes;
                            //printf("nodes: %d\n", node_list_length(new_nodes));
                        }
                        else {
                            new_nodes->prev = match_window_start->prev;
                            
                            if (new_nodes->prev->next != 0) {
                                new_nodes->prev->next->prev = new_nodes;
                                new_nodes->next = new_nodes->prev->next;
                            }
                            
                            new_nodes->prev->next = new_nodes;
                        }
                    }
                }
                current = current->next;
            }
            current = nodes;
        }
    }
    
    return nodes->node.ast_node;
}

ASTNode parse(TokenList* tokens) {
    int grammar_rule_count;
    GrammarRule* grammar_rules = get_grammar_rules(&grammar_rule_count);
    return parse_grammar(tokens, grammar_rules, grammar_rule_count);
}