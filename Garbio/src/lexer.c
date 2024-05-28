#include "include/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

lexer_T* init_lexer(char* contents)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->contents = contents;
    lexer->index = 0;
    lexer->c = contents[lexer->index];

    return lexer;
}

void lexer_advance(lexer_T* lexer)
{
    if (lexer->c != '\0' && lexer->index < strlen(lexer->contents))
    {
        lexer->index += 1;
        lexer->c = lexer->contents[lexer->index];
    }
}

void lexer_skip_whitespace(lexer_T* lexer)
{
    while(lexer->c == ' ' || lexer->c == '\n')
    {
        lexer_advance(lexer);
    }
}

token_T* lexer_get_next_token(lexer_T* lexer)
{
    while (lexer->c != '\0' && lexer->index < strlen(lexer->contents))
    {
        if (lexer->c == '#')
        {
            lexer_skip_comment(lexer);
            continue;
        }

        if (lexer->c == ' ' || lexer->c == 10)
            lexer_skip_whitespace(lexer);

        if (lexer->c == '"')
            return lexer_collect_string(lexer);

        if (isdigit(lexer->c))
            return lexer_collect_number(lexer);

        switch (lexer->c)
        {
            case '=': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_EQUALS, lexer_get_current_char_as_string(lexer)));  

                break;
            case ';': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_SEMI, lexer_get_current_char_as_string(lexer)));

                break;
            case '(': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_LEFTPAREN, lexer_get_current_char_as_string(lexer)));
            
            break;
            case ')': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_RIGHTPAREN, lexer_get_current_char_as_string(lexer)));
            
            break; 
            case '{': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_LEFTBRACE, lexer_get_current_char_as_string(lexer)));
            
            break;
            case '}': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_RIGHTBRACE, lexer_get_current_char_as_string(lexer)));
            
            break; 
            case ',': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_COMMA, lexer_get_current_char_as_string(lexer)));
            
            break;
            case '+': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_PLUS, lexer_get_current_char_as_string(lexer)));
            
            break;
            case '-': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_MINUS, lexer_get_current_char_as_string(lexer)));
            
            break;
            case '*': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_MUL, lexer_get_current_char_as_string(lexer)));
            
            break;
            case '/': 
                return lexer_advance_with_token(lexer, init_token(TOKEN_DIV, lexer_get_current_char_as_string(lexer)));
            
            break;
        }

        if (isalnum(lexer->c))
        {
            token_T* id_token = lexer_collect_id(lexer);

            if (strcmp(id_token->value, "true") == 0)
            {
                id_token->type = TOKEN_TRUE;
            }
            else if (strcmp(id_token->value, "false") == 0)
            {
                id_token->type = TOKEN_FALSE;
            }
            else if (strcmp(id_token->value, "if") == 0)
            {
                id_token->type = TOKEN_IF;
            }
            else if (strcmp(id_token->value, "elseif") == 0)
            {
                id_token->type = TOKEN_ELSEIF;
            }
            else if (strcmp(id_token->value, "else") == 0)
            {
                id_token->type = TOKEN_ELSE;
            }

            return id_token;
        }
    }

    return init_token(TOKEN_EOF, "\0");
}

token_T* lexer_collect_string(lexer_T* lexer)
{
    lexer_advance(lexer); // to skip "

    // Allocating memory for string
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';
    
    while (lexer->c != '"')
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }

    lexer_advance(lexer);

    return init_token(TOKEN_STRING, value);
}

token_T* lexer_collect_number(lexer_T* lexer)
{
    char* value = calloc(1, sizeof(char));
    while(isdigit(lexer->c))
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1));
        strcat(value, s);
        lexer_advance(lexer);
    }

    return init_token(TOKEN_NUMBER, value);
}

token_T* lexer_collect_id(lexer_T* lexer)
{
    // Allocating memory for string
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (isalnum(lexer->c))
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }

    return init_token(TOKEN_ID, value);
}

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);

    return token;
}

char* lexer_get_current_char_as_string(lexer_T* lexer)
{
    //printf("Char: '%c'\n", lexer->c);
    char* str = calloc(2, sizeof(char));
    str[0] = lexer->c;
    str[1] = '\0';

    return str;
}

void lexer_skip_comment(lexer_T* lexer)
{
    while (lexer->c != '\n')
    {
        lexer_advance(lexer);
    }

    // Advance past the newline character
    if (lexer->c == '\n')
    {
        lexer_advance(lexer);
    }
}