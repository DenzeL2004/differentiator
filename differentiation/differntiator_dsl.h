#ifndef _DIFFERENTIATOR_DSL_H_
#define _DIFFERENTIATOR_DSL_H_

#define CREATE_VAL(val) Create_value_node ((val), nullptr, nullptr)

#define LEFT    node->left
#define RIGHT   node->right

#define DL         Differentiate_node (LEFT,  var)
#define DR         Differentiate_node (RIGHT, var)
#define DIF(node)  Differentiate_node (node, var)


#define CL  Tree_copy (LEFT)
#define CR  Tree_copy (RIGHT)

#define ADD(left, right)    Create_operation_node (OP_ADD, left, right)
#define SUB(left, right)    Create_operation_node (OP_SUB, left, right)

#define MUL(left, right)    Create_operation_node (OP_MUL, left, right)
#define DIV(left, right)    Create_operation_node (OP_DIV, left, right)

#define DEG(left, right)    Create_operation_node (OP_DEG, left, right)

#define SIN(left)  Create_operation_node (OP_SIN, left, nullptr)
#define COS(left)  Create_operation_node (OP_COS, left, nullptr)

#define LOG(left)  Create_operation_node (OP_LOG, left, nullptr)

#define IS_VAL(node) (((Differentiator_data*) (node)->data)->node_type == VALUE_T)
#define IS_OP(node)  (((Differentiator_data*) (node)->data)->node_type == OPERATION_T)
#define IS_VAR(node) (((Differentiator_data*) (node)->data)->node_type == VARIABLE_T)

#define IS_FUNC(node) (IS_OP(node) || IS_VAR(node))

#define GET_VAL(node) (((Differentiator_data*) (node)->data)->data.val)
#define GET_OP(node)  (((Differentiator_data*) (node)->data)->data.operation)
#define GET_VAR(node) (((Differentiator_data*) (node)->data)->data.var)

#endif //_DIFFERENTIATOR_DSL_H_