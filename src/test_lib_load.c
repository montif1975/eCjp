#include "include/ecjp.h"

#include "sys/stat.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    ecjp_return_code_t ret = 0; 
    
    ret = ecjp_dummy();
    if (ret != ECJP_NO_ERROR) {
        fprintf(stdout, "ecjp_dummy() failed with error code: %d\n", ret);
        fprintf(stdout, "Unable to load library?\n");
        ret = -1;
    }
    else {
        fprintf(stdout, "ecjp_dummy() succeeded.\n");
    }
    return ret;
}

