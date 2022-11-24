#include <assert.h>


#include "../differentiator_tree/differentiator_tree.h"
#include "../differentiator_tree/draw_tree.h"

#include "../operation_list.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"

static int Simplication_node (Node* node);

static int Simple_op_mul (Node *node);

//======================================================================================

int Simplifier (Node* node)
{
    assert (node != nullptr && "null is nullptr");  

    int changed = 0;

    if (Is_leaf_node (node))
        return changed;

    if (!Check_nullptr (node->left))
        changed |= Simplifier (node->left);

    if (!Check_nullptr (node->right))
    {
        changed |= Simplifier (node->right);
    }

    changed |= Simplication_node (node);

    return changed;
}   

//======================================================================================

static int Simplication_node (Node* node)
{
    assert (node != nullptr && "null is nullptr");  

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    
    if (node_data->node_type == OPERATION_T)
    {
        if (node_data->data.operation == OP_MUL)
            return Simple_op_mul (node);
    }

    return 0;
}


//======================================================================================

static int Simple_op_add (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    Differentiator_data *left_node_data = (Differentiator_data*) node->left->data;
    Differentiator_data *right_node_data = (Differentiator_data*) node->right->data;


    if (right_node_data->node_type  == VALUE_T && Is_zero (right_node_data->data.val))
    {
        Free_differentiator_nodes_data (node->right);

        node_data->node_type      = left_node_data->node_type;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = left_node_data->data.val;

        Free_differentiator_nodes_data (node->left);

        node->left  = nullptr;
        node->right = nullptr;
    
        return 1;
    }

    if (left_node_data->node_type  == VALUE_T && right_node_data->node_type == VALUE_T)
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = right_node_data->data.val + left_node_data->data.val;

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;
    
        return 1;
    }

    return 0;
}

//======================================================================================

static int Simple_op_mul (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    Differentiator_data *left_node_data = (Differentiator_data*) node->left->data;
    Differentiator_data *right_node_data = (Differentiator_data*) node->right->data;

    if ((left_node_data->node_type == VALUE_T))
    {
        Node* tmp_node = node->left;
        node->left = node->right;
        node->right = tmp_node;
    }

    if (right_node_data->node_type == VALUE_T && Is_zero (right_node_data->data.val))
    {
    
        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);
        
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = 0;

        node->left  = nullptr;
        node->right = nullptr;
    
        return 1;
    }


    if (right_node_data->node_type == VALUE_T && Equality_double (right_node_data->data.val, 1.0))
    {
        Node *node_cpy = Tree_copy (node->left);
    
        My_swap (node, node_cpy, sizeof (Node));

        Free_differentiator_nodes_data (node_cpy);
    
        return 1;
    }

    if (left_node_data->node_type  == VALUE_T && right_node_data->node_type == VALUE_T)
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = right_node_data->data.val * left_node_data->data.val;

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    return 0;
}

//======================================================================================