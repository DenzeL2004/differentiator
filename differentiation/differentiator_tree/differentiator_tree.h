#ifndef _DIFFERENTIATOR_TREE_H_
#define _DIFFERENTIATOR_TREE_H_

#include "../operation_list.h"

#include "../../src/tree/tree.h"


enum Node_type
{
    UNKNOWN_T   = -1,

    VALUE_T     =  0,
    OPERATION_T =  1,
    VARIABLE_T  =  2,
};


struct Differentiator_data
{
    Node_type node_type;
    
    union Node_data
    {
        double                   val;
        const char*              var;
        int                operation;
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
Differentiator_data* Init_differentiator_node ();


Node* Create_value_node     (const double value,    Node* left, Node* right);

Node* Create_operation_node (const int operation, Node* left, Node* right);

Node* Create_variable_node  (const char* variable, Node* left, Node* right);

Node *Tree_copy (Node *node);


/** 
 * @brief Removes a node with all information
 * @param [in] node Pointer to Node pointer
 * @return Returns zero if the deletion was successful, otherwise returns a non-zero number
*/
int Free_differentiator_nodes_data (Node *node);



#endif //#endif _DIFFERENTIATOR_TREE_H_