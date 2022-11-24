#ifndef _DRAW_TREE_H_
#define _DRAW_TREE_H_

#include "../operation_list.h"

#include "differentiator_tree.h"

static const int Max_command_buffer = 100;

static const int Mask_draw_node_modes = (1 << 4) - 1;

enum Draw_tree_func_err
{
    TREE_DRAW_GRAPH_ERR     = -1,
};

enum Tree_node_draw_modes
{
    
    DRAW_ID         = 0,

    DRAW_PTR        = 1,
    DRAW_DATA       = 2,

    DRAW_SONS_PTR   = 3,
};


int Draw_tree_graph (const Tree *tree, const char* name_output_file, 
                     const int node_draw_mode = Mask_draw_node_modes);

#endif  //#endif _DRAW_TREE_H_