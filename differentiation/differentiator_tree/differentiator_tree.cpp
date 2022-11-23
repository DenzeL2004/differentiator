#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "differentiator_tree.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"

//======================================================================================

Differentiator_data* Init_differentiator_node ()
{
    Differentiator_data *data = (Differentiator_data*) calloc (1, sizeof (Differentiator_data)); 
    if (Check_nullptr (data))
    {
        PROCESS_ERROR (CREATE_EMPTY_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    data->node_type = UNKNOWN_T;

    data->data.operation = OP_UNKNOWN; 
    data->data.val       = 0;
    data->data.var       = nullptr;

    return data;
}

//======================================================================================

int Get_operation_node (Node *node, int operation)
{
    assert (node != nullptr && "node is nullptr");

    Differentiator_data* node_data = Init_differentiator_node ();

    if (Check_nullptr (node_data))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return GET_OPERATION_NODE_ERR;
    }


    node_data->node_type = OPERATION_T;

    node_data->data.operation = operation;

    node->data = node_data;

    return 0;
}

//======================================================================================

int Get_value_node (Node *node, double value)
{
    assert (node != nullptr && "node is nullptr");
    
    Differentiator_data* node_data = Init_differentiator_node ();

    if (Check_nullptr (node_data))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return GET_OPERATION_NODE_ERR;
    }


    node_data->node_type = VALUE_T;

    node_data->data.val = value;

    node->data = node_data;

    return 0;
}

//======================================================================================

int Get_variable_node (Node *node, const char* name_variable)
{   
    assert (node != nullptr && "node is nullptr");
    assert (name_variable != nullptr && "name_variable is nullptr");
    
    Differentiator_data* node_data = Init_differentiator_node ();

    if (Check_nullptr (node_data))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return GET_OPERATION_NODE_ERR;
    }


    node_data->node_type = VARIABLE_T;

    node_data->data.var = name_variable;

    node->data = node_data;

    return 0;
}

//======================================================================================

int Free_differentiatoer_nodes_data (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    ((Differentiator_data*) node->data)->node_type = UNKNOWN_T;

    ((Differentiator_data*) node->data)->data.operation = OP_UNKNOWN;
    ((Differentiator_data*) node->data)->data.val       = 0;
    ((Differentiator_data*) node->data)->data.var       = nullptr;

    free (node->data);
    node->data = nullptr;

    if (!Check_nullptr (node->left))
        Free_differentiatoer_nodes_data (node->left);

    if (!Check_nullptr (node->right))
        Free_differentiatoer_nodes_data (node->right);

    Delete_node (node);

    return 0;
}

