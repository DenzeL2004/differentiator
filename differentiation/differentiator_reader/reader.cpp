#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "reader.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"


static Node* Expr     (Node* node, int *pos, const char* expression, char* copy_expression);

static Node* Term     (Node* node, int *pos, const char* expression, char* copy_expression);

static Node* Get_degree   (Node* node, int *pos, const char* expression, char* copy_expression);


static Node* Get_expression     (Node* node, int *pos, const char* expression, char* copy_expression);



static Node* Unary_operation       (Node* node, int *pos, const char* expression, char* copy_expression);

static int   Check_unary_operation (const int pos, const char* expression);


static double Get_num (int *pos, const char* expression);

static char*  Get_var (int *pos, const char* expression, char* copy_expression);

//=================================================================================================

int Parce_math_expression (Tree *tree_expresion, const char *expression, char *copy_expression)
{
    assert (tree_expresion  != nullptr && "tree_expresion is nullptr");
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");
    
    int pos = 0;
    tree_expresion->root = Expr (tree_expresion->root, &pos, expression, copy_expression);

    if (Check_nullptr (tree_expresion->root))
        return PROCESS_ERROR (PARCE_ERR, "root is nullptr");

    if (*(expression + pos) != '\0')
        return PROCESS_ERROR (SYNTAX_ERR, "Expression ended with a non-null character,"
                                                  " symbol = |%c|", *(expression + pos));

    return 0;
}

//=================================================================================================

static Node* Expr (Node* node, int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");
    assert (node            != nullptr && "node is nullptr");

    node = Term (node, pos, expression, copy_expression);

    while (*(expression + *pos) == '+' || *(expression + *pos) == '-')
    {
        Node* new_node = Create_node ();
        if (Check_nullptr (new_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, new node is nullptr\n");
            return nullptr;
        }

        new_node->left = node;
        node = new_node;

        const char operation = *(expression + *pos);
        (*pos)++;

        node->right = Create_node ();
        if (Check_nullptr (node->right))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, right son is nullptr\n");
            return nullptr;
        }

        Term (node->right, pos, expression, copy_expression);

        if (operation == '+')
            Get_operation_node (node, OP_ADD);
        else
            Get_operation_node (node, OP_SUB);

    }

    return node;
}

//=================================================================================================

static Node* Term (Node* node, int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");
    assert (node            != nullptr && "node is nullptr");

    node = Get_degree (node, pos, expression, copy_expression);
    
    while (*(expression + *pos) == '*' || *(expression + *pos) == '/')
    {
        Node* new_node = Create_node ();
        if (Check_nullptr (new_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, new node is nullptr\n");
            return nullptr;
        }

        new_node->left = node;
        node = new_node;

        const char operation = *(expression + *pos);
        (*pos)++;

        node->right = Create_node ();
        if (Check_nullptr (node->right))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, right son is nullptr\n");
            return nullptr;
        }

        node->right = Get_degree (node->right, pos, expression, copy_expression);

        if (operation == '*')
            Get_operation_node (node, OP_MUL);
        else
            Get_operation_node (node, OP_DIV);
    }

    return node;   
}

//=================================================================================================

static Node* Get_degree (Node* node, int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");
    assert (node            != nullptr && "node is nullptr");

    node = Get_expression (node, pos, expression, copy_expression);
    
    while (*(expression + *pos) == '^')
    {
        Node* new_node = Create_node ();
        if (Check_nullptr (new_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, new node is nullptr\n");
            return nullptr;
        }

        new_node->left = node;
        node = new_node;

        (*pos)++;

        node->right = Create_node ();
        if (Check_nullptr (node->right))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, new node is nullptr\n");
            return nullptr;
        }

        node->right = Get_expression (node->right, pos, expression, copy_expression);
        
        Get_operation_node (node, OP_DEG);
    }

    return node;   
}

//=================================================================================================

static Node* Get_expression (Node* node, int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");
    assert (node            != nullptr && "node is nullptr");

    if (*(expression + *pos) == '(')
    {
        (*pos)++;

        node = Expr (node, pos, expression, copy_expression);

        if (*(expression + *pos) != ')')
        {
            PROCESS_ERROR (SYNTAX_ERR, "not \')\' after \'(\', symbol = |%c|\n", *(expression + *pos));
            return nullptr;
        }
        
        (*pos)++;
    }
    
    else if (Check_unary_operation (*pos, expression))
    {
        node = Unary_operation (node, pos, expression, copy_expression);
    }
    else
    {
        char* variable = Get_var (pos, expression, copy_expression);
        if (Check_nullptr (variable))
        {
            double val = Get_num (pos, expression);
            Get_value_node (node, val);
        }

        else
            Get_variable_node (node, variable);
    }

    return node;
}

//=================================================================================================

static Node* Unary_operation (Node* node, int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");
    assert (node != nullptr && "node is nullptr");


    if (!strncmp (expression + *pos, "sin", 3))
    {
        *pos += 3;

        node->left = Create_node ();
        if (Check_nullptr (node->left))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, left son is nullptr\n");
            return nullptr;
        }

        node->left = Get_expression (node->left, pos, expression, copy_expression);

        Get_operation_node (node, OP_SIN);

        return node;
    }

    if (!strncmp (expression + *pos, "cos", 3))
    {
        *pos += 3;

        node->left = Create_node ();
        if (Check_nullptr (node->left))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, left son is nullptr\n");
            return nullptr;
        }

        node->left = Get_expression (node->left, pos, expression, copy_expression);

        Get_operation_node (node, OP_COS);

        return node;
    }

    if (!strncmp (expression + *pos, "log", 3))
    {
        *pos += 3;

        node->left = Create_node ();
        if (Check_nullptr (node->left))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, left son is nullptr\n");
            return nullptr;
        }

        node->left = Get_expression (node->left, pos, expression, copy_expression);

        Get_operation_node (node, OP_LOG);

        return node;
    }

    return nullptr;
}

//=================================================================================================

static int Check_unary_operation (const int pos, const char* expression)
{
    assert (expression != nullptr && "expression is nullptr");

    if (!strncmp ((expression + pos), "sin", 3)) return 1;

    if (!strncmp ((expression + pos), "cos", 3)) return 1;

    if (!strncmp ((expression + pos), "log", 3)) return 1;

    return 0;
}

//=================================================================================================

static double Get_num (int *pos, const char* expression)
{
    assert (expression != nullptr && "expression is nullptr");

    int shift = 0;

    double val = 0;

    sscanf (expression + *pos, "%lg %n", &val, &shift);    

    assert (shift != 0 && "the expr_ptr did not move");

    *pos += shift;

    return val;
}

//=================================================================================================

static char* Get_var (int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");

    int shift = 0;

    char *variable = (copy_expression + *pos);

    sscanf (copy_expression + *pos, "%*[^^+-*() 0-9,.] %n", &shift);   

    if (!shift)
        return nullptr;
    
    *pos += shift;
    *(copy_expression + *pos) = '\0';

    return variable;
}

//=================================================================================================
