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
    ASTNodeType type;
    GrammarRuleCheckFunction check;
} GrammarRule;

GrammarRule gr(ASTNodeType type, GrammarRuleCheckFunction check) {
    GrammarRule rule;
    rule.type = type;
    rule.check = check;
    return rule;
}

/*
 
 Checks if an ASTNode is a given type including recursively (if an ASTNode has one child, it is a wrapper around it's only child, so it is essentially the same node. For example, a number literal is technically an expression, but the tree looks like this or something similar
            expression ->
                add_expr ->
                    mult_expr ->
                        atom_expr ->
                            Token(number)
 
 */

bool is_type_recursive(ASTNode node, ASTNodeType type) {
    //printf("ASTNode{'%d'}\n", node.type);
    if (node.child_count == 1) {
        //printf("\tASTNode{'%d'}\n", node.type);
        if (node.children->node.type == TYPE_ASTNODE) {
            if (node.children->node.ast_node.type == type) {
                return true;
            }
            else {
                return is_type_recursive(node.children->node.ast_node, type);
            }
        }
    }
    
    return false;
}

/*
 
 // A number literal, variable, or an expression in parentheses.
 
 Token(number|identifier) | ASTNode(paren_expr)
 
*/
bool GR_atom_expr(NodeList* match_window, int match_window_size) {
    if (match_window_size == 1) {
        if (match_window->node.type == TYPE_TOKEN) {
            Token token = match_window->node.token;
            if ((token.type == number) || (token.type == identifier)) {
                return true;
            }
        }
        else if (match_window->node.type == TYPE_ASTNODE) {
            ASTNode node = match_window->node.ast_node;
            if (node.type == paren_expr) {
                return true;
            }
            /*if (!is_type_recursive(node, atom_expr)) {
                printf("Hi\n");
                if (node.type == expression) {
                    return true;
                }
            }*/
        }
    }
    return false;
}

/*
 
 ASTNode(expression_subtype) Token(operator_token_type) ASTNode(expression_subtype)
 
*/
bool grammar_rule_binary_operator(NodeList* match_window, int match_window_size, TokenType operator_token_type, ASTNodeType* expression_subtypes, int expression_subtype_count) {
    if (match_window_size == 3) {
        for (int i = 0; i < expression_subtype_count; i++) {
            ASTNodeType expression_subtype = expression_subtypes[i];
            if ((match_window->node.type == TYPE_ASTNODE) &&
                (match_window->node.ast_node.type == expression_subtype) &&
                (match_window->next->node.type == TYPE_TOKEN) &&
                (match_window->next->node.token.type == operator_token_type) &&
                (match_window->next->next->node.type == TYPE_ASTNODE) &&
                (match_window->next->next->node.ast_node.type == expression_subtype)) {
                return true;
            }
        }
    }
    else if (match_window_size == 1) {
        for (int i = 0; i < expression_subtype_count; i++) {
            ASTNodeType expression_subtype = expression_subtypes[i];
            if ((match_window->node.type == TYPE_ASTNODE) && (match_window->node.ast_node.type == expression_subtype)) {
                return true;
            }
        }
    }
    return false;
}

/*
 
 ASTNode(expression) Token(+) ASTNode(expression)
 
 */
bool GR_add_expr(NodeList* match_window, int match_window_size) {
    ASTNodeType expression_subtypes[] = {
        mult_expr,
        div_expr,
        mod_expr
    };
    return grammar_rule_binary_operator(match_window, match_window_size, plus, expression_subtypes, sizeof(expression_subtypes)/sizeof(ASTNodeType));
}
                
/*
 
 ASTNode(expression) Token(-) ASTNode(expression)
 
 */
bool GR_sub_expr(NodeList* match_window, int match_window_size) {
    ASTNodeType expression_subtypes[] = {
        mult_expr,
        div_expr,
        mod_expr
    };
    return grammar_rule_binary_operator(match_window, match_window_size, dash, expression_subtypes, sizeof(expression_subtypes)/sizeof(ASTNodeType));
}

/*
 
 ASTNode(expression) Token(*) ASTNode(expression)
 
 */
bool GR_mult_expr(NodeList* match_window, int match_window_size) {
    ASTNodeType expression_subtypes[] = {
        /*variable_expr,
        number_literal_expr,
        parentheses_expr*/
        atom_expr
    };
    bool result = grammar_rule_binary_operator(match_window, match_window_size, star, expression_subtypes, sizeof(expression_subtypes)/sizeof(ASTNodeType));
    return result;
}

/*
 
 ASTNode(expression) Token(/) ASTNode(expression)
 
 */
bool GR_div_expr(NodeList* match_window, int match_window_size) {
    ASTNodeType expression_subtypes[] = {
        /*variable_expr,
         number_literal_expr,
         parentheses_expr*/
        atom_expr
    };
    return grammar_rule_binary_operator(match_window, match_window_size, forward_slash, expression_subtypes, sizeof(expression_subtypes)/sizeof(ASTNodeType));
}

/*
 
 ASTNode(expression) Token(%) ASTNode(expression)
 
 */
bool GR_mod_expr(NodeList* match_window, int match_window_size) {
    ASTNodeType expression_subtypes[] = {
        /*variable_expr,
         number_literal_expr,
         parentheses_expr*/
        atom_expr
    };
    return grammar_rule_binary_operator(match_window, match_window_size, percent, expression_subtypes, sizeof(expression_subtypes)/sizeof(ASTNodeType));
}

