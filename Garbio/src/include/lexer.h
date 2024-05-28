#ifndef LEXER_H
#define LEXER_H
#include "token.h"

typedef struct LEXER_STRUCT
{
    char c;
    unsigned int index;
    char* contents; // The source code that will be parsed
} lexer_T;

// Take contents from a source (like a file)
lexer_T* init_lexer(char* contents);

// To move the pointer to the next character in the content
// It will stop at the last character
void lexer_advance(lexer_T* lexer);

void lexer_skip_whitespace(lexer_T* lexer);

token_T* lexer_get_next_token(lexer_T* lexer);

// This will define how to parse a string
token_T* lexer_collect_string(lexer_T* lexer);

token_T* lexer_collect_id(lexer_T* lexer);

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token);

// This will convert char c into a string
char* lexer_get_current_char_as_string(lexer_T* lexer);

// This will define how to parse a number
token_T* lexer_collect_number(lexer_T* lexer);

// To skip comments
void lexer_skip_comment(lexer_T* lexer);

#endif