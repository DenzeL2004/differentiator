#ifndef _DIFFERENTIATOR_H_
#define _DIFFERENTIATOR_H_

#include "differentiator_config.h"

#include "differentiator_tree/differentiator_tree.h"

enum Differentiator_func_err
{
    FREE_NODE_DATA_ERR      = -1,
    
    SAVING_DATABASE_ERR     = -2,
    SAVING_NODE_ERR         = -3,

    READ_DATABASE_ERR       = -4,
    READ_NODE_ERR           = -5,
};


struct Differentiator_struct
{
    char *database = nullptr;

    Tree tree = {};
};

/*
int Save_database_to_file (const Differentiator_struct *expression, 
                           const char *name_output_file = Default_output_file_name);

int Load_database (Differentiator_struct *expression, const char *name_input_file);

*/
#endif //#endif _DIFFERENTIATOR_H_