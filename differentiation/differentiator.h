#ifndef _DIFFERENTIATOR_H_
#define _DIFFERENTIATOR_H_

#include "operation_list.h"

#include "differentiator_tree/differentiator_tree.h"
#include "differentiator_tree/draw_tree.h"

#include "name_table/name_table.h"

enum Differentiator_func_err
{
    DIFFERENTIATOR_CTOR_ERR     = -1,
    DIFFERENTIATOR_DTOR_ERR     = -2,
    
    DIFFERENT_EXPRESSION_ERR    = -3,

    PROCESS_EXPRESSION_ERR      = -4,

    LOAD_DATABASE_ERR           = -5,

    DRAW_DATABASE_ERR           = -7,

    INIT_NAME_TABLE_ERR         = -8,
    GET_NAME_OBJECT_ERR         = -9,

    CALC_EXPRESSION_ERR         = -10,
};


struct Differentiator_struct
{
    const char *input_database = nullptr;
          char *copy_database  = nullptr;

    Name_table name_table = {};

    Tree tree = {};
};



int Differentiator_struct_ctor (Differentiator_struct *expression);

int Differentiator_struct_dtor (Differentiator_struct *expression);


int Expression_processing (Differentiator_struct *expression);


int Differentiate_expression (Tree *math_expression, Tree *dif_expression, 
                              const char* var, const int derivative_number, FILE* fdout);

double Calc_expression (Node *node, Name_table *name_table);


int Simplifier_expression (Tree *math_expresion);


int Draw_database (Tree *tree, const int node_mode = Mask_draw_node_modes);

int Load_database (Differentiator_struct *expression, const char *name_input_file);


#endif //#endif _DIFFERENTIATOR_H_