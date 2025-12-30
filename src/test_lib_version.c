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
    int major, minor, patch;
    char version_string[16];
    ecjp_return_code_t ret;
    
    ret = 0;
    major = minor = patch = 0;
    memset(version_string, 0, sizeof(version_string));

    ret = ecjp_get_version(&major, &minor, &patch);
    if (ret != ECJP_NO_ERROR) {
        ecjp_fprintf("ecjp_get_version() failed with error code: %d\n", ret);
        ret = -1;
    }
    else {
        ecjp_fprintf("ecjp_get_version() succeeded. Version: %d.%d.%d\n", major, minor, patch);
        ret = ecjp_get_version_string(version_string, sizeof(version_string));
        if (ret != ECJP_NO_ERROR) {
            ecjp_fprintf("ecjp_get_version_string() failed with error code: %d\n", ret);
            ret = -1;
        }
        else {
            ecjp_fprintf("ecjp_get_version_string() succeeded. Version string: %s\n", version_string);
        }
    }

    return ret;
}

