#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 
#include <math.h>


#include "differentiator.h"
#include "differentiator_tree/draw_tree.h"

#include "differentiator_reader/reader.h"
#include "differentiator_simplifier/simplifier.h"


#include "differntiator_dsl.h"

#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"

#include "../src/stack/stack.h"
#include "../src/process_text/process_text.h"



static Node* Differentiate_node (Node* node);


static int Read_expression_from_buffer (Node *node, Text_info *text);

static int Read_node_from_buffer (Node *node, Text_info *text);


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

//======================================================================================

int Simplifier_expression (Tree *math_expresion)
{
    assert (math_expresion != nullptr && "math expression is nullptr");

    while (Simplifier (math_expresion->root))
        continue;


    return 0;
}

//======================================================================================

int Differentiate_expression (Tree *math_expression, Tree *dif_expression, const int derivative_number)
{
    assert (math_expression != nullptr && "math expression is nullptr");
    assert (dif_expression  != nullptr && "dif expresion   is nullptr");

    if (derivative_number < 0)
    {
        PROCESS_ERROR (DIFFERENT_EXPRESSION_ERR, "Derivative of negative order\n");
        return DIFFERENT_EXPRESSION_ERR;
    }

    Free_differentiator_nodes_data (dif_expression->root);
    dif_expression->root = Tree_copy (math_expression->root);

    for (int id = 1; id <= derivative_number; id++)
    {
        Node *last_dif_node = dif_expression->root;

        dif_expression->root = Differentiate_node (dif_expression->root);

        Free_differentiator_nodes_data (last_dif_node);

        if (IS_VAL (dif_expression->root) && Is_zero (GET_VAL (dif_expression->root))) ///print id < derivative_number??
            break;

    }
    Draw_database (dif_expression);
    Simplifier_expression (dif_expression);

    return 0;
}

//======================================================================================

static Node* Differentiate_node (Node* node)
{
    assert (node != nullptr && "node is nullptr");

    Differentiator_data *node_data = (Differentiator_data*) node->data;

    switch (node_data->node_type)
    {
        case VALUE_T:    return CREATE_VAL (0);

        case VARIABLE_T: return CREATE_VAL (1);
        
        case OPERATION_T: 
            switch (node_data->data.operation)
            {
                case OP_ADD: return ADD (DL, DR);
                case OP_SUB: return SUB (DL, DR);

                case OP_MUL: return ADD (MUL (DL, CR), MUL (CL, DR));
                case OP_DIV: return DIV (SUB (MUL (DL, CR), MUL (CL, DR)), MUL (CR, CR));

                case OP_SIN: return MUL (COS (CL), DL);
                case OP_COS: return MUL (CREATE_VAL (-1), MUL (SIN (CL), DL));

                case OP_LOG:
                    return MUL (DIV (CREATE_VAL (1), CL), DL);

                case OP_DEG:
                    if (IS_VAL (LEFT) && IS_VAL (RIGHT))
                        return CREATE_VAL (0);

                    else if (IS_VAL (LEFT) && IS_FUNC (RIGHT))
                        return MUL (DEG (CL, CR), MUL (LOG (CL), DR));

                    else if (IS_FUNC (LEFT) && IS_VAL (RIGHT))
                        return MUL (CR, MUL (DEG (CL, CREATE_VAL (GET_VAL (RIGHT) - 1)), DL));
                    
                    else if (IS_FUNC (LEFT) && IS_FUNC (RIGHT))
                        return Differentiate_node (DEG (CREATE_VAL (exp (1)), MUL (LOG (CL), CR)));
                    
                    else
                    {
                        Err_report ("Unknown expressions\n");
                        return nullptr;
                    }
                
                default:
                {
                    Err_report ("Unknown expressions\n");
                    return nullptr;
                }

            }
        
        default:
        {
            Err_report ("Unknown node type = %d\n", node_data->node_type);
            return nullptr;
        }

    }

    return nullptr;
}

//======================================================================================

int Draw_database (Tree *tree, const int node_mode)
{
    assert (tree != nullptr && "tree is nullptr");

    static int Cnt_graphs = 0;      //<-To display the current tree view

    char name_output_file[Max_command_buffer] = "";
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

    Simplifier_expression (&expression->tree);

    if (result_parce)
        return PROCESS_ERROR (LOAD_DATABASE_ERR, "Parce result = %d", result_parce);

    return 0;
}

//======================================================================================