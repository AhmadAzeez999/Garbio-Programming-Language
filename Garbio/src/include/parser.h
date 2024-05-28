#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "AST.h"
#include "scope.h"

typedef struct PARSER_STRUCT
{
    lexer_T* lexer;
    token_T* current_token;
    token_T* prev_token;
    scope_T* scope; // Global scope

} parser_T;

parser_T* init_parser(lexer_T* lexer);

void parser_eat(parser_T* parser, int token_type);

AST_T* parser_parse(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope);

AST_T* parser_parse_expression(parser_T* parser, scope_T* scope);

AST_T* parser_parse_binop(parser_T* parser, scope_T* scope);

AST_T* parser_parse_primary(parser_T* parser, scope_T* scope);

AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope);

AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope);

AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope);

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope);

AST_T* parser_parse_string(parser_T* parser, scope_T* scope);

AST_T* parser_parse_id(parser_T* parser, scope_T* scope);

AST_T* parser_parse_boolean(parser_T* parser, scope_T* scope, int value);

AST_T* parser_parse_if(parser_T* parser, scope_T* scope);

AST_T* parser_parse_number(parser_T* parser, scope_T* scope);

#endif