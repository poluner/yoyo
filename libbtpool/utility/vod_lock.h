#ifndef _VOD_LOCK_H_
#define _VOD_LOCK_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include "utility/define.h"

#if defined(LINUX)
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#elif defined(AMLOS)
#include <sys/types.h>
#include <os/OS_API.h>
#elif defined(SUNPLUS)
#include "utility/define.h"
#include <gsl.h>
#endif

/* thread lock */
#if defined(LINUX)
typedef pthread_mutex_t VOD_LOCK;
#elif defined(AMLOS)
typedef OS_EVENT *VOD_LOCK;
#elif defined(MSTAR)
typedef _int32 VOD_LOCK;
#elif defined(WINCE)
typedef CRITICAL_SECTION_SD VOD_LOCK;
#elif defined(SUNPLUS)

#if defined(SUNPLUS_SEM)
typedef GL_Semaphore_t VOD_LOCK;
#else
typedef   GL_Mutex_t VOD_LOCK;
#endif

#else
typedef _int32 VOD_LOCK;
#endif

_int32 VodNewLock_init(VOD_LOCK *lock);
_int32 VodNewLock_uninit(VOD_LOCK *lock);

_int32 VodNewLock_lock(VOD_LOCK *lock);
_int32 VodNewLock_trylock(VOD_LOCK *lock);
_int32 VodNewLock_unlock(VOD_LOCK *lock);

#ifdef __cplusplus
}
#endif
#endif
