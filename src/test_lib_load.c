#include "ecjp.h"

#include "sys/stat.h"
#include <unistd.h>

#ifdef ECJP_RUN_ON_PC
    #define ecjp_fprintf(format, ...)    fprintf(stdout, format, __VA_ARGS__)
    #define ecjp_fprint(format)          fprintf(stdout, format)
#else
    #define ecjp_fprintf(format, ...)
    #define ecjp_fprint(format)
#endif

int main(int argc, char *argv[])
{
    ecjp_return_code_t ret = 0; 
    
    ret = ecjp_dummy();
    if (ret != ECJP_NO_ERROR) {
        ecjp_fprintf("ecjp_dummy() failed with error code: %d\n", ret);
        ecjp_fprint("Unable to load library?\n");
        ret = -1;
    }
    else {
        ecjp_fprint("ecjp_dummy() succeeded.\n");
    }
    return ret;
}

