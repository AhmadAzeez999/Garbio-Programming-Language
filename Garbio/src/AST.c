#include "include/AST.h"

AST_T* init_ast(int type)
{
    AST_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->type = type;

    ast->scope = (void*) 0;

    /* AST_VARIABLE_DEFINITION */
    ast->variable_definition_variable_name = (void*) 0;
    ast->variable_definition_value = (void*) 0;

    /* AST_FUNCTION_DEFINITION */
    ast->function_definition_body = (void*) 0;
    ast->function_definition_name = (void*) 0;
    ast->function_definition_args = (void*) 0;
    ast->function_definition_args_size = 0;

    /* AST_VARIABLE */
    ast->variable_name = (void*) 0;

    /* AST_FUNCTION_CALL */
    ast->function_call_name = (void*) 0;
    ast->function_call_arguments = (void*) 0;
    ast->function_call_arguments_size = 0;

    /* AST_STRING */
    ast->string_value = (void*) 0;

    /* AST_COMPOUND */
    ast->compound_value = (void*) 0;
    ast->compound_size = 0;

    /* AST_BOOLEAN */
    ast->boolean_value = 0;

    /* AST_IF_ELSE */
    ast->if_condition = (void*) 0;
    ast->if_body = (void*) 0;
    ast->if_elseif_conditions = (void*) 0;
    ast->if_elseif_conditions_size = 0;
    ast->if_elseif_bodies = (void*) 0;
    ast->if_elseif_bodies_size = 0;
    ast->if_else_body = (void*) 0;

    /* AST_NUMBER */
    ast->string_value;

    /* AST_BINOP */
    ast->binop_left = (void*) 0;
    ast->binop_right = (void*) 0;
    ast->op = 0;

    return ast;
}