#ifndef _READER_H_
#define _READER_H_

#include "../differentiator_tree/differentiator_tree.h"

enum Reader_func_err
{
    PARCE_ERR   = -1,
    SYNTAX_ERR  = -2,
};


int Parce_math_expression (Tree *tree_expresion, const char *expression, char *copy_expression);


#endif  //_READER_H_