#ifndef _DIFFERENTIATOR_H_
#define _DIFFERENTIATOR_H_

#include "differentiator_config.h"

#include "differentiator_tree/differentiator_tree.h"
#include "differentiator_tree/draw_tree.h"

enum Differentiator_func_err
{
    DIFFERENTIATOR_CTOR_ERR = -1,
    DIFFERENTIATOR_DTOR_ERR = -2,
    
    SAVING_DATABASE_ERR     = -3,
    SAVING_NODE_ERR         = -4,

    READ_DATABASE_ERR       = -5,
    READ_NODE_ERR           = -6,

    DRAW_DATABASE_ERR       = -7,
};


struct Differentiator_struct
{
    char *database = nullptr;

    Tree tree = {};
};



int Differentiator_struct_ctor (Differentiator_struct *expression);

int Differentiator_struct_dtor (Differentiator_struct *expression);


int Print_database (Tree *tree, const int node_mode = Mask_draw_node_modes);

/*
int Save_database_to_file (const Differentiator_struct *expression, 
                           const char *name_output_file = Default_output_file_name);
*/


int Load_database (Differentiator_struct *expression, const char *name_input_file);


#endif //#endif _DIFFERENTIATOR_H_