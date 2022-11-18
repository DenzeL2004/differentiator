

#include <stdio.h>
#include <string>


#include "differentiation/differentiator.h"
#include "src/log_info/log_errors.h"
#include "src/Generals_func/generals.h"


int main (int argc, char *argv[])
{   
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif 
    
    
    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif
}