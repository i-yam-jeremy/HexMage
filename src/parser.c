//
//  parser.c
//  
//
//  Created by Jeremy Hanlon on 8/9/16.
//
//

#include "parser.h"

#include "keywords.h"

#include <stdio.h>
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
 
 ASTNode(number|addition_expr|subtraction_expr|multiplication_expr|divionsion_expr|modulus_expr|function_call_expr)
 
 */
bool GR_expression(NodeList* match_window, int match_window_size) {
    char* types[] = {
        "number",
        "addition_expr",
        "subtraction_expr",
        "multiplication_expr",
        "division_expr",
        "modulus_expr",
        "variable_expr",
        "function_call_expr"
    };
    if (match_window_size == 1) {
        if (match_window->node.type == TYPE_ASTNODE) {
            for (int i = 0; i < sizeof(types)/sizeof(char*); i++) {
                if (strcmp(types[i], match_window->node.ast_node.name) == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

/*
 
 Token(identifier)
 
 */
bool GR_keyword(NodeList* match_window, int match_window_size) {
    if (match_window_size == 1) {
        if (match_window->node.type == TYPE_TOKEN) {
            Token token = match_window->node.token;
            if (strcmp(token.name, "identifier") == 0) {
                char* value = (char*) malloc(token.size+1);
                memcpy(value, token.value, token.size);
                value[token.size] = '\0';
                return is_keyword(value);
            }
        }
    }
    return false;
}

/*
 
 Token(identifier)

*/
bool GR_variable_expr(NodeList* match_window, int match_window_size) {
    if (match_window_size == 1) {
        if (match_window->node.type == TYPE_TOKEN) {
            Token token = match_window->node.token;
            if (strcmp(token.name, "identifier") == 0) {
                char* value = (char*) malloc(token.size+1);
                memcpy(value, token.value, token.size);
                value[token.size] = '\0';
                if (! is_keyword(value)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/*
 
 Token("(") ASTNode(expression) Token(")")
 
 */
bool GR_parenthesis_expr(NodeList* match_window, int match_window_size) {
    if (match_window_size == 3) {
        if ((match_window->node.type == TYPE_TOKEN) &&
            (strcmp(match_window->node.token.name, "(") == 0) &&
            (match_window->next->node.type == TYPE_ASTNODE) &&
            (strcmp(match_window->next->node.ast_node.name, "expression") == 0) &&
            (match_window->next->next->node.type == TYPE_TOKEN) &&
            (strcmp(match_window->next->next->node.token.name, ")") == 0)) {
            return true;
        }
    }
    return false;
}

/*
 
 function=ASTNode(expression) Token("(") ( ASTNode(expression) (Token(,) ASTNode(expression))* )? Token(")")
 
*/
bool GR_function_call_expr(NodeList* match_window, int match_window_size) {
    if (match_window_size >= 3) {
        NodeList* nodes = match_window;
        if ((nodes->node.type == TYPE_ASTNODE) && (strcmp(match_window->node.ast_node.name, "expression") == 0)) {
            nodes = nodes->next;
            if ((nodes->node.type == TYPE_TOKEN) && (strcmp(nodes->node.token.name, "(") == 0)) {
                nodes = nodes->next;
                int arg_node_count = (match_window_size - 3); // - 3 because function expression, open paren, and close paren
                int arg_count = (arg_node_count / 2) + (arg_node_count % 2);
                
                for (int i = 0; i < arg_count; i++) {
                    if (!((nodes->node.type == TYPE_ASTNODE) && (strcmp(nodes->node.ast_node.name, "expression") == 0))) {
                        return false;
                    }
                    nodes = nodes->next;
                    if (i != arg_count-1) { // if not last argument
                        if (!((nodes->node.type == TYPE_TOKEN) && (strcmp(nodes->node.token.name, ",") == 0))) {
                            return false;
                        }
                        nodes = nodes->next;
                    }
                }
                
                if ((nodes->node.type == TYPE_TOKEN) && (strcmp(nodes->node.token.name, ")") == 0)) {
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
        gr("keyword", GR_keyword),
        gr("variable_expr", GR_variable_expr),
        gr("parenthesis_expr", GR_parenthesis_expr),
        gr("function_call_expr", GR_function_call_expr),
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

void remove_nodes(NodeList** nodes, NodeList* sublist, int items_to_remove) {
    
    NodeList* current = sublist;
    
    while ((current != 0) && (items_to_remove > 0)) {
        current = current->next;
        items_to_remove--;
    }
    
    if (sublist->prev) {
        sublist->prev->next = current; // If removing from not the beginning of list (which means there are previous items), just link items together
    }
    else {
        *nodes = current; // If removing from beginning of list, the list start pointer changes, so change the list start pointer
    }
}

void print_node(Node node, int indentation);

void print_ast_node(ASTNode node, int indentation) {
    printf("ASTNode{'%s'}\n", node.name);
    NodeList* current_child = node.children;
    for (int i = 0; i < node.child_count; i++) {
        print_node(current_child->node, indentation+1);
        current_child = current_child->next;
    }
}

void print_token(Token token) {
    printf("Token{'%s', '%.*s'}\n", token.name, token.size, token.value);
}

void print_node(Node node, int indentation) {
    for (int i = 0; i < indentation; i++) {
        printf("\t");
    }
    if (node.type == TYPE_ASTNODE) {
        ASTNode ast_node = node.ast_node;
        print_ast_node(ast_node, indentation);
    }
    else if (node.type == TYPE_TOKEN){
        Token token = node.token;
        print_token(token);
    }
}

void print_node_list(NodeList* nodes) {
    printf("-------------------------------\n");
    while (nodes != 0) {
        Node node = nodes->node;
        print_node(node, 0);
        nodes = nodes->next;
    }
    printf("-------------------------------\n");
}

bool check_rules(NodeList** nodes, NodeList* current, int i, int j, GrammarRule* grammar_rules, int grammar_rule_count) {
    int match_window_size = j;
    NodeList* match_window_start = current;
    
    for (int g = 0; g < grammar_rule_count; g++) {
        GrammarRule rule = grammar_rules[g];
        bool match = rule.check(match_window_start, match_window_size);
        
        if (match) {
            remove_nodes(nodes, match_window_start, match_window_size);
            
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
                new_nodes->next = *nodes;
                *nodes = new_nodes;
            }
            else {
                new_nodes->prev = match_window_start->prev;
                
                if (new_nodes->prev->next != 0) {
                    new_nodes->prev->next->prev = new_nodes;
                    new_nodes->next = new_nodes->prev->next;
                }
                
                new_nodes->prev->next = new_nodes;
            }
            return true;
        }
    }
    return false;
}

ASTNode parse_grammar(TokenList* tokens, GrammarRule* grammar_rules, int grammar_rule_count) {
    
    NodeList* nodes = to_node_list(0, tokens);
    
    NodeList* current = nodes;
    
    while ((nodes->next != 0) || (nodes->node.type != TYPE_ASTNODE) || (strcmp(nodes->node.ast_node.name, "expression") != 0)) {
        bool list_changed = false;
        //print_node_list(nodes);
        for (int i = 0; i < node_list_length(nodes); i++) {
            for (int j = 1; j <= node_list_length(nodes) - i; j++) {
                bool match = check_rules(&nodes, current, i, j, grammar_rules, grammar_rule_count);
                if (match) {
                    //printf("Matched: i: %d, j: %d\n", i, j);
                    list_changed = true;
                }
            }
            current = current->next;
        }
        current = nodes;
        
        if (!list_changed) {
            fprintf(stderr, "Error: could not parse.\n");
            print_node_list(nodes);
            fprintf(stderr, "Error: could not parse.\n");
            exit(1);
        }
    }
    
    print_node_list(nodes);
    
    return nodes->node.ast_node;
}

ASTNode parse(TokenList* tokens) {
    int grammar_rule_count;
    GrammarRule* grammar_rules = get_grammar_rules(&grammar_rule_count);
    return parse_grammar(tokens, grammar_rules, grammar_rule_count);
}