#include "include/scope.h"
#include <string.h>

scope_T* init_scope()
{
    scope_T* scope = calloc(1, sizeof(struct SCOPE_STRUCT));
    scope->function_definitions = (void*) 0;
    scope->function_definitions_size = 0;

    scope->variable_definitions = (void*) 0;
    scope->variable_definitions_size = 0;

    return scope;
}

AST_T* scope_add_function_definition(scope_T* scope, AST_T* funcdef)
{
    scope->function_definitions_size += 1;

    // If it is not pointing to any memory location
    if (scope->function_definitions == (void*)0)
    {
        // It will contain a list of AST pointers
        scope->function_definitions = calloc(1, sizeof(struct AST_STRUCT*));
    }
    else
    {
        scope->function_definitions = 
            realloc(
                scope->function_definitions,
                scope->function_definitions_size * sizeof(struct AST_STRUCT**)
            );
    }

    scope->function_definitions[scope->function_definitions_size - 1] = funcdef;

    return funcdef;
}

AST_T* scope_get_function_definition(scope_T* scope, const char* funcname)
{
    for (int i = 0; i < scope->function_definitions_size; i++)
    {
        AST_T* funcdef = scope->function_definitions[i];

        if (strcmp(funcdef->function_definition_name, funcname) == 0)
        {
            return funcdef;
        }
    }

    return (void*)0;
}

AST_T* scope_add_variable_definition(scope_T* scope, AST_T* vardef)
{
    if (scope->variable_definitions == (void*) 0) // if the list is a null pointer
    {
        scope->variable_definitions = calloc(1, sizeof(struct AST_STRUCT*));
        scope->variable_definitions[0] = vardef; // push def to list
        scope->variable_definitions_size += 1;
    }
    else // if the list already exists
    {
        scope->variable_definitions_size += 1;
        scope->variable_definitions = realloc(
            scope->variable_definitions,
            scope->variable_definitions_size * sizeof(struct AST_STRUCT*)
        );
        scope->variable_definitions[scope->variable_definitions_size - 1] = vardef;
    }

    return vardef;
}

AST_T* scope_get_variable_definition(scope_T* scope, const char* varname)
{
    for (int i = 0; i < scope->variable_definitions_size; i++)
    {
        AST_T* vardef = scope->variable_definitions[i];

        if (strcmp(vardef->variable_definition_variable_name, varname) == 0)
        {
            return vardef;
        }
    }

    return (void*)0;
}