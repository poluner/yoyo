// mutex.h: interface for the mutex class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMMON_UTILITY_MUTEX__
#define _COMMON_UTILITY_MUTEX__


#include <pthread.h>

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&_lock, NULL);
    }
    virtual ~Mutex()
    {
        pthread_mutex_destroy(&_lock);
    }
    void Lock()
    {
        pthread_mutex_lock(&_lock);
    }
    void Unlock()
    {
        pthread_mutex_unlock(&_lock);
    }
private:
    pthread_mutex_t _lock;

};

class ScopedLock
{
public:
    ScopedLock( Mutex& mutex_obj ) : _mutex_obj(mutex_obj)
    {
        _mutex_obj.Lock();
    }
    ~ScopedLock()
    {
        _mutex_obj.Unlock();
    }
private:
    Mutex& _mutex_obj;

};

#define TAKE_MUTEX(MyMutex)					ScopedLock MyMutex##_Lock(MyMutex)
#define ENTER_CRITICAL_SECTION(MyMutex)		do { TAKE_MUTEX(MyMutex)
#define LEAVE_CRITICAL_SECTION				} while(FALSE);

#endif // 
