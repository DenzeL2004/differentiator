#ifndef _DIFFERENTIATOR_TREE_H_
#define _DIFFERENTIATOR_TREE_H_

#include "../differentiator_config.h"

#include "../../src/tree/tree.h"


enum Node_type
{
    UNKNOWN_T   = -1,

    VALUE_T     =  0,
    OPERATION_T =  1,
    VARIABLE_T  =  2,
};


struct Differentiator_node
{
    Node_type node_type;
    
    union Node_data
    {
        double                   val;
        const char*              var;
        Differentiator_operation operation;
    } data;
};


enum Differentiator_node_func_err
{
    CREATE_EMPTY_NODE_ERR   = -1,

    GET_VALUE_NODE_ERR      = -2,
    GET_OPERATION_NODE_ERR  = -3,
    GET_VARIABLE_NODE_ERR   = -4,
};


/**
 * @brief  Creates an empty tree node 
 * @return Node* pointer to the created node
 */
Differentiator_node* Create_differentiatoer_node ();



int Get_operation_node (Node *node, Differentiator_operation operation);

int Get_value_node (Node *node, double value);

int Get_variable_node (Node *node, const char* nmae_variable);


/** 
 * @brief Removes a node with all information
 * @param [in] node Pointer to Node pointer
 * @return Returns zero if the deletion was successful, otherwise returns a non-zero number
*/
int Free_differentiatoer_nodes_data (Node *node);



#endif //#endif _DIFFERENTIATOR_TREE_H_