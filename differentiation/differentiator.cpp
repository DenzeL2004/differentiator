#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "differentiator.h"


#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"

#include "../src/stack/stack.h"
#include "../src/process_text/process_text.h"




/*static int Print_database (Tree *tree);



static int Save_expression_to_file (const Node *node, FILE *fpout);


static int Read_expression_from_buffer (Node *node, Text_info *text);

static int Read_node_from_buffer (Node *node, Text_info *text);

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
    
    expression->database = text.text_buf;
    text.pos = 0;

    if (Read_expression_from_buffer (expression->tree.root, &text))
        return PROCESS_ERROR (READ_DATABASE_ERR, "Error read a tree with a pointer to the root |%p|\n", 
                                                                          (char*) expression->tree.root);
    
    if (Close_file_discriptor (fdin))
        return PROCESS_ERROR (ERR_FILE_CLOSE, "Error close input file named \"%s\"\n", name_input_file);


    return 0;
}

//======================================================================================

static int Read_expression_from_buffer (Node *node, Text_info *text)
{
    assert (text != nullptr && "text is nullptr");

    if (Read_node_from_buffer (node, text))
        return PROCESS_ERROR (READ_NODE_ERR, "Error reading the value of node = |%p|", (char*) node);    
    
    if (*(text->text_buf + text->pos) == '}') 
    {
        text->pos++;
        return 0;
    }

    if (*(text->text_buf + text->pos) == '{') {
        
        if (Add_node_sons (node))
            return PROCESS_ERROR (READ_NODE_ERR, "Error adding children of a node = |%p|", (char*) node);
        

        if (Read_expression_from_buffer (node->left, text))
            return PROCESS_ERROR (READ_NODE_ERR, "Error reading the value of the left son = |%p|",
                                                                         (char*) node->left);
        
        if (Read_expression_from_buffer (node->right, text))
            return PROCESS_ERROR (READ_NODE_ERR, "Error reading the value of the right son = |%p|",
                                                        (char*) node->right);

        int shift = 0;
        char symbol = 0;

        int result_scanned = sscanf (text->text_buf + text->pos, " %c %n", &symbol, &shift);
        if (result_scanned != 1)
            return PROCESS_ERROR (READ_NODE_ERR, "Read error, was read: %d\n", result_scanned);
    
        
      
        if (symbol == '}') 
        {
            text->pos += shift;
            return 0;
        }

        else 
        {
            return PROCESS_ERROR (READ_NODE_ERR, "Read error, undefined symbol, was read: |%c|\n", symbol);
        }

    }
    
    return READ_NODE_ERR;
}

//======================================================================================

static int Read_node_from_buffer (Node *node, Text_info *text)
{
    assert (text != nullptr && "text is nullptr");

    int shift = 0;
    char symbol = 0;
    
    int result_scanned = sscanf (text->text_buf + text->pos, " %c %n", &symbol, &shift);
    if (result_scanned != 1) 
        return PROCESS_ERROR (READ_NODE_ERR, "Read error, was read: %d\n", result_scanned);
    
    if (symbol != '{') 
        return PROCESS_ERROR (READ_DATABASE_ERR, "Read error, undefined symbol, was read: |%c|\n", symbol);

    text->pos += shift;
    char *node_data = (char*) (text->text_buf + text->pos);

    symbol = 0;
    int len_word = 0;

    while (symbol != '{' && symbol != '}') 
    {
        result_scanned = sscanf (text->text_buf + text->pos, "%*s%n %n%c", 
                                                    &len_word, &shift, &symbol);
        if (result_scanned != 1) 
            return PROCESS_ERROR (READ_NODE_ERR, "Read error, was read: %d\n", result_scanned);
        
        text->pos += shift;
    }
    
    *(text->text_buf + text->pos - shift + len_word) = '\0';
    
    node->data = node_data;

    return 0;
}

//======================================================================================*/