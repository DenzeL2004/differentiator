#ifndef _SIMPLIFIER_H_
#define _SIMPLIFIER_H_

#include "../differentiator_tree/differentiator_tree.h"
#include "../operation_list.h"

//#define CALC_CONST      //<- calculation of operations where most likely the answer will not be an integer

#define FAST_SIMPLIFIER

int Simplifier (Node* node);

#endif //_SIMPLIFIER_H_
