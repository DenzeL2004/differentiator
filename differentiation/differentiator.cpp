#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 


#include "differentiator.h"
#include "differentiator_tree/draw_tree.h"
#include "differentiator_reader/reader.h"


#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"

#include "../src/stack/stack.h"
#include "../src/process_text/process_text.h"



static int Read_expression_from_buffer (Node *node, Text_info *text);

static int Read_node_from_buffer (Node *node, Text_info *text);


static int Determine_node_type (Node *node, const char* str);

static int Node_is_val       (const char* str);

static int Node_is_var       (const char* str);

static int Node_is_operation (const char *str);



/*

static int Save_expression_to_file (const Node *node, FILE *fpout);

*/

//======================================================================================

int Differentiator_struct_ctor (Differentiator_struct *expression)
{
    assert (expression != nullptr && "expression is nullptr");

    if (Tree_ctor (&expression->tree))
        return PROCESS_ERROR (TREE_CTOR_ERR, "Ctor expression's tree"
                                             " in differentiator ctor\n");
   
    expression->input_database = nullptr;
    expression->copy_database = nullptr;

    return 0;
}

//======================================================================================

int Differentiator_struct_dtor (Differentiator_struct *expression)
{
    assert (expression != nullptr && "expression is nullptr");

    if (Tree_ctor (&expression->tree))
        return PROCESS_ERROR (TREE_DTOR_ERR, "Dtor expression's tree"
                                             " in differentiator dtor\n");
    
    free ((char*) expression->input_database);
    expression->input_database = nullptr;

    free (expression->copy_database);
    expression->copy_database = nullptr;

    return 0;
}

Differentiator_struct* Differentiate_expression (Differentiator_struct *expression)
{
    assert (expression != nullptr && "expression is nullptr");


}

/*
//======================================================================================

int Save_database_to_file (const Differentiator_struct *expression, const char *name_output_file)
{
    assert (expression != nullptr && "expression is nullptr");


    FILE *fpout = Open_file_ptr (name_output_file, "w");

    if (Check_nullptr (fpout)) 
        return PROCESS_ERROR (ERR_FILE_OPEN, "Error opening output file named \"%s\"\n", name_output_file);

    if (Save_expression_to_file (expression->tree.root, fpout))
        return PROCESS_ERROR (SAVING_DATABASE_ERR, "Error saving a tree with a pointer to the root |%p|\n", 
                                                                            (char*) expression->tree.root);

    if (Close_file_ptr (fpout)) 
        return PROCESS_ERROR (ERR_FILE_CLOSE, "Error close output file named \"%s\"\n", name_output_file);

    return 0;
}

//======================================================================================

static int Save_expression_to_file (const Node *node, FILE *fpout)
{
    assert (node != nullptr && "node is nullptr");
    assert (fpout != nullptr && "fpout is nullptr");

    if (Is_leaf_node (node))
        fprintf (fpout, " { %s } ", node->data);

    else
    {
        fprintf(fpout, " { %s", node->data);

        if (!Check_nullptr (node->left)) 
        {
            if (Save_expression_to_file(node->left, fpout))
                return SAVING_NODE_ERR;
        }

        if (!Check_nullptr (node->right)) 
        {
            if (Save_expression_to_file(node->right, fpout))
                return SAVING_NODE_ERR;
        }

        fprintf(fpout, " } ");
    }

    return 0;
}

//======================================================================================
*/

//======================================================================================

int Print_database (Tree *tree, const int node_mode)
{
    assert (tree != nullptr && "tree is nullptr");

    static int Cnt_graphs = 0;      //<-To display the current tree view

    char name_output_file[Max_command_buffer] = {0};
    sprintf (name_output_file, "graph_img\\picture%d.png", Cnt_graphs); 

    Cnt_graphs++;

    if (Draw_tree_graph (tree, name_output_file, node_mode))
        return PROCESS_ERROR (DRAW_DATABASE_ERR, "Error in graph drawing\n");

    char command_buffer[Max_command_buffer] = {0};
    sprintf(command_buffer, "@temp\\%s", name_output_file);

    if (system (command_buffer))
        return PROCESS_ERROR (DRAW_DATABASE_ERR, "Failed to open image\n");

    return 0;
}

//======================================================================================

int Load_database (Differentiator_struct *expression, const char *name_input_file)
{
    assert (expression != nullptr && "expression is nullptr");
    assert (name_input_file != nullptr && "name_input_file is nullptr");

    int fdin = Open_file_discriptor (name_input_file, O_RDONLY);
    if (fdin < 0)
        return PROCESS_ERROR (ERR_FILE_OPEN, "Error opening iputfile file named \"%s\"\n", 
                                                                           name_input_file);

    Text_info text = {};
    
    if (Text_read (fdin, &text))
        return PROCESS_ERROR (ERR_FILE_READING, "Error reading into Text_info structure\n");

    if (Close_file_discriptor (fdin))
        return PROCESS_ERROR (ERR_FILE_CLOSE, "Error close input file named \"%s\"\n", name_input_file);


    expression->input_database = strdup (text.text_buf);
    expression->copy_database  = strdup (text.text_buf);

    Free_buffer (&text);

    int result_parce = Parce_math_expression (&expression->tree, 
                                               expression->input_database, expression->copy_database);

    if (result_parce)
        return PROCESS_ERROR (LOAD_DATABASE_ERR, "Parce result = %d", result_parce);

    return 0;
}

//======================================================================================