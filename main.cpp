#include <stdio.h>
#include <string>


#include "differentiation/differentiator.h"
#include "src/log_info/log_errors.h"
#include "src/Generals_func/generals.h"


int main (int argc, char *argv[])
{   
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif

    Differentiator_struct expression = {};

    if (Differentiator_struct_ctor (&expression))
        return PROCESS_ERROR (DIFFERENTIATOR_CTOR_ERR, 
                              "Ctor Differentiator struct expression\n");


    switch (argc)
    {
        case 1:
            printf ("No file to read.\nSpecify the file from which you want to read\n");
            return 0;

        case 2:
        {
            Load_database (&expression, argv[1]);
            break;
        }

        default:
            Log_report ("Too many command line arguments\n");
            Err_report ("Too many command line arguments\n");
            return -1;
    }

    Draw_database (&expression.tree);

        
    Tree dif_expression = {};

    if (Tree_ctor (&dif_expression))
        return PROCESS_ERROR (TREE_CTOR_ERR, 
                              "Ctor tree dif_expression\n");
    
    Differentiate_expression (&expression.tree, &dif_expression, 2);
    
    Draw_database (&dif_expression);


    if (Tree_dtor (&dif_expression))
        return PROCESS_ERROR (TREE_DTOR_ERR, 
                              "Dtor tree dif_expression\n");

    

    if (Differentiator_struct_dtor (&expression))
        return PROCESS_ERROR (DIFFERENTIATOR_DTOR_ERR, 
                              "Dtor Differentiator struct expression\n");
    
    
    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif
}