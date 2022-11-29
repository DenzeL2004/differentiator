#ifndef _READER_H_
#define _READER_H_

#include "../differentiator_tree/differentiator_tree.h"


//===========================================
//! G = E, '\0'
//! E = T {[+ -] T}*
//! T = D {[* /] D}*
//! D = U {[^] U}*
//! U = cos(E) | sin(E) | ln(E) | P
//! P = (E) | N | V
//! N = ['0' - '9']+
//! V = ['a' - 'z' | 'A' - 'Z']+
//===========================================

enum Reader_func_err
{
    PARCE_ERR   = -1,
    SYNTAX_ERR  = -2,
};


int Parce_math_expression (Tree *tree_expresion, const char *expression, char *copy_expression);


#endif  //_READER_H_