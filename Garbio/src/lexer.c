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

        if (lexer->c == '\'')
            return lexer_collect_char(lexer);

        switch (lexer->c)
        {
            case '=':
                if (lexer_peek(lexer, 1) == '=')
                {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    return init_token(TOKEN_EQEQ, "==");
                }
                
                return lexer_advance_with_token(lexer, init_token(TOKEN_EQUALS, lexer_get_current_char_as_string(lexer)));  

                break;

            case '!':
                if (lexer_peek(lexer, 1) == '=')
                {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    return init_token(TOKEN_NEQ, "!=");
                }

                break;

            case '<':
                if (lexer_peek(lexer, 1) == '=')
                {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    return init_token(TOKEN_LTE, "<=");
                }
                
                return lexer_advance_with_token(lexer, init_token(TOKEN_LT, lexer_get_current_char_as_string(lexer)));  

                break;

            case '>':
                if (lexer_peek(lexer, 1) == '=')
                {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    return init_token(TOKEN_GTE, ">=");
                }
                
                return lexer_advance_with_token(lexer, init_token(TOKEN_GT, lexer_get_current_char_as_string(lexer)));  

                break;

            case '|':
                if (lexer_peek(lexer, 1) == '|')
                {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    return init_token(TOKEN_OR, "||");
                }

                break;

            case '&':
                if (lexer_peek(lexer, 1) == '&')
                {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    return init_token(TOKEN_AND, "&&");
                }

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
            else if (strcmp(id_token->value, "while") == 0)
            {
                id_token->type = TOKEN_WHILE;
            }
            else if (strcmp(id_token->value, "and") == 0)
            {
                id_token->type = TOKEN_AND;
            }
            else if (strcmp(id_token->value, "or") == 0)
            {
                id_token->type = TOKEN_OR;
            }
            else if (strcmp(id_token->value, "not") == 0)
            {
                id_token->type = TOKEN_NEQ;
            }
            else if (strcmp(id_token->value, "return") == 0)
            {
                id_token->type = TOKEN_RETURN;
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
    int has_dot = 0;

    while(isdigit(lexer->c) || lexer->c == '.')
    {
        if (lexer->c == '.')
        {
            if (has_dot)
            {
                fprintf(stderr, "Syntax error: Multiple dots in number\n");
                exit(1);
            }

            has_dot = 1;
        }

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

char lexer_peek(lexer_T* lexer, int offset)
{
    size_t pos = lexer->index + offset;
    if (pos >= strlen(lexer->contents))
        return '\0'; // if out of bounds
    return lexer->contents[pos];
}

token_T* lexer_collect_char(lexer_T* lexer)
{
    lexer_advance(lexer); // to skip '

    // Allocating memory for char
    char* value = calloc(2, sizeof(char));

    value[0] = lexer->c;
    
    lexer_advance(lexer); // to move past captured character
    
    if (lexer->c != '\'')
    {
        fprintf(stderr, "Error: Unclosed character literal or char contains more than one character.\n");
        free(value);
        exit(1);
    }

    lexer_advance(lexer); // to skip closing '

    return init_token(TOKEN_CHAR, value);
}