#include "include/visitor.h"
#include "include/scope.h"
#include "include/token.h"
#include <stdio.h>
#include <string.h>

const char* get_type_string(int type)
{
    switch (type)
    {
    case AST_TYPE_INT:
        return "int";
        break;
    case AST_TYPE_FLOAT:
        return "float";
        break;
    case AST_TYPE_CHAR:
        return "char";
        break;
    case AST_TYPE_STRING:
        return "string";
        break;
    case AST_TYPE_GENERIC:
        return "generic";
        break;
    
    default:
        return "Unknown type";
        break;
    }
}

static AST_T* builtin_function_print(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_STRING: 
                printf("%s", visited_ast->string_value);
                break;

            case AST_BOOLEAN: 
                printf("%s", visited_ast->boolean_value ? "true" : "false");
                break;

            case AST_NUMBER:
                if (visited_ast->value_type == AST_TYPE_INT)
                    printf("%d", (int)visited_ast->number_value);
                else
                    printf("%f", visited_ast->number_value);
                break;
            
            case AST_CHAR: 
                printf("%c", visited_ast->char_value);
                break;
            
            default: 
                printf("%p", visited_ast);
                break;
        }
    }

    return init_ast(AST_NOOP);
}

static AST_T* builtin_function_println(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_STRING: printf("%s\n", visited_ast->string_value);
                break;

            case AST_BOOLEAN: printf("%s\n", visited_ast->boolean_value ? "true" : "false");
                break;

            case AST_NUMBER:
                if (visited_ast->value_type == AST_TYPE_INT)
                    printf("%d\n", (int)visited_ast->number_value);
                else
                    printf("%f\n", visited_ast->number_value);
                break;

            case AST_CHAR: 
                printf("%c\n", visited_ast->char_value);
                break;
            
            default: printf("%p\n", visited_ast);
                break;
        }
    }

    return init_ast(AST_NOOP);
}

static AST_T* builtin_function_input(visitor_T* visitor, AST_T** args, int args_size)
{
    char input_buffer[1024]; // Buffer for storing user input

    if (args_size > 0)
    {
        // Print the prompt message if any
        AST_T* prompt_ast = visitor_visit(visitor, args[0]);

        if (prompt_ast->type == AST_STRING)
        {
            printf("%s", prompt_ast->string_value);
        }
    }

    // Read user input
    if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL)
    {
        // Removing trailing newline character
        size_t len = strlen(input_buffer);

        if (len > 0 && input_buffer[len - 1] == '\n')
        {
            input_buffer[len - 1] = '\0';
        }

        // Creating a new AST node for the string input
        AST_T* input_ast = init_ast(AST_STRING);
        input_ast->string_value = strdup(input_buffer);
        return input_ast;
    }

    // Returning an empty string if input failed
    AST_T* empty_ast = init_ast(AST_STRING);
    empty_ast->string_value = strdup("");
    return empty_ast;
}

visitor_T* init_visitor()
{
    visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));

    visitor->return_value = (void*)0;
    visitor->return_called = 0;

    return visitor;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{        
    switch (node->type)
    {
        case AST_VARIABLE_DEFINITION: 
            return visitor_visit_variable_definition(visitor, node);
            break;

        case AST_FUNCTION_DEFINITION: 
            return visitor_visit_function_definition(visitor, node);
            break;

        case AST_VARIABLE: 
            return visitor_visit_variable(visitor, node);
            break;

        case AST_FUNCTION_CALL: 
            return visitor_visit_function_call(visitor, node);
            break;

        case AST_STRING:   
            return visitor_visit_string(visitor, node);
            break; 

        case AST_COMPOUND: 
            return visitor_visit_compound(visitor, node);
            break;

        case AST_BOOLEAN: 
            return visitor_visit_boolean(visitor, node);
            break;

        case AST_IF: 
            return visitor_visit_if(visitor, node);
            break;

        case AST_NUMBER: 
            return visitor_visit_number(visitor, node);
            break;

        case AST_BINOP: 
            return visitor_visit_binop(visitor, node);
            break;

        case AST_WHILE: 
            return visitor_visit_while(visitor, node);
            break;

        case AST_CHAR: 
            return visitor_visit_char(visitor, node);
            break;

        case AST_RETURN: 
            return visitor_visit_return(visitor, node);
            break;

        case AST_NOOP: return node;
            break;

        default:
            break;
    }

    fprintf(stderr, "Uncaught state of type `%d`\n", node->type);
    exit(1);

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
    if (node->variable_definition_value->type == AST_FUNCTION_CALL)
    {
        // Getting the value from the function call which has a return type
        node->variable_definition_value = visitor_visit(visitor, node->variable_definition_value);
    }
    
    AST_T* existing_var_def = scope_get_variable_definition(node->scope, node->variable_definition_variable_name);

    if (existing_var_def != NULL)
    {

        if (existing_var_def->variable_definition_type != node->variable_definition_value->value_type &&
            existing_var_def->variable_definition_type != AST_TYPE_GENERIC)
        {
            fprintf(stderr, "Type error: Whoa! You can't assign a value of type %s to a variable of type %s\n", 
            get_type_string(node->variable_definition_value->value_type),
            get_type_string(existing_var_def->variable_definition_type));
            exit(1);
        }

        // Update the existing variable's value
        existing_var_def->variable_definition_value = visitor_visit(visitor, node->variable_definition_value);
        return existing_var_def;
    }
    else
    {
        if (node->variable_definition_type != node->variable_definition_value->value_type &&
            node->variable_definition_type != AST_TYPE_GENERIC)
        {
            fprintf(stderr, "Type error: Whoa! You can't assign a value of type %s to a variable of type %s\n", 
            get_type_string(node->variable_definition_value->value_type), 
            get_type_string(node->variable_definition_type));
            exit(1);
        }

        // Add the new variable definition to the scope
        scope_add_variable_definition(node->scope, node);
        return node;
    }
}


AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_function_definition(node->scope, node);

    return node;
}

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
    AST_T* vardef = scope_get_variable_definition(node->scope, node->variable_name);

    if (vardef != (void*) 0) // if we found the var definition, visit it
        return visitor_visit(visitor, vardef->variable_definition_value);

    fprintf(stderr, "Wait what?! Undefined variable `%s`\n", node->variable_name);
    exit(1);
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
    if (strcmp(node->function_call_name, "display") == 0)
    {
        return builtin_function_print(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    if (strcmp(node->function_call_name, "displayln") == 0)
    {
        return builtin_function_println(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    if (strcmp(node->function_call_name, "input") == 0)
    {
        return builtin_function_input(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    AST_T* funcdef = scope_get_function_definition(node->scope, node->function_call_name);

    if (funcdef == (void*)0)
    {
        fprintf(stderr, "Hold up! Undefined function `%s`\n", node->function_call_name);
        exit(1);
    }

    if (funcdef->function_definition_args_size != node->function_call_arguments_size)
    {
        fprintf(stderr, "Error: Function '%s' expects %ld arguments, but %ld were provided.\n",
        node->function_call_name,
        funcdef->function_definition_args_size,
        node->function_call_arguments_size);
        exit(1);
    }

    // For functions that return a value
    visitor->return_called = 0;
    visitor->return_value = (void*)0;

    for (int i = 0; i < (int) node->function_call_arguments_size; i++)
    {
        // Grab the variable from the function definition arguments
        AST_T* ast_var = (AST_T*) funcdef->function_definition_args[i];

        // Grab the value from the function call arguments
        AST_T* ast_value = (AST_T*) node->function_call_arguments[i];

        // Create variable definition
        AST_T* ast_vardef = init_ast(AST_VARIABLE_DEFINITION);

        // Attach value to variable definition
        ast_vardef->variable_definition_value = ast_value;

        // Copy variable name into variable definition name
        ast_vardef->variable_definition_variable_name = (char*) calloc(strlen(ast_var->variable_name) + 1, sizeof(char));
        strcpy(ast_vardef->variable_definition_variable_name, ast_var->variable_name);

        scope_add_variable_definition(funcdef->function_definition_body->scope, ast_vardef);
    }

    AST_T* function_body_visit = visitor_visit(visitor, funcdef->function_definition_body);

    if (visitor->return_called)
    {
        if (funcdef->function_definition_type == AST_TYPE_GENERIC)
        {
            return visitor->return_value;
        }
        else if (funcdef->function_definition_type == visitor->return_value->value_type)
        {
            return visitor->return_value;
        }
        else if (funcdef->function_definition_type == 0)
        {
            fprintf(stderr, "Function: '%s' does not have a return type.\n", funcdef->function_definition_name);
            exit(1);
        }
        else
        {
            printf("Type error: What?! A function of type %s can't return a variable of type %s\n", 
            get_type_string(funcdef->function_definition_type), 
            get_type_string(visitor->return_value->value_type));
            exit(1);
        }
    }
    else if (funcdef->function_definition_type != 0)
    {
        fprintf(stderr, "You're missing the 'return' keyword in function: '%s'.\n", funcdef->function_definition_name);
        exit(1);
    }

    return function_body_visit;
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
    for (int i = 0; i < node->compound_size; i++)
    {
        visitor_visit(visitor, node->compound_value[i]);
    }

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_boolean(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_if(visitor_T* visitor, AST_T* node)
{
    // Check condition
    AST_T* condition = visitor_visit(visitor, node->if_condition);

    if (condition->boolean_value)
    {
        return visitor_visit(visitor, node->if_body);
    }
    else
    {
        for (size_t i = 0; i < node->if_elseif_conditions_size; i++)
        {
            AST_T* elseif_condition = visitor_visit(visitor, node->if_elseif_conditions[i]);
            if (elseif_condition->boolean_value)
            {
                return visitor_visit(visitor, node->if_elseif_bodies[i]);
            }
        }

        if (node->if_else_body)
        {
            return visitor_visit(visitor, node->if_else_body);
        }
    }

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_number(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_binop(visitor_T* visitor, AST_T* node)
{
    AST_T* binop_left = visitor_visit(visitor, node->binop_left);
    AST_T* binop_right = visitor_visit(visitor, node->binop_right);

    int number_value = 0;

    AST_T* ast_number = init_ast(AST_NUMBER);

    switch (node->op)
    {
        case TOKEN_PLUS:

            if (binop_left->type == AST_STRING && binop_right->type == AST_STRING)
            {
                // Concatenate strings
                size_t left_len = strlen(binop_left->string_value);
                size_t right_len = strlen(binop_right->string_value);
                char* result_str = calloc(left_len + right_len + 1, sizeof(char));
                strcpy(result_str, binop_left->string_value);
                strcat(result_str, binop_right->string_value);

                AST_T* ast_string = init_ast(AST_STRING);
                ast_string->string_value = result_str;

                return ast_string;
            }

            if (binop_left->value_type == AST_TYPE_INT)
            {
                ast_number->number_value = (int)(binop_left->number_value + binop_right->number_value);
                ast_number->value_type = AST_TYPE_INT;
            }
            else if (binop_left->value_type == AST_TYPE_FLOAT)
            {
                ast_number->number_value = (binop_left->number_value + binop_right->number_value);
                ast_number->value_type = AST_TYPE_FLOAT;
            }

            return ast_number;
            
        case TOKEN_MINUS:
            ast_number->number_value = (binop_left->number_value - binop_right->number_value);
            return ast_number;

        case TOKEN_MUL:
            ast_number->number_value = (binop_left->number_value * binop_right->number_value);
            return ast_number;

        case TOKEN_DIV:
            if (binop_right->number_value == 0)
            {
                fprintf(stderr, "Division by zero error \n");
                exit(1);
            }

            ast_number->number_value = (binop_left->number_value / binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value / binop_right->number_value);

            return ast_number;

        case TOKEN_EQEQ:
            ast_number->number_value = (binop_left->number_value == binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value == binop_right->number_value);

            return ast_number;

        case TOKEN_NEQ:
            ast_number->number_value = (binop_left->number_value != binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value != binop_right->number_value);

            return ast_number;

        case TOKEN_LT:
            ast_number->number_value = (binop_left->number_value < binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value < binop_right->number_value);

            return ast_number;

        case TOKEN_GT:
            ast_number->number_value = (binop_left->number_value > binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value > binop_right->number_value);

            return ast_number;

        case TOKEN_LTE:
            ast_number->number_value = (binop_left->number_value <= binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value <= binop_right->number_value);

            return ast_number;

        case TOKEN_GTE:
            ast_number->number_value = (binop_left->number_value >= binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value >= binop_right->number_value);
            return ast_number;

        case TOKEN_OR:
            ast_number->number_value = (binop_left->number_value || binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value || binop_right->number_value);

            return ast_number;

        case TOKEN_AND:
            ast_number->number_value = (binop_left->number_value && binop_right->number_value);
            ast_number->boolean_value = (binop_left->number_value && binop_right->number_value);

            return ast_number;

        default:
            break;
    }

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_while(visitor_T* visitor, AST_T* node)
{
    while (visitor_visit(visitor, node->while_condition)->boolean_value)
    {
        visitor_visit(visitor, node->while_body);
    }

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_char(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_return(visitor_T* visitor, AST_T* node)
{
    AST_T* return_value = visitor_visit(visitor, node->return_value);

    visitor->return_value = return_value;
    visitor->return_called = 1;

    // To stop visiting further nodes
    return return_value;
}
