#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum
    {
        AST_VARIABLE_DEFINITION,
        AST_FUNCTION_DEFINITION,
        AST_VARIABLE,
        AST_FUNCTION_CALL,
        AST_STRING,
        AST_COMPOUND,
        AST_BOOLEAN,
        AST_IF,
        AST_ELSEIF,
        AST_ELSE,
        AST_NUMBER,
        AST_BINOP,
        AST_WHILE,
        AST_TYPE_INT,
        AST_TYPE_FLOAT,
        AST_TYPE_DOUBLE,
        AST_TYPE_CHAR,
        AST_TYPE_STRING,
        AST_TYPE_GENERIC,
        AST_CHAR,
        AST_RETURN,
        AST_NOOP // No operation
    } type;

    // Scope pointer
    struct SCOPE_STRUCT* scope;

    /* AST_VARIABLE_DEFINITION */
    char* variable_definition_variable_name;
    struct AST_STRUCT* variable_definition_value;
    int variable_definition_type;
    int value_type;

    /* AST_FUNCTION_DEFINITION */
    struct AST_STRUCT* function_definition_body;
    char* function_definition_name;
    struct AST_STRUCT** function_definition_args;
    size_t function_definition_args_size;
    int function_definition_type;
    struct AST_STRUCT* function_definition_return_value;

    /* AST_VARIABLE */
    char* variable_name;

    /* AST_FUNCTION_CALL */
    char* function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t function_call_arguments_size;
    int function_call_type;

    /* AST_STRING */
    char* string_value;

    /* AST_COMPOUND */
    struct AST_STRUCT** compound_value;
    size_t compound_size;

    /* AST_BOOLEAN */
    int boolean_value;

    /* AST_IF_ELSE */
    struct AST_STRUCT* if_condition;
    struct AST_STRUCT* if_body;
    struct AST_STRUCT** if_elseif_conditions;
    size_t if_elseif_conditions_size;
    struct AST_STRUCT** if_elseif_bodies;
    size_t if_elseif_bodies_size;
    struct AST_STRUCT* if_else_body;

    /* AST_NUMBER */
    double number_value;

    /* AST_BINOP */
    struct AST_STRUCT* binop_left; // left operand
    struct AST_STRUCT* binop_right; // right operand
    int op; // operator

    /* AST_WHILE */
    struct AST_STRUCT* while_condition;
    struct AST_STRUCT* while_body;

    /* AST_ASSIGNMENT */
    struct AST_STRUCT* assignment_variable;
    struct AST_STRUCT* assignment_value;

    /* AST_CHAR */
    char char_value;

    /* AST_RETURN */
    struct AST_STRUCT* return_value;

} AST_T;

AST_T* init_ast(int type);

#endif