#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "differentiator_tree.h"
#include "../differntiator_dsl.h"

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
    data->data.val       = NAN;
    data->data.var       = nullptr;

    return data;
}


//======================================================================================

Node* Create_value_node (const double value, Node* left, Node* right)
{
    Node *node = Create_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    Differentiator_data* node_data = Init_differentiator_node ();

    if (Check_nullptr (node_data))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    node_data->node_type = VALUE_T;

    node_data->data.val = value;

    node->data = node_data;

    node->left  = left;
    node->right = right;

    return node;
}

//======================================================================================

Node* Create_operation_node (const int operation, Node* left, Node* right)
{
    Node *node = Create_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    Differentiator_data* node_data = Init_differentiator_node ();

    if (Check_nullptr (node_data))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    node_data->node_type = OPERATION_T;

    node_data->data.operation = operation;

    node->data = node_data;

    node->left  = left;
    node->right = right;

    return node;

}

//======================================================================================

Node* Create_variable_node (const char *name_variable, Node* left, Node* right)
{
    assert (name_variable != nullptr && "name_variable is nullptr");

    Node *node = Create_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    Differentiator_data* node_data = Init_differentiator_node ();

    if (Check_nullptr (node_data))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    node_data->node_type = VARIABLE_T;

    node_data->data.var = name_variable;

    node->data = node_data;

    node->left  = left;
    node->right = right;

    return node;
}

//======================================================================================

int Free_differentiator_nodes_data (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    if (!Check_nullptr (node->data))
    {
        ((Differentiator_data*) node->data)->node_type = UNKNOWN_T;

        ((Differentiator_data*) node->data)->data.operation = OP_UNKNOWN;
        ((Differentiator_data*) node->data)->data.val       = NAN;
        ((Differentiator_data*) node->data)->data.var       = nullptr;

        free ((Differentiator_data*) node->data);
        node->data = nullptr;
    }


    if (!Check_nullptr (node->left))
        Free_differentiator_nodes_data (node->left);

    if (!Check_nullptr (node->right))
        Free_differentiator_nodes_data (node->right);

    Delete_node (node);
   
    return 0;
}

//======================================================================================

Node *Tree_copy (Node *node)
{
    if (Check_nullptr (node)) return nullptr;
    
    Node *left = nullptr, *right = nullptr;

    if (!Check_nullptr (node->left))
        left = Tree_copy(node->left);
    
    if (!Check_nullptr (node->right))
        right = Tree_copy(node->right);
    
    if (IS_OP (node))  return Create_operation_node (GET_OP (node), left, right);
    
    if (IS_VAL (node)) return Create_value_node (GET_VAL (node), left, right);

    if (IS_VAR (node)) return Create_variable_node (GET_VAR (node), left, right);
    
    return nullptr;
}


//======================================================================================