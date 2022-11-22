#ifndef _DIFFERENTIATOR_CONFIG_H_
#define _DIFFERENTIATOR_CONFIG_H_

static const int Number_operations = 8;

enum Differentiator_operation
{
    OP_UNKNOWN  = -1,

    OP_ADD      =  0,
    OP_SUB      =  1,

    OP_MUL      =  2,
    OP_DIV      =  3,

    OP_SIN      =  4,
    OP_COS      =  5,

    OP_LOG       =  6,

    OP_DEG      =  7,
};

static const char *Name_operations[] = {
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "SIN",
    "COS",
    "LN",
    "DEG"
};

#endif //_DIFFERENTIATOR_CONFIG_H_