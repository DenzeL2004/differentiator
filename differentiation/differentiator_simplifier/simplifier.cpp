#include <assert.h>
#include <math.h>

#include "../differentiator_tree/differentiator_tree.h"
#include "../differentiator_tree/draw_tree.h"

#include "../differntiator_dsl.h"

#include "simplifier.h"


#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"



static int Simplification_node  (Node* node);


static int Simple_op_mul        (Node *node);

static int Simple_op_add        (Node *node);

static int Simple_op_sub        (Node *node);

static int Simple_op_div        (Node *node);

static int Simple_op_deg        (Node *node);

static int Simple_op_sin        (Node *node);

static int Simple_op_cos        (Node *node);

static int Simple_op_log        (Node* node);

//======================================================================================

int Simplifier (Node* node)
{
    assert (node != nullptr && "node is nullptr");  

    int changed = 0;

    if (Is_leaf_node (node))
        return changed;

    if (!Check_nullptr (node->left))
        changed |= Simplifier (node->left);

    if (!Check_nullptr (node->right))
    {
        changed |= Simplifier (node->right);
    }

    changed |= Simplification_node (node);

    return changed;
}   

//======================================================================================

static int Simplification_node (Node* node)
{
    assert (node != nullptr && "null is nullptr");  

    
    if (IS_OP (node))
    {
        if (GET_OP (node) == OP_MUL)
            return Simple_op_mul (node);

        if (GET_OP (node) == OP_ADD)
            return Simple_op_add (node);

        if (GET_OP (node) == OP_SUB)
            return Simple_op_sub (node);

        if (GET_OP (node) == OP_DIV)
            return Simple_op_div (node);

        if (GET_OP (node) == OP_DEG)
            return Simple_op_deg (node);

        if (GET_OP (node) == OP_SIN)
            return Simple_op_sin (node);

        if (GET_OP (node) == OP_COS)
            return Simple_op_cos (node);

        if (GET_OP (node) == OP_LOG)
            return Simple_op_log (node);
    }

    return 0;
}


//======================================================================================

static int Simple_op_add (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;

    if (IS_VAL (node->left))
    {
        Node* tmp_node = node->left;
        node->left = node->right;
        node->right = tmp_node;
    }

    if (IS_VAL (node->right) && Is_zero (GET_VAL (node->right)))
    {
    
        Node *node_cpy = Tree_copy (node->left);
    
        My_swap (node, node_cpy, sizeof (Node));

        Free_differentiator_nodes_data (node_cpy);
    
        return 1;
    }


    if (IS_VAL (node->left) && IS_VAL (node->right))
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = GET_VAL (node->left) + GET_VAL (node->right);

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    return 0;
}

//======================================================================================

static int Simple_op_sub (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;

    if (IS_VAL (node->right) && Is_zero (GET_VAL (node->right)))
    {
    
        Node *node_cpy = Tree_copy (node->left);
    
        My_swap (node, node_cpy, sizeof (Node));

        Free_differentiator_nodes_data (node_cpy);
    
        return 1;
    }


    if (IS_VAL (node->left) && IS_VAL (node->right))
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = GET_VAL (node->left) - GET_VAL (node->right);

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

    if (IS_VAL (node->left))
    {
        Node* tmp_node = node->left;
        node->left = node->right;
        node->right = tmp_node;
    }

    if (IS_VAL (node->right) && Is_zero (GET_VAL (node->right)))
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


    if (IS_VAL (node->right) && Equality_double (GET_VAL (node->right), 1.0))
    {
        Node *node_cpy = Tree_copy (node->left);
    
        My_swap (node, node_cpy, sizeof (Node));

        Free_differentiator_nodes_data (node_cpy);
    
        return 1;
    }

    if (IS_VAL (node->left) && IS_VAL (node->right))
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = GET_VAL (node->left) * GET_VAL (node->right);

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    return 0;
}

//======================================================================================

static int Simple_op_div (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    
    if (IS_VAL (node->right) && Equality_double (GET_VAL (node->right), 1.0))
    {
        Node *node_cpy = Tree_copy (node->left);
    
        My_swap (node, node_cpy, sizeof (Node));

        Free_differentiator_nodes_data (node_cpy);
    
        return 1;
    }

    #ifdef CALC_CONST

    if (IS_VAL (node->left) && IS_VAL (node->right) && !Is_zero (GET_VAL (node->right)))
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = GET_VAL (node->left) / GET_VAL (node->right);

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    #endif

    return 0;
}

//======================================================================================

static int Simple_op_deg (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    
    if (IS_VAL (node->right) && Is_zero (GET_VAL (node->right)))
    {
    
        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);
        
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = 1;

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    if (IS_VAL (node->right) && Equality_double (GET_VAL (node->right), 1.0))
    {
        Node *node_cpy = Tree_copy (node->left);
    
        My_swap (node, node_cpy, sizeof (Node));

        Free_differentiator_nodes_data (node_cpy);
    
        return 1;
    }

    if (IS_VAL (node->left) && Is_zero (GET_VAL (node->left)))
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

    if (IS_VAL (node->left) && Equality_double (GET_VAL (node->left), 1.0))
    {
    
        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);
        
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = 1;

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    #ifdef CALC_CONST

    if (IS_VAL (node->left) && IS_VAL (node->right))
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = pow (GET_VAL (node->left), GET_VAL (node->right));

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    #endif

    return 0;
}

//======================================================================================

static int Simple_op_sin (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    
    #ifdef CALC_CONST

    if (IS_VAL (node->left))
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = sin (GET_VAL (node->left));

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    #endif

    return 0;
}

//======================================================================================

static int Simple_op_cos (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    
    #ifdef CALC_CONST

    if (IS_VAL (node->left))
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = cos (GET_VAL (node->left));

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    #endif

    return 0;
}

//======================================================================================

static int Simple_op_log (Node *node)
{
    assert (node != nullptr && "null is nullptr"); 

    Differentiator_data *node_data = (Differentiator_data*) node->data;
    
    #ifdef CALC_CONST

    if (IS_VAL (node->left) && !Is_zero (GET_VAL (node->left)) && GET_VAL (node->left) > 0.0)
    {
        node_data->node_type      = VALUE_T;
        node_data->data.operation = OP_UNKNOWN;
        node_data->data.val       = log (GET_VAL (node->left));

        Free_differentiator_nodes_data (node->left);
        Free_differentiator_nodes_data (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return 1;
    }

    #endif

    return 0;
}

//======================================================================================