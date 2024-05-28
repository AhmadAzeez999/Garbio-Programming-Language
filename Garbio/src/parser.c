#include "include/parser.h"
#include "include/scope.h"
#include <stdio.h>
#include <string.h>

//  Helper method
// static scope_T* get_node_scope(parser_T* parser, AST_T* node)
// {
//     return node->scope == (void*)0 ? parser->scope : node->scope;
// }

parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->prev_token = parser->current_token;

    parser->scope = init_scope();

    return parser;
}

void parser_eat(parser_T* parser, int token_type)
{
    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else
    {
        printf("Unexpected token '%s', with type %d \n",
        parser->current_token->value,
        parser->current_token->type
        );
        exit(1);
    }
}

// Main entry point
AST_T* parser_parse(parser_T* parser, scope_T* scope)
{
    return parser_parse_statements(parser, scope);
}

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope)
{
    switch (parser->current_token->type)
    {
        case TOKEN_ID: 
            return parser_parse_id(parser, scope);
        case TOKEN_IF:
            return parser_parse_if(parser, scope);
    
        break;
    default:
        break;
    }

    return init_ast(AST_NOOP);
}

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* ast_statement = parser_parse_statement(parser, scope);
    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while (parser->current_token->type == TOKEN_SEMI)
    {
        parser_eat(parser, TOKEN_SEMI);

        AST_T* ast_statement = parser_parse_statement(parser, scope);

        if (ast_statement)
        {
            compound->compound_size += 1;
            compound->compound_value = realloc(
                compound->compound_value,
                compound->compound_size * sizeof(struct AST_STRUCT*)
            );
            compound->compound_value[compound->compound_size-1] = ast_statement;
        }
    }

    return compound;
}

AST_T* parser_parse_expression(parser_T* parser, scope_T* scope)
{
    return parser_parse_binop(parser, scope);
}

AST_T* parser_parse_binop(parser_T* parser, scope_T* scope)
{
    AST_T* binop_left = parser_parse_primary(parser, scope);

    while (
        parser->current_token->type == TOKEN_PLUS ||
        parser->current_token->type == TOKEN_MINUS ||
        parser->current_token->type == TOKEN_MUL ||
        parser->current_token->type == TOKEN_DIV
    )
    {
        int op = parser->current_token->type;
        parser_eat(parser, op);

        AST_T* binop_right = parser_parse_primary(parser, scope);

        AST_T* binop = init_ast(AST_BINOP);
        binop->binop_left = binop_left;
        binop->binop_right = binop_right;
        binop->op = op;
        binop->scope = scope;

        binop_left = binop;
    }

    return binop_left;
}

AST_T* parser_parse_primary(parser_T* parser, scope_T* scope)
{
    switch (parser->current_token->type)
    {
        case TOKEN_STRING: 
            return parser_parse_string(parser, scope);

        case TOKEN_ID: 
            return parser_parse_id(parser, scope);

        case TOKEN_TRUE: 
            return parser_parse_boolean(parser, scope, 1);

        case TOKEN_FALSE: 
            return parser_parse_boolean(parser, scope, 0);

        case TOKEN_NUMBER: 
            return parser_parse_number(parser, scope);
        
        case TOKEN_LEFTPAREN:
            parser_eat(parser, TOKEN_LEFTPAREN);
            AST_T* expression = parser_parse_expression(parser, scope);
            parser_eat(parser, TOKEN_RIGHTPAREN);

            return expression;
        
        default:
            break;
    }

    return init_ast(AST_NOOP);
}

AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope)
{
    AST_T* function_call = init_ast(AST_FUNCTION_CALL);

    function_call->function_call_name = parser->prev_token->value;
    parser_eat(parser, TOKEN_LEFTPAREN);

    if (parser->current_token->type != TOKEN_RIGHTPAREN) // Checks if there are any args
    {
        // Allocating memory for function call araguments
        function_call->function_call_arguments = calloc(1, sizeof(struct AST_STRUCT*));

        AST_T* ast_expression = parser_parse_expression(parser, scope);
        function_call->function_call_arguments[0] = ast_expression;
        function_call->function_call_arguments_size += 1;

        while (parser->current_token->type == TOKEN_COMMA)
        {
            parser_eat(parser, TOKEN_COMMA);

            AST_T* ast_expression = parser_parse_expression(parser, scope);
            function_call->function_call_arguments_size += 1;
            function_call->function_call_arguments = realloc(
                function_call->function_call_arguments,
                function_call->function_call_arguments_size * sizeof(struct AST_STRUCT*)
            );
            function_call->function_call_arguments[function_call->function_call_arguments_size-1] = ast_expression;
        }
    }

    parser_eat(parser, TOKEN_RIGHTPAREN);

    function_call->scope = scope;

    return function_call;
}

AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope)
{
    parser_eat(parser, TOKEN_ID); // var
    char* variable_definition_variable_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // var name
    parser_eat(parser, TOKEN_EQUALS);

    AST_T* variable_definition_value = parser_parse_expression(parser, scope);

    AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
    variable_definition->variable_definition_variable_name = variable_definition_variable_name;
    variable_definition->variable_definition_value = variable_definition_value;

    variable_definition->scope = scope;

    return variable_definition;
}

AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_FUNCTION_DEFINITION); // ast node
    parser_eat(parser, TOKEN_ID); // function

    char* function_name = parser->current_token->value;
    ast->function_definition_name = calloc(
        strlen(function_name) + 1, sizeof(char)
    );
    strcpy(ast->function_definition_name, function_name);

    parser_eat(parser, TOKEN_ID); // function name

    parser_eat(parser, TOKEN_LEFTPAREN);

    if (parser->current_token->type != TOKEN_RIGHTPAREN) // Checks if there are any args
    {
        ast->function_definition_args = calloc(1, sizeof(struct AST_STRUCT*));

        AST_T* arg = parser_parse_variable(parser, scope);
        ast->function_definition_args_size += 1;
        ast->function_definition_args[ast->function_definition_args_size - 1] = arg;

        while (parser->current_token->type == TOKEN_COMMA)
        {
            parser_eat(parser, TOKEN_COMMA);

            ast->function_definition_args_size += 1;

            ast->function_definition_args = 
            realloc(
                ast->function_definition_args, 
                ast->function_definition_args_size * sizeof(struct AST_STRUCT*)
            );

            AST_T* arg = parser_parse_variable(parser, scope);
            ast->function_definition_args[ast->function_definition_args_size - 1] = arg;
        }
    }

    parser_eat(parser, TOKEN_RIGHTPAREN);

    parser_eat(parser, TOKEN_LEFTBRACE);

    ast->function_definition_body = parser_parse_statements(parser, scope);

    parser_eat(parser, TOKEN_RIGHTBRACE);

    ast->scope = scope;

    return ast;
}

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope)
{
    char* token_value = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // var name or function call name

    if (parser->current_token->type == TOKEN_LEFTPAREN)
        return parser_parse_function_call(parser, scope);

    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    ast_variable->scope = scope;

    return ast_variable;
}

AST_T* parser_parse_string(parser_T* parser, scope_T* scope)
{
    AST_T* ast_string = init_ast(AST_STRING);
    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    ast_string->scope = scope;

    return ast_string;
}

AST_T* parser_parse_id(parser_T* parser, scope_T* scope)
{
    if (strcmp(parser->current_token->value, "var") == 0)
    {
        return parser_parse_variable_definition(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "function") == 0)
    {
        return parser_parse_function_definition(parser, scope);
    }
    else
    {
        return parser_parse_variable(parser, scope);
    }
}

AST_T* parser_parse_boolean(parser_T* parser, scope_T* scope, int value)
{
    AST_T* ast_boolean = init_ast(AST_BOOLEAN);
    ast_boolean->boolean_value = value;

    parser_eat(parser, value ? TOKEN_TRUE : TOKEN_FALSE);

    ast_boolean->scope = scope;

    return ast_boolean;
}

AST_T* parser_parse_if(parser_T* parser, scope_T* scope)
{
    parser_eat(parser, TOKEN_IF);
    parser_eat(parser, TOKEN_LEFTPAREN);

    AST_T* condition = parser_parse_expression(parser, scope);

    parser_eat(parser, TOKEN_RIGHTPAREN);
    parser_eat(parser, TOKEN_LEFTBRACE);

    AST_T* body = parser_parse_statements(parser, scope);

    parser_eat(parser, TOKEN_RIGHTBRACE);

    AST_T* ast_if = init_ast(AST_IF);
    ast_if->if_condition = condition;
    ast_if->if_body = body;

    while (parser->current_token->type == TOKEN_ELSEIF)
    {
        parser_eat(parser, TOKEN_ELSEIF);
        parser_eat(parser, TOKEN_LEFTPAREN);

        AST_T* elseif_condition = parser_parse_expression(parser, scope);

        parser_eat(parser, TOKEN_RIGHTPAREN);
        parser_eat(parser, TOKEN_LEFTBRACE);

        AST_T* elseif_body = parser_parse_statements(parser, scope);

        parser_eat(parser, TOKEN_RIGHTBRACE);

        ast_if->if_elseif_conditions = realloc(
            ast_if->if_elseif_conditions,
            (ast_if->if_elseif_conditions_size + 1) * sizeof(AST_T*)
        );

        ast_if->if_elseif_bodies = realloc(
            ast_if->if_elseif_bodies,
            (ast_if->if_elseif_bodies_size + 1) * sizeof(AST_T*)
        );

        ast_if->if_elseif_conditions[ast_if->if_elseif_conditions_size++] = elseif_condition;
        ast_if->if_elseif_bodies[ast_if->if_elseif_bodies_size++] = elseif_body;
    }

    if (parser->current_token->type == TOKEN_ELSE)
    {
        parser_eat(parser, TOKEN_ELSE);
        parser_eat(parser, TOKEN_LEFTBRACE);

        AST_T* else_body = parser_parse_statements(parser, scope);

        parser_eat(parser, TOKEN_RIGHTBRACE);
        ast_if->if_else_body = else_body;
    }

    return ast_if;
}

AST_T* parser_parse_number(parser_T* parser, scope_T* scope)
{
    AST_T* ast_number = init_ast(AST_NUMBER);
    
    // Converting string to a float
    ast_number->number_value = atof(parser->current_token->value);
    parser_eat(parser, TOKEN_NUMBER);

    ast_number->scope = scope;

    return ast_number;
}