/*
 
 ASTNode(number|addition_expr|subtraction_expr|multiplication_expr|divionsion_expr|modulus_expr|function_call_expr)
 
 */
bool GR_expression(NodeList* match_window, int match_window_size) {
    ASTNodeType types[] = {
        add_expr,
        sub_expr
    };
    if (match_window_size == 1) {
        if (match_window->node.type == TYPE_ASTNODE) {
            for (int i = 0; i < sizeof(types)/sizeof(char*); i++) {
                if (types[i] == match_window->node.ast_node.type) {
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
/*bool GR_keyword(NodeList* match_window, int match_window_size) {
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
}*/

/*
 
 Token(identifier)

*/
/*bool GR_variable_expr(NodeList* match_window, int match_window_size) {
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
}*/

/*
 
 Token("(") ASTNode(expression) Token(")")
 
 */
bool GR_paren_expr(NodeList* match_window, int match_window_size) {
    if (match_window_size == 3) {
        if ((match_window->node.type == TYPE_TOKEN) &&
            (match_window->node.token.type == l_paren) &&
            (match_window->next->node.type == TYPE_ASTNODE) &&
            (match_window->next->node.ast_node.type == expression) &&
            (match_window->next->next->node.type == TYPE_TOKEN) &&
            (match_window->next->next->node.token.type == r_paren)) {
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
        if ((nodes->node.type == TYPE_ASTNODE) && (match_window->node.ast_node.type == expression)) {
            nodes = nodes->next;
            if ((nodes->node.type == TYPE_TOKEN) && (nodes->node.token.type == r_paren)) {
                nodes = nodes->next;
                int arg_node_count = (match_window_size - 3); // - 3 because function expression, open paren, and close paren
                int arg_count = (arg_node_count / 2) + (arg_node_count % 2);
                
                for (int i = 0; i < arg_count; i++) {
                    if (!((nodes->node.type == TYPE_ASTNODE) && (nodes->node.ast_node.type == expression))) {
                        return false;
                    }
                    nodes = nodes->next;
                    if (i != arg_count-1) { // if not last argument
                        if (!((nodes->node.type == TYPE_TOKEN) && (nodes->node.token.type == comma))) {
                            return false;
                        }
                        nodes = nodes->next;
                    }
                }
                
                if ((nodes->node.type == TYPE_TOKEN) && (nodes->node.token.type == r_paren)) {
                    return true;
                }
            }
        }
    }
    return false;
}


GrammarRule* get_grammar_rules(int* grammar_rule_count) {
    GrammarRule GRAMMAR_RULES[] = {
        gr(atom_expr, GR_atom_expr),
        gr(mult_expr, GR_mult_expr),
        gr(div_expr, GR_div_expr),
        gr(mod_expr, GR_mod_expr),
        gr(add_expr, GR_add_expr),
        gr(sub_expr, GR_sub_expr),
        /*gr("keyword", GR_keyword),
        gr("variable_expr", GR_variable_expr),
        gr("function_call_expr", GR_function_call_expr),z*/
        gr(paren_expr, GR_paren_expr),
        gr(expression, GR_expression)
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
    printf("ASTNode{'%d'}\n", node.type);
    NodeList* current_child = node.children;
    for (int i = 0; i < node.child_count; i++) {
        print_node(current_child->node, indentation+1);
        current_child = current_child->next;
    }
}

void print_token(Token token) {
    printf("Token{'%d', '%.*s'}\n", token.type, token.size, token.value);
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
            ast_node.type = rule.type;
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
    
    bool forward_pass = true;
    
    while ((nodes->next != 0) || (nodes->node.type != TYPE_ASTNODE) || (nodes->node.ast_node.type == expression)) {
        bool list_changed = false;
        print_node_list(nodes);
        if (forward_pass) {
            printf("forward\n");
        }
        else {
            printf("backward\n");
        }
        for (int i = 0; i < node_list_length(nodes); i++) {
            for (int j = 1; j <= node_list_length(nodes) - i; j++) {
                
                if (!forward_pass) {
                    for (int n = 0; n < j-1; n++) {
                        current = current->prev;
                    }
                }
                
                bool match = check_rules(&nodes, current, i, j, grammar_rules, grammar_rule_count);
                if (match) {
                    //printf("Matched: i: %d, j: %d\n", i, j);
                    list_changed = true;
                }
                
                if (!forward_pass) {
                    for (int n = 0; n < j-1; n++) {
                        current = current->next;
                    }
                }
            }
            if (forward_pass) {
                if (current->next != 0) {
                    current = current->next;
                }
            }
            else {
                if (current->prev) {
                    current = current->prev;
                }
            }
        }
        forward_pass = !forward_pass;
        //current = nodes;
        
        if (!list_changed) {
            fprintf(stderr, "Error: could not parse.\n");
            print_node_list(nodes);
            fprintf(stderr, "Error: could not parse.\n");
            int length = node_list_length(nodes);
            for (int i = 0; i < length; i++) {
                if (nodes->node.type == TYPE_ASTNODE) {
                    if (is_type_recursive(nodes->node.ast_node, atom_expr)) {
                        printf("true\n");
                    }
                    else {
                        printf("false\n");
                    }
                }
                nodes = nodes->next;
            }
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