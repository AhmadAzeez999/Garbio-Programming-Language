#ifndef SCOPE_H
#define SCOPE_H
#include "AST.h"

typedef struct SCOPE_STRUCT
{
    AST_T** function_definitions;
    size_t function_definitions_size;

    AST_T** variable_definitions;
    size_t variable_definitions_size;

} scope_T;

scope_T* init_scope();

AST_T* scope_add_function_definition(scope_T* scope, AST_T* funcdef);

AST_T* scope_get_function_definition(scope_T* scope, const char* funcname);

AST_T* scope_add_variable_definition(scope_T* scope, AST_T* vardef);

AST_T* scope_get_variable_definition(scope_T* scope, const char* varname);

#endif