#include <stdio.h>
#include <assert.h>

#include "../../src/Generals_func/generals.h"
#include "../../src/log_info/log_errors.h"

#include "../differentiator_tree/differentiator_tree.h"
#include "../differntiator_dsl.h"

#include "latex_print.h"

static void Write_latex_title (FILE *tex);

static int Call_latex (FILE *tex);

static void Print_latex_node (FILE* tex, Node *node, int *shift);

static void Wrap_in_brackets (FILE *tex, Node *node, int *shift);

static void New_line (FILE *tex, const char *op, int *shift);

//======================================================================================

static void Write_latex_title (FILE *tex) 
{
    assert (tex != nullptr && "tex file is nullptr");
    
    fprintf (tex, "\\documentclass[a4paper, 12pt] {article}\n");
    fprintf (tex, "\\usepackage{graphicx}\n");
    fprintf (tex, "\\usepackage[utf8]{inputenc}\n");
    fprintf (tex, "\\usepackage{multirow}\n");
    fprintf (tex, "\\usepackage{mathtext}\n");
    fprintf (tex, "\\usepackage[T2A]{fontenc}\n");
    fprintf (tex, "\\usepackage{titlesec}\n");
    fprintf (tex, "\\usepackage{float}\n");
    fprintf (tex, "\\usepackage{empheq}\n");
    fprintf (tex, "\\usepackage{amsfonts}\n");
    fprintf (tex, "\\usepackage{amsmath}\n");

    fprintf (tex, "\\title{Семестровая работа по математическому анализу}\n");
    fprintf (tex, "\\author{Даниил Коломийцев \\thanks{спонсор В.В.Редкозубов}}\n");
    
    fprintf (tex, "\\date{November 2022}\n");

    fprintf (tex, "\\begin{document} \n");
    fprintf (tex, "\\maketitle\n");
    fprintf (tex, "\\newpage\n");

    fprintf (tex, "\n");

    return;
}

//======================================================================================

FILE* Latex_start  ()
{
    FILE *tex = Open_file_ptr (Tex_file, "w");
    if (Check_nullptr (tex))
    {
        PROCESS_ERROR (ERR_FILE_OPEN, "tex file didn't open");
        return nullptr;
    }

    Write_latex_title (tex);
    
    return tex;
}

//======================================================================================

int Latex_finish (FILE *tex) 
{
    assert (tex != nullptr && "tex is nullptr");

    fprintf(tex, "\n\\end{document}\n");

    if (Close_file_ptr (tex))
        return PROCESS_ERROR (ERR_FILE_CLOSE, "Tex file is't closed\n");

    if (Call_latex (tex))
        return PROCESS_ERROR (CREAT_TEX_FILE_ERR, "Pdf file not created");

    return 0;    
}

//======================================================================================

int Print_latex_message (FILE *tex, const char *format, ...)
{
    va_list args = {0};
   
    va_start(args, format);

    vfprintf(tex, format, args);
    
    va_end(args);

    return 0;
}

//======================================================================================

int Print_latex_tree (FILE *tex, Tree *expression)
{
    assert (tex        != nullptr && "tex is nullptr");
    assert (expression != nullptr && "expression is nullptr");

    fprintf (tex, "\n\\begin{math}\n");

    int shift = 0;
    Print_latex_node (tex, expression->root, &shift);
    
    fprintf (tex, "\n\\end{math}\n");
    fprintf (tex, "\\\\\n");
    fprintf (tex, "\\\\\n");

    return 0;
}

//======================================================================================

static void Print_latex_node (FILE* tex, Node *node, int *shift) 
{
    assert (tex   != nullptr && "tex is nullptr");
    assert (node  != nullptr && "node is nullptr");
    assert (shift != nullptr && "shift is nullptr");

    if (Is_leaf_node (node))
    {
        int offset = 0;
        if (IS_VAL (node))
        {
            if (GET_VAL (node) > 0.0 || Is_zero (GET_VAL (node)))
                fprintf (tex, "%.8lg%n", GET_VAL (node), &offset); 
            else
            {
                fprintf (tex, "(");
                fprintf (tex, "%.8lg%n", GET_VAL (node), &offset); 
                fprintf (tex, ")");
            }
        }

        if (IS_VAR (node))
            fprintf (tex, "%s%n", GET_VAR (node), &offset);
        

        *shift += offset;
        return;
    }
    
    switch (GET_OP (node))
    {
        case OP_ADD:
            {
                Print_latex_node (tex, LEFT, shift);

                fprintf (tex, "+");
               //// if (*shift >= Page_width) New_line (tex, "+", shift); 
                
                Print_latex_node (tex, RIGHT, shift);
                
                *shift++;
            }
            break;

        case OP_SUB:
            {
                Print_latex_node (tex, LEFT, shift);
                
                fprintf (tex, "-");
                //if (*shift >= Page_width) New_line (tex, "-", shift); 
                

                Print_latex_node (tex, RIGHT, shift);

                *shift++;
            }
            break;
        
        case OP_MUL:
            {
                int need_brackets = 0;
                if (IS_OP (LEFT) && (GET_OP (LEFT) == OP_SUB || GET_OP (LEFT) == OP_ADD))
                    need_brackets = 1;

                if (need_brackets)
                    Wrap_in_brackets (tex, LEFT, shift);
                else
                    Print_latex_node (tex, LEFT, shift);

                fprintf (tex, " \\cdot "); 
              //  if (*shift >= Page_width) {New_line (tex, " \\cdot ", shift); printf ("after: %d\n", *shift);} 

                
                need_brackets = 0;
                if (IS_OP (RIGHT) && (GET_OP (RIGHT) == OP_SUB || GET_OP (RIGHT) == OP_ADD))
                    need_brackets = 1;
                
                if (need_brackets)
                    Wrap_in_brackets (tex, RIGHT, shift);
                else
                    Print_latex_node (tex, RIGHT, shift);

                *shift++;
            }
            break;
        
        case OP_DIV:
            {
                fprintf (tex, " \\frac ");

                fprintf (tex, "{");
                int left_shift = 0;
                Print_latex_node (tex, LEFT, &left_shift);
                fprintf (tex, "}");

                fprintf (tex, "{");
                int right_shift = 0;
                Print_latex_node (tex, RIGHT, &right_shift);
                fprintf (tex, "}");

                *shift += MAX (left_shift, right_shift);
            }
            break;
        
        case OP_DEG:
            {
                int need_brackets = 0;
                if (IS_OP (LEFT))
                    need_brackets = 1;

                if (need_brackets)
                    Wrap_in_brackets (tex, LEFT, shift);
                else
                    Print_latex_node (tex, LEFT, shift);

                fprintf (tex, "^");

                fprintf (tex, "{");
                Print_latex_node (tex, RIGHT, shift);
                fprintf (tex, "}");

                *shift++;
            }
            break;

        case OP_SIN:
            fprintf (tex, " \\sin ");
            Wrap_in_brackets (tex, LEFT, shift);
            
            *shift += 3;
            break;

        case OP_COS:
            fprintf (tex, " \\cos ");
            Wrap_in_brackets (tex, LEFT, shift);

            *shift += 2;
            break;
        
        case OP_LOG:
            fprintf (tex, " \\ln ");
            Wrap_in_brackets (tex, LEFT, shift);

            *shift += 2;
            break;

        default:
            break;
    }

    return;
}

//======================================================================================

static void Wrap_in_brackets (FILE *tex, Node *node, int *shift)
{
    assert (tex   != nullptr && "tex is nullptr");
    assert (node  != nullptr && "node is nullptr");
    assert (shift != nullptr && "shift is nullptr");

    fprintf (tex, "(");
    Print_latex_node (tex, node, shift);
    fprintf (tex, ")");

    return;
}

static void New_line (FILE *tex, const char *op, int *shift)
{
    assert (tex   != nullptr && "tex is nullptr");
    assert (op    != nullptr && "node is nullptr");
    assert (shift != nullptr && "shift is nullptr");

    printf ("%d %s\n", *shift, op);

    fprintf (tex, "\n$");
    fprintf (tex,"\\\\");
    fprintf (tex, "$%s", op);

    *shift -= Page_width;

    return;
}

//======================================================================================

static int Call_latex (FILE *tex)
{
    assert (tex != nullptr && "tex is nullptr");

    char command[Max_definition_buffer] = "";

    sprintf (command, "pdflatex %s", Tex_file);

    return system (command);
}

//======================================================================================