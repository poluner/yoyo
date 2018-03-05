#include "utility/sd_assert.h"
#include "utility/sd_string.h"
#include "utility/log_wrapper.h"
LOGID_MODEULE("asyn_frame")

#if defined(_PC_LINUX)
#include <execinfo.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG

void print_strace(void)
{
    return;
}


void log_assert(const char * func, const char * file , int line, const char * ex)
{

}

#endif


#ifdef _DEBUG
int32_t sd_check_value(int32_t errcode,const char * func, const char * file , int line)
{
    if (SUCCESS == errcode)
    {
        return SUCCESS;
    }

    LOG_ERROR("Assert Fatal Error:%s in %s:%d:CHECK_VALUE(%d)", func, file, line, errcode);


#ifdef MACOS
    printf( "\n Assert Fatal Error:%s in %s:%d:CHECK_VALUE(%d)\n", func,file,line,errcode);
#endif

#if (defined(LINUX) && (!defined(_ANDROID_LINUX)))
    print_strace();
#else
//    sleep(20);
#endif

    assert(FALSE);
#ifdef WINCE
    exit(errcode);
#endif
    return errcode;
}
#endif /* _DEBUG  */

