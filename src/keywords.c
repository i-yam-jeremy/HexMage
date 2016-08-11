//
//  keywords.c
//  
//
//  Created by Jeremy Hanlon on 8/10/16.
//
//

#include "keywords.h"

#include <string.h>

bool is_keyword(char* str) {
    char* keywords[] = {
        "class"
    };
    for (int i = 0; i < sizeof(keywords)/sizeof(char*); i++) {
        if (strcmp(keywords[i], str) == 0) {
            return true;
        }
    }
    return false;
}