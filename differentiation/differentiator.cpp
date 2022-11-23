#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 
#include <math.h>


#include "differentiator.h"
#include "differentiator_tree/draw_tree.h"
#include "differentiator_reader/reader.h"

#include "differntiator_dsl.h"

#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"

#include "../src/stack/stack.h"
#include "../src/process_text/process_text.h"



static int Read_expression_from_buffer (Node *node, Text_info *text);

static int Read_node_from_buffer (Node *node, Text_info *text);



static Node* Create_val_node       (double val,   Node* left, Node* right);

static Node* Create_operation_node (int operation, Node* left, Node* right);



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


Node* Differentiate_expression (Node* node)
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

                case OP_DEG:
                    if (IS_VAL (LEFT) && IS_VAL (RIGHT))
                        return CREATE_VAL (0);

                    else if (IS_VAL (LEFT) && IS_FUNC (RIGHT))
                        return MUL (DEG (CL, CR), MUL (LOG (CL), DR));

                    else if (IS_FUNC (LEFT) && IS_VAL (RIGHT))
                        return MUL (CR, MUL (DEG (CL, CREATE_VAL (GET_VAL (RIGHT) - 1)), DL));
                    
                    else if (IS_FUNC (LEFT) && IS_FUNC (RIGHT))
                        return Differentiate_expression (DEG (CREATE_VAL (exp (1)), MUL (LOG (CL), CR)));
                    
                    else
                        Err_report ("Unknown expressions\n");
                
                case OP_LOG:
                    return MUL (DIV (CREATE_VAL (1), CL), DL);
            }
    }

    return nullptr;
}

//======================================================================================

static Node* Create_val_node (double val, Node* left, Node* right)
{
    Node *node = Create_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    if (Get_value_node (node, val))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Get node data val, node = |%p|\n", (char*) node);
        return nullptr;
    }

    node->left  = left;
    node->right = right;

    return node;
}

//======================================================================================

static Node* Create_operation_node (int operation, Node* left, Node* right)
{
    Node *node = Create_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    if (Get_operation_node (node, operation))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Get node data operation, node = |%p|\n", (char*) node);
        return nullptr;
    }

    node->left  = left;
    node->right = right;

    return node;

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

    if (result_parce)
        return PROCESS_ERROR (LOAD_DATABASE_ERR, "Parce result = %d", result_parce);

    return 0;
}

//======================================================================================