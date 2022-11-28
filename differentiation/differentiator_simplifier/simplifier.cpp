#include <assert.h>
#include <math.h>

#include "../differentiator_tree/differentiator_tree.h"
#include "../differentiator_tree/draw_tree.h"

#include "../differntiator_dsl.h"

#include "simplifier.h"


#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"



static int Simplification_node  (Node* node);

static int Convolution_of_constants (Node* node);

static int Neutral_element_removal  (Node *node);

static int Is_given_value_node (const Node *node, double val);

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
        changed |= Simplifier (node->right);

    #ifdef FAST_SIMPLIFIER
                      changed |= Simplification_node (node);
    #else
        if (!changed) changed |= Simplification_node (node);
    #endif

    return changed;
}   

//======================================================================================

static int Simplification_node (Node* node)
{
    assert (node != nullptr && "null is nullptr");  

    int changed = 0;

    changed |= Neutral_element_removal  (node);
    
    changed |= Convolution_of_constants (node);

    return changed;
}

//======================================================================================

static int Neutral_element_removal (Node *node)
{
    assert (node != nullptr && "null is nullptr");

    if (IS_OP (node))
    {
        if (Is_given_value_node (node->right, 0.0))
        {
            if (GET_OP (node) == OP_ADD || GET_OP (node) == OP_SUB)
            {   
                Node *node_cpy = Tree_copy (node->left);
                My_swap (node, node_cpy, sizeof (Node));
                Free_differentiator_nodes_data (node_cpy);

                return 1;
            }

            if (GET_OP (node) == OP_MUL || GET_OP (node) == OP_DEG)
            {
                Free_differentiator_nodes_data (node->left);
                Free_differentiator_nodes_data (node->right);
                
                node->left  = nullptr;
                node->right = nullptr;

                if (GET_OP (node) == OP_MUL) CHANGE_DATA_ON_VAL (node, 0.0);
                if (GET_OP (node) == OP_DEG) CHANGE_DATA_ON_VAL (node, 1.0);
                
                return 1;
            }
        }

        if (Is_given_value_node (node->right, 1))
        {
            if (GET_OP (node) == OP_MUL || GET_OP (node) == OP_DEG || GET_OP (node) == OP_SUB)
            {
                Node *node_cpy = Tree_copy (node->left);
                My_swap (node, node_cpy, sizeof (Node));
                Free_differentiator_nodes_data (node_cpy);
        
                return 1;
            }
        }

        if (Is_given_value_node (node->left, 0))
        {
            if (GET_OP (node) == OP_ADD)
            {
                Node *node_cpy = Tree_copy (node->right);
                My_swap (node, node_cpy, sizeof (Node));
                Free_differentiator_nodes_data (node_cpy);

                return 1;
            }

            if (GET_OP (node) == OP_MUL || GET_OP (node) == OP_DEG || GET_OP (node) == OP_SUB)
            {
                Free_differentiator_nodes_data (node->left);
                Free_differentiator_nodes_data (node->right);
                
                node->left  = nullptr;
                node->right = nullptr;

                CHANGE_DATA_ON_VAL (node, 0.0);

                return 1;
            }

        }

        if (Is_given_value_node (node->left, 1))
        {
            if (GET_OP (node) == OP_MUL)
            {
                Node *node_cpy = Tree_copy (node->right);
                My_swap (node, node_cpy, sizeof (Node));
                Free_differentiator_nodes_data (node_cpy);
        
                return 1;
            }

            if (GET_OP (node) == OP_DEG)
            {
                
                Free_differentiator_nodes_data (node->left);
                Free_differentiator_nodes_data (node->right);
                
                node->left  = nullptr;
                node->right = nullptr;

                CHANGE_DATA_ON_VAL (node, 1.0);

                return 1;
            }
        }
    }

    return 0;
}

//======================================================================================

static int Is_given_value_node (const Node *node, double val)
{
    return (!Check_nullptr ((char*) node) && IS_VAL (node) && Equality_double (GET_VAL (node), val));
}

//======================================================================================

static int Convolution_of_constants (Node* node)
{
    assert (node != nullptr && "null is nullptr");  
 
    if (IS_OP (node))
    {   
        if (IS_VAL (node->left) && (!Check_nullptr (node->right) && IS_VAL (node->right)))
        {
            double val_l = GET_VAL (node->left), val_r = GET_VAL (node->right);
            double res_val = NAN;

            if (GET_OP (node) == OP_MUL)
                res_val = val_l * val_r;        

            if (GET_OP (node) == OP_ADD)
                res_val = val_l + val_r; 

            if (GET_OP (node) == OP_SUB)
                res_val = val_l - val_r; 

            if (GET_OP (node) == OP_DIV)
                if (!Is_zero (val_r)) 
                    res_val = val_l / val_r; 

            if (GET_OP (node) == OP_DEG)
                res_val = powl (val_l, val_r); 

            CHANGE_DATA_ON_VAL (node, res_val);

            Free_differentiator_nodes_data (node->left);
            Free_differentiator_nodes_data (node->right);

            node->left  = nullptr;
            node->right = nullptr;

            return 1;
        }

        #ifdef CALC_CONST
            if (IS_VAL (node->left) && Check_nullptr (node->right))
            {
                double val_l = GET_VAL (node->left);
                double res_val = NAN;

                if (GET_OP (node) == OP_SIN)
                    res_val = sin (val_l);

                if (GET_OP (node) == OP_COS)
                    res_val = cos (val_l);

                if (GET_OP (node) == OP_LOG)
                    if (!Is_zero (val_l) && val_l > 0.0)
                        res_val = log (val_l);

                
                CHANGE_DATA_ON_VAL (node, res_val);

                Free_differentiator_nodes_data (node->left);

                node->left  = nullptr;

                return 1;
            }
        #endif
    }

    return 0;
}