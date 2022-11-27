#ifndef _LATEX_PRINT_H_
#undef  _LATEX_PRINT_H_

#include "stdint.h"

static const char* Tex_file = "dif.tex";

static const int Page_width = 30;

static const char *Linking_words[] = 
{
    "Следовательно",
    "Внимательный чиатель мог заметить, что",
    "Очевидно, что",
    "По теорее 1.2.1334323123 Кантора-Вейоштрасса-Буньковского-Борелли, следует, что",
    "Несложно заметить, что",
    "Это шаг вообще можно было и пропустить, так как очевиднее него ничего на свете нет",
    "Наеюсь, данный переход вас не сильно шокировал",
    "По анологии с задачей 2.34.23434",
    "Кок, кок, кок и все",
    "Шлеп, шлеп, шлеп, получим",
};

static const int Cnt_linking_words = sizeof (Linking_words) / sizeof (char*);


enum Tex_func_err
{
    CREAT_TEX_FILE_ERR =    -1,
};

FILE* Latex_start  ();

int Latex_finish (FILE *tex);


int Print_latex_message (FILE *tex, const char *format, ...);

int Print_latex_tree (FILE *tex, Tree *expression);

#endif //#endif _LATEX_PRINT_H_