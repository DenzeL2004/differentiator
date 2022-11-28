#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "reader.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"


static Node* Get_expression        (int *pos, const char* expression, char* copy_expression);

static Node* Get_term              (int *pos, const char* expression, char* copy_expression);

static Node* Get_degree            (int *pos, const char* expression, char* copy_expression);


static Node* Get_priority          (int *pos, const char* expression, char* copy_expression);



static Node* Unary_operation       (int *pos, const char* expression, char* copy_expression);

static int   Check_unary_operation (const int pos, const char* expression);


static double Get_num (int *pos, const char* expression);

static char*  Get_var (int *pos, const char* expression, char* copy_expression);

//=================================================================================================

int Parce_math_expression (Tree *tree_expresion, const char *expression, char *copy_expression)
{
    assert (tree_expresion  != nullptr && "tree_expresion is nullptr");
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");

    Free_differentiator_nodes_data (tree_expresion->root);
    
    int pos = 0;
    tree_expresion->root = Get_expression (&pos, expression, copy_expression);

    if (Check_nullptr (tree_expresion->root))
        return PROCESS_ERROR (PARCE_ERR, "root is nullptr");

    if (*(expression + pos) != '\0')
        return PROCESS_ERROR (SYNTAX_ERR, "Expression ended with a non-null character,"
                                                  " symbol = |%c|", *(expression + pos));

    return 0;
}

//=================================================================================================

static Node* Get_expression (int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");

    Node* node = Get_term (pos, expression, copy_expression);

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

        node->right = Get_term (pos, expression, copy_expression);

        if (operation == '+')
            node = Create_operation_node (OP_ADD, node->left, node->right);
        else
            node = Create_operation_node (OP_SUB, node->left, node->right);

    }

    return node;
}

//=================================================================================================

static Node* Get_term (int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");

    Node* node = Get_degree (pos, expression, copy_expression);
    
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

        node->right = Get_degree (pos, expression, copy_expression);

        if (operation == '*')
            node = Create_operation_node (OP_MUL, node->left, node->right);
        else
            node = Create_operation_node (OP_DIV, node->left, node->right);
    }

    return node;   
}

//=================================================================================================

static Node* Get_degree (int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");

    Node* node = Get_priority (pos, expression, copy_expression);
    
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

        node->right = Get_priority (pos, expression, copy_expression);
        
        node = Create_operation_node (OP_DEG, node->left, node->right);
    }

    return node;   
}

//=================================================================================================

static Node* Get_priority (int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");

    Node* node = nullptr;

    if (*(expression + *pos) == '(')
    {
        (*pos)++;

        node = Get_expression (pos, expression, copy_expression);

        if (*(expression + *pos) != ')')
        {
            PROCESS_ERROR (SYNTAX_ERR, "not \')\' after \'(\', symbol = |%c|\n", *(expression + *pos));
            return nullptr;
        }
        
        (*pos)++;
    }
    
    else if (Check_unary_operation (*pos, expression))
    {
        node = Unary_operation (pos, expression, copy_expression);
    }
    else
    {
        char* variable = Get_var (pos, expression, copy_expression);
        if (Check_nullptr (variable))
        {
            double val = Get_num (pos, expression);
            node = Create_value_node (val, nullptr, nullptr);
        }

        else
        {
            node = Create_variable_node (variable, nullptr, nullptr);
        }
    }

    return node;
}

//=================================================================================================

static Node* Unary_operation (int *pos, const char* expression, char* copy_expression)
{
    assert (expression      != nullptr && "expression is nullptr");
    assert (copy_expression != nullptr && "copy_expression is nullptr");

    Node* node = nullptr;

    if (!strncmp (expression + *pos, "sin", 3))
    {
        *pos += 3;

        node = Create_operation_node (OP_SIN, nullptr, nullptr);

        node->left = Create_node ();
        if (Check_nullptr (node->left))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, left son is nullptr\n");
            return nullptr;
        }

        node->left = Get_priority (pos, expression, copy_expression);
        
        return node;
    }

    if (!strncmp (expression + *pos, "cos", 3))
    {
        *pos += 3;

        node = Create_operation_node (OP_COS, nullptr, nullptr);

        node->left = Create_node ();
        if (Check_nullptr (node->left))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, left son is nullptr\n");
            return nullptr;
        }
        
        node->left = Get_priority (pos, expression, copy_expression);
        
        return node;
    }

    if (!strncmp (expression + *pos, "ln", 2))
    {
        *pos += 2;

        node = Create_operation_node (OP_LOG, nullptr, nullptr);

        node->left = Create_node ();
        if (Check_nullptr (node->left))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, left son is nullptr\n");
            return nullptr;
        }

        node->left = Get_priority (pos, expression, copy_expression);

        return node;
    }

    return node;
}

//=================================================================================================

static int Check_unary_operation (const int pos, const char* expression)
{
    assert (expression != nullptr && "expression is nullptr");

    if (!strncmp ((expression + pos), "sin", 3)) return 1;

    if (!strncmp ((expression + pos), "cos", 3)) return 1;

    if (!strncmp ((expression + pos), "ln", 2)) return 1;

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

    sscanf (copy_expression + *pos, "%*[^^+-*/() 0-9,.\] %n", &shift);   

    if (!shift)
        return nullptr;
    
    *pos += shift;
    *(copy_expression + *pos) = '\0';

    return variable;
}

//=================================================================================================
