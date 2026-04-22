#pragma once 

#include "cmsis_os2.h"

class CMSISMutex
{
  public:
    CMSISMutex()
    {
        mutex_id = osMutexNew(NULL);
    }

    ~CMSISMutex()
    {
        if (mutex_id != nullptr)
        {
            // Ensure the mutex is deleted only if it was created successfully
            osMutexDelete(mutex_id);
        }
    }

    osMutexId_t id() const
    {
        return mutex_id;
    }

  protected:
    osMutexId_t mutex_id;
};

class CMSISLock
{
  public:
    CMSISLock(CMSISMutex &mutex)
        : mutex(mutex)
    {
    }

    osStatus_t acquire()
    {
        error = osMutexAcquire(mutex.id(), osWaitForever);
        return error;
    }

    osStatus_t tryAcquire()
    {
        error = osMutexAcquire(mutex.id(), 0);
        return error;
    }

    ~CMSISLock()
    {
        if (error == osOK)
        {
            error = osMutexRelease(mutex.id());
        }
    }

    osStatus_t getError() const
    {
        return error;
    }

  protected:
    CMSISMutex &mutex;
    osStatus_t error;
};

#define CG_EVENTS_MULTI_THREAD

#define CG_MUTEX CMSISMutex
#define CG_MUTEX_ERROR_TYPE osStatus_t

#define CG_MUTEX_HAS_ERROR(ERROR) (ERROR != osOK)

#define CG_ENTER_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                           \
        CMSISLock lock((MUTEX));                \
        ERROR = lock.acquire();

#define CG_EXIT_CRITICAL_SECTION(MUTEX, ERROR) \
    }

#define CG_ENTER_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                                \
        CMSISLock lock((MUTEX));                     \
        ERROR = lock.acquire();

#define CG_EXIT_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    }

#define CG_MK_LIST_EVENT_ALLOCATOR(T) (CMSISEventPoolAllocator<T>{})
#define CG_MK_PROTECTED_BUF_ALLOCATOR(T) (CMSISBufPoolAllocator<T>{})
#define CG_MK_PROTECTED_MUTEX_ALLOCATOR(T) (CMSISMutexPoolAllocator<T>{})

#include "cmsis_allocator.hpp"

#define CG_TIME_STAMP_TYPE uint32_t

#define CG_GET_TIME_STAMP() osKernelGetTickCount()  

#define CG_MAX_VALUES 8
#define CG_TENSOR_NB_DIMS 3 

#define LOG_ERR(fmt, ...) //fprintf(stderr, "[ERR] " fmt, ##__VA_ARGS__)