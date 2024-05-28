#ifndef TOKEN_H
#define TOKEN_H

typedef struct TOKEN_STRUCT
{
    enum
    {
        TOKEN_ID,
        TOKEN_EQUALS,
        TOKEN_STRING,
        TOKEN_SEMI,
        TOKEN_LEFTPAREN,
        TOKEN_RIGHTPAREN,
        TOKEN_LEFTBRACE,
        TOKEN_RIGHTBRACE,
        TOKEN_COMMA,
        TOKEN_EOF,
        TOKEN_TRUE,
        TOKEN_FALSE,
        TOKEN_IF,
        TOKEN_ELSEIF,
        TOKEN_ELSE,
        TOKEN_NUMBER,
        TOKEN_PLUS,
        TOKEN_MINUS,
        TOKEN_MUL,
        TOKEN_DIV
    } type;

    char* value;
} token_T;

token_T* init_token(int type, char* value);

#endif