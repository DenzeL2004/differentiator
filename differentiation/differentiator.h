#ifndef _DIFFERENTIATOR_H_
#define _DIFFERENTIATOR_H_

#include "operation_list.h"

#include "differentiator_tree/differentiator_tree.h"
#include "differentiator_tree/draw_tree.h"

enum Differentiator_func_err
{
    DIFFERENTIATOR_CTOR_ERR     = -1,
    DIFFERENTIATOR_DTOR_ERR     = -2,
    
    DIFFERENT_EXPRESSION_ERR    = -3,

    LOAD_DATABASE_ERR           = -5,

    DRAW_DATABASE_ERR           = -7,
};


struct Differentiator_struct
{
    const char *input_database = nullptr;
          char *copy_database  = nullptr;

    Tree tree = {};
};



int Differentiator_struct_ctor (Differentiator_struct *expression);

int Differentiator_struct_dtor (Differentiator_struct *expression);


int Differentiate_expression (Tree *math_expression, Tree *dif_expression, const int derivative_number);


int Draw_database (Tree *tree, const int node_mode = Mask_draw_node_modes);

int Load_database (Differentiator_struct *expression, const char *name_input_file);


#endif //#endif _DIFFERENTIATOR_H_