#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 
#include <math.h>
#include <time.h>


#include "differentiator.h"
#include "differentiator_tree/draw_tree.h"

#include "differentiator_reader/reader.h"
#include "differentiator_simplifier/simplifier.h"


#include "differntiator_dsl.h"

#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"

#include "../src/process_text/process_text.h"




static Node* Differentiate_node (Node* node, const char *var);


static int Init_name_table (Differentiator_struct *expression);

static int Search_name_object (Node *node, Name_table *name_table);

static Node *Taylor_term (FILE *fdout, Differentiator_struct *expression, 
                                        const char* var, const int order);

static int Print_modes ();


//======================================================================================

int Differentiator_struct_ctor (Differentiator_struct *expression)
{
    assert (expression != nullptr && "expression is nullptr");

    if (Tree_ctor (&expression->tree))
        return PROCESS_ERROR (TREE_CTOR_ERR, "Ctor expression's tree"
                                             " in differentiator ctor\n");
   
    expression->input_database = nullptr;
    expression->copy_database  = nullptr;

    if (Name_table_ctor (&expression->name_table))
        return PROCESS_ERROR (NAME_TABLE_CTOR_ERR, "Dtor expression's name_table"
                                                     " in differentiator ctor\n");

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

    if (Name_table_dtor (&expression->name_table))
        return PROCESS_ERROR (NAME_TABLE_CTOR_ERR, "Ctor expression's name_table"
                                                      " in differentiator dtor\n");

    return 0;
}


//======================================================================================

int Expression_processing (Differentiator_struct *expression)
{
    assert (expression != nullptr && "expression is nullptr");
    
    Tree tmp_expression = {};

    if (Tree_ctor (&tmp_expression))
        return PROCESS_ERROR (TREE_CTOR_ERR, "Ctor tree tmp_expression\n");

    tmp_expression.root = Tree_copy (expression->tree.root);
    
    FILE *tex = Latex_start ();
    Print_latex_message (tex, "\\textbf{Пред нами предстоит задача полностью расшарить данное выражение:}\n");
    Print_latex_tree (tex, &tmp_expression); 

    Print_modes ();

    while (true)
    {
        printf ("Enter command: ");

        char cur_cmd[Max_command_buffer] = "";

        if (scanf ("%s", cur_cmd) != 1)
            return PROCESS_ERROR (PROCESS_EXPRESSION_ERR, "Error reading the operating "
                                                          "mode of the differentiator\n");
            
        if (!strcmpi ("Differentiate", cur_cmd) || !strcmpi ("Di", cur_cmd))     
        {                                       
            int order = 0;
            char cur_variable[Max_command_buffer] = "";

            printf ("Enter the number of the derivative and by which variable to differentiate.\n"); 
            int res_scan = scanf ("%d%s", &order, cur_variable);

            if (res_scan != 2)
                return PROCESS_ERROR (PROCESS_EXPRESSION_ERR, 
                                     "Reading input parameters, res_scan = %d\n"
                                      "order = %d, var = %s", cur_variable);

            Differentiate_expression (tex, &expression->tree, &tmp_expression, cur_variable, order, PRINT);
        }   

        else if (!strcmpi ("Draw", cur_cmd) || !strcmpi ("Dr", cur_cmd))
        {
            if (Draw_database (&tmp_expression))
                return PROCESS_ERROR (DRAW_DATABASE_ERR, "Print database error\n");
        }

        else if (!strcmpi ("Calculate", cur_cmd) || !strcmpi ("C", cur_cmd))     
        {                                 
            double res = Calc_expression (tmp_expression.root, &expression->name_table);
            printf ("Calculation result: %.5lg\n", res);
        } 

        else if (!strcmpi ("Taylor", cur_cmd) || !strcmpi ("T", cur_cmd))     
        {                                       
            int order = 0;
            char cur_variable[Max_command_buffer] = "";

            printf ("Enter the number of the derivative and by which variable to decompose.\n"); 
            int res_scan = scanf ("%d%s", &order, cur_variable);

            if (res_scan != 2)
                return PROCESS_ERROR (PROCESS_EXPRESSION_ERR, 
                                     "Reading input parameters, res_scan = %d\n"
                                      "order = %d, var = %s", cur_variable);

            Taylor_expansion (tex, expression, &tmp_expression, cur_variable, order, PRINT);
        } 

        else if (!strcmpi ("Modes", cur_cmd) || !strcmpi ("M", cur_cmd))     
        {                                       
            Print_modes ();            
        } 

        else if (!strcmpi ("Exit", cur_cmd) || !strcmpi ("E", cur_cmd))     
        {                                       
            break;      
        } 
        
        else
            printf ("Unknown mode\n");
    }

    if (Tree_dtor (&tmp_expression))
        return PROCESS_ERROR (TREE_DTOR_ERR, "Dtor tree tmp_expression\n");

    Latex_finish (tex);
    return 0;
}

//======================================================================================

static int Print_modes ()
{
    printf ("\n");

    printf ("1. [Di]fferentiate. Differentiate initial math expression.\n");
    printf ("2. [Dr]aw. Draws last transformation expression tree.\n");
    printf ("3. [C]alculate. Dot last transformation expression calculation.\n");
    printf ("4. [T]aylor. Print Taylor polynomial with given variable initial expression.\n");
    printf ("5. [M]odes. Print mode List.\n");
    printf ("6. [E]xit. Finish the processing.\n");

    printf ("\n");

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

int Differentiate_expression (FILE* fdout, Tree *math_expression, Tree *dif_expression, 
                              const char* var, const int derivative_number, const int print_mode)
{
    assert (math_expression != nullptr && "math expression is nullptr");
    assert (dif_expression  != nullptr && "dif expresion   is nullptr");
    assert (var             != nullptr && "var is nullptr");
    assert (fdout           != nullptr && "assert is nullptr");

    if (derivative_number < 0)
    {
        PROCESS_ERROR (DIFFERENT_EXPRESSION_ERR, "Derivative of negative order\n");
        return DIFFERENT_EXPRESSION_ERR;
    }

    Free_differentiator_nodes_data (dif_expression->root);
    dif_expression->root = Tree_copy (math_expression->root);
    Simplifier_expression (dif_expression);

    if (print_mode)
    {
        Print_latex_message (fdout, "\\textbf{Возьмем %d-ую производную по"
                                    " аргументу \'%s\' исходного выражения:}\n", 
                                                         derivative_number, var);     
        Print_latex_tree (fdout, dif_expression);
    }

    srand (time (NULL));
    for (int id = 1; id <= derivative_number; id++)
    {
        Node *last_dif_node = dif_expression->root;

        dif_expression->root = Differentiate_node (dif_expression->root, var);

        Free_differentiator_nodes_data (last_dif_node);

        if (print_mode)
        {
            Print_latex_message (fdout, "%d-ая производная:\n", id);
            Print_latex_tree (fdout, dif_expression);
        }

        Simplifier_expression (dif_expression);

        if (print_mode)
        {
            int id_phrase = rand() % Cnt_linking_words;
            Print_latex_message (fdout, "%s\n", Linking_words[id_phrase]);
            Print_latex_tree (fdout, dif_expression);
        }

        if (IS_VAL (dif_expression->root) && Is_zero (GET_VAL (dif_expression->root)))
        {
            if (print_mode)
                Print_latex_message (fdout, "Больше %d-ой производной выражение не имеет.\n");
            break;
        }
    }
    
    return 0;
}

//======================================================================================

static Node* Differentiate_node (Node* node, const char *var)
{
    assert (node != nullptr && "node is nullptr");
    assert (var  != nullptr && "var  is nullptr");

    Differentiator_data *node_data = (Differentiator_data*) node->data;

    switch (node_data->node_type)
    {
        case VALUE_T:    return CREATE_VAL (0);

        case VARIABLE_T:
            if (!strcmp (GET_VAR (node), var)) 
                return CREATE_VAL (1);
            else
                return CREATE_VAL (0); 
        
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
                        return DIF (DEG (CREATE_VAL (exp (1)), MUL (LOG (CL), CR)));
                    
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

int Taylor_expansion (FILE* fdout, Differentiator_struct *expression, Tree *taylor_expansion,
                      const char* var, const int term_number, const int print_mode)
{
    assert (expression       != nullptr && "expression is nullptr");
    assert (taylor_expansion != nullptr && "taylor_expansion is nullptr");
    assert (var              != nullptr && "var is nullptr");
    assert (fdout            != nullptr && "assert is nullptr");

    if (print_mode)
    {
        Print_latex_message (fdout, "\\textbf{Разложим до %d-ого члена ряда Тейлора выражение:}\n", 
                                                                                        term_number);
        Print_latex_tree (fdout, &expression->tree);
    }

    Free_differentiator_nodes_data (taylor_expansion->root);

    taylor_expansion->root = Taylor_term (fdout, expression, var, 0);

    for (int id = 1; id <= term_number; id++)
    {
        Node *next_term =  Taylor_term (fdout, expression, var, id);
        taylor_expansion->root = Create_operation_node (OP_ADD, taylor_expansion->root, next_term);
    }

    Simplifier_expression (taylor_expansion);
    
    if (print_mode)
    {
        Print_latex_message (fdout, "Разложение по переменой \'%s\':\n", var);
        Print_latex_tree (fdout, taylor_expansion);
    }

    return 0;
}

//======================================================================================

static Node *Taylor_term (FILE *fdout, Differentiator_struct *expression, 
                          const char* var, const int order)
{
    assert (fdout           != nullptr && "fdout is nullptr");
    assert (expression      != nullptr && "expression is nullptr");
    assert (var             != nullptr && "var is nullptr");
    
    Tree tmp_expression = {};

    if (Tree_ctor (&tmp_expression))
    {
        PROCESS_ERROR (TREE_CTOR_ERR, "Ctor tree tmp_expression\n");
        return nullptr;
    }
    
    Differentiate_expression (fdout, &expression->tree, &tmp_expression,
                              var, order, NOPRINT);
    
    double res = Calc_expression (tmp_expression.root, &expression->name_table);
    res /= Factorial (order);

    Node *coef_node = Create_value_node    (res, nullptr, nullptr);
    Node *var_node  = Create_variable_node (var, nullptr, nullptr);
    
    int id_var = Find_id_object (&expression->name_table, var);
    if (id_var == Not_init_object)
    {
        PROCESS_ERROR (TAYLOR_ERR, "cannot be decomposed into a variable \'%s\'\n", var);
        return nullptr;
    }
    double dot_val = *((double*) expression->name_table.objects[id_var].data);
    Node *dot_node = Create_value_node (dot_val, nullptr, nullptr);    

    Node *deg_nod = Create_operation_node (OP_DEG,  Create_operation_node (OP_SUB, var_node, dot_node), 
                                                    Create_value_node (order, nullptr, nullptr));

    Node* res_node = Create_operation_node (OP_MUL, coef_node, deg_nod);

    if (Tree_dtor (&tmp_expression))
    {
        PROCESS_ERROR (TREE_DTOR_ERR, "Dtor tree tmp_expression\n");
        return nullptr;
    }

    return res_node;
}

//======================================================================================

double Calc_expression (Node *node, Name_table *name_table)
{
    assert (node       != nullptr && "node is nullptr");
    assert (name_table != nullptr && "name_table is nullptr");

    if (Is_leaf_node (node))
    {
        if (IS_VAL (node))
            return GET_VAL (node);
        
        if (IS_VAR (node))
        {
            int id = Find_id_object (name_table, GET_VAR (node));
            if (id == Not_init_object)
            {
                PROCESS_ERROR (CALC_EXPRESSION_ERR, "Unknown variable %s\n", GET_VAR (node));
                return NAN;
            } 

            return *((double*) name_table->objects[id].data);
        }

        PROCESS_ERROR (CALC_EXPRESSION_ERR, "operator (%s) without parameters\n",  Name_operations [GET_OP (node)]);
        return NAN;
    }

    double val_left = Calc_expression (node->left, name_table);   

    double val_right = NAN;
    if (!Check_nullptr (node->right))
        val_right = Calc_expression (node->right, name_table);

    double res_val = NAN;

    switch (GET_OP (node))
    {
        case OP_ADD:
            res_val = val_left + val_right;
            break;

        case OP_SUB:
            res_val = val_left - val_right;
            break;
        
        case OP_MUL:
            res_val = val_left * val_right;
            break;
        
        case OP_DIV:
            if (Is_zero (val_right))    
                res_val = NAN;
            else
                res_val = val_left / val_right;
            break;
        
        case OP_DEG:
            if (isnan (val_left) || isnan (val_right)) break;;

            res_val = pow (val_left, val_right);
            break;

        case OP_SIN:
            res_val = sin (val_left);
            break;

        case OP_COS:
            res_val = cos (val_left);
            break;
        
        case OP_LOG:
            if (Is_zero (val_left) || val_left < 0)
                res_val = NAN;
            else
                res_val = log (val_left);
            break;

        default:
            break;
    }

    return res_val;
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

    free ((char*) expression->input_database);
    free (expression->copy_database);

    expression->input_database = strdup (text.text_buf);
    expression->copy_database  = strdup (text.text_buf);

    Free_buffer (&text);

    int result_parce = Parce_math_expression (&expression->tree, 
                                               expression->input_database, expression->copy_database);
    if (result_parce)
        return PROCESS_ERROR (LOAD_DATABASE_ERR, "Parce result = %d", result_parce);

    int result_init = Init_name_table (expression);
    if (result_init)
        return PROCESS_ERROR (INIT_NAME_TABLE_ERR, "name initialization ended with %d", result_init);

    return 0;
}

//======================================================================================

static int Init_name_table (Differentiator_struct *expression)
{
    assert (expression != nullptr && "expression is nullptr");

    int result_get_name = Search_name_object (expression->tree.root, &expression->name_table);
    if (result_get_name)
        return PROCESS_ERROR (GET_NAME_OBJECT_ERR, "Getting object names ended with %d\n", result_get_name);

    if (Check_nullptr (&expression->name_table))
        return PROCESS_ERROR (ERR_NULLPTR, "Name table is nullptr\n");

    for (int id = 0; id < expression->name_table.cnt_object; id++)
    {
        switch (expression->name_table.objects[id].type)
        {
            case OBJ_VARIABLE_T:
                {
                    printf ("%s = ", expression->name_table.objects[id].name);
        
                    double val = NAN;
                    scanf ("%lg", &val);

                    if (isnan (val)) 
                        return PROCESS_ERROR (ERR_READ, "Value was not read. Val = %lg\n", val);

                    expression->name_table.objects[id].data = calloc (1, sizeof (double));
                    
                    *((double*) expression->name_table.objects[id].data) = val;
                }
                break;
            
            default:
                return PROCESS_ERROR (OBJ_UNKNOWN_T, "Unknown object type\n");
        }
    }

    return 0;
}

//======================================================================================

static int Search_name_object (Node *node, Name_table *name_table)
{
    assert (node != nullptr && "node is nullptr");
    assert (name_table != nullptr && "name_table is nullptr");

    if (!Check_nullptr (node->left))
        if (Search_name_object (node->left, name_table))
            return PROCESS_ERROR (GET_NAME_OBJECT_ERR, "left son worked incorrectly");

    if (!Check_nullptr (node->right))
        if (Search_name_object (node->right, name_table))
            return PROCESS_ERROR (GET_NAME_OBJECT_ERR, "right son worked incorrectly");

    if (IS_VAR (node))
        if (Add_object (name_table, GET_VAR (node), OBJ_VARIABLE_T) == ADD_OBJECT_ERR)
            return PROCESS_ERROR (ADD_OBJECT_ERR, "add name variable node = |%p|", (char*) node);

    return 0;
}

//======================================================================================
