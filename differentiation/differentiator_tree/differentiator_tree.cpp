#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "differentiator_tree.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"


//======================================================================================

Node* Create_differentiatoer_node ()
{
    Node* node = Create_node ();

    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_EMPTY_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    Differentiator_node *differentiator_node = (Differentiator_node*) calloc (1, sizeof (Differentiator_node)); 
    if (Check_nullptr (differentiator_node))
    {
        PROCESS_ERROR (CREATE_EMPTY_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    differentiator_node->node_type = UNKNOWN_T;

    differentiator_node->data.operation = OP_UNKNOWN; 
    differentiator_node->data.val       = 0;
    differentiator_node->data.var       = nullptr;

    node->data = differentiator_node;

    return node;
}

//======================================================================================

Node* Get_operation_node (Differentiator_operation operation)
{
    Node* node = Create_differentiatoer_node ();

    if (Check_nullptr (node))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }


    ((Differentiator_node*) node->data)->node_type = OPERATION_T;

    ((Differentiator_node*) node->data)->data.operation = operation;

    return node;
}

//======================================================================================

Node* Get_value_node (double value)
{
    Node* node = Create_differentiatoer_node ();

    if (Check_nullptr (node))
    {
        PROCESS_ERROR (GET_VALUE_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }


    ((Differentiator_node*) node->data)->node_type = VALUE_T;

    ((Differentiator_node*) node->data)->data.val = value;

    return node;
}

//======================================================================================

Node* Get_variable_node (const char* name_variable)
{   
    assert (name_variable != nullptr && "name_variable is nullptr");

    Node* node = Create_differentiatoer_node ();

    if (Check_nullptr (node))
    {
        PROCESS_ERROR (GET_VARIABLE_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }


    ((Differentiator_node*) node->data)->node_type = VARIABLE_T;

    ((Differentiator_node*) node->data)->data.var = name_variable;

    return node;
}

//======================================================================================

int Free_differentiatoer_nodes_data (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    ((Differentiator_node*) node->data)->node_type = UNKNOWN_T;

    ((Differentiator_node*) node->data)->data.operation = OP_UNKNOWN;
    ((Differentiator_node*) node->data)->data.val       = 0;
    ((Differentiator_node*) node->data)->data.var       = nullptr;

    free (node->data);
    node->data = nullptr;

    if (!Check_nullptr (node->left))
        Free_differentiatoer_nodes_data (node->left);

    if (!Check_nullptr (node->right))
        Free_differentiatoer_nodes_data (node->right);

    Delete_node (node);

    return 0;
}

