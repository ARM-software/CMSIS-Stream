#pragma once 

extern "C"
{
#include "RTE_Components.h"
#include CMSIS_device_header

#include "cmsis_os2.h" /* CMSIS-RTOS2 API */
}

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

#include "stream_cmsisrtos_allocator.hpp"

#define CG_TIME_STAMP_TYPE uint32_t

#define CG_GET_TIME_STAMP() osKernelGetTickCount()  

#ifndef CONFIG_MAX_NUMBER_EVENT_ARGUMENTS
#define CONFIG_MAX_NUMBER_EVENT_ARGUMENTS 8
#endif

#define CG_MAX_VALUES CONFIG_MAX_NUMBER_EVENT_ARGUMENTS

#ifndef CMSISSTREAM_TENSOR_MAX_DIMENSIONS 
#define CMSISSTREAM_TENSOR_MAX_DIMENSIONS 3
#endif

#define CG_TENSOR_NB_DIMS CMSISSTREAM_TENSOR_MAX_DIMENSIONS

#ifndef CMSISSTREAM_LOG_DBG
#define CMSISSTREAM_LOG_DBG(fmt, ...)
#endif

#ifndef CMSISSTREAM_LOG_ERR
#define CMSISSTREAM_LOG_ERR(fmt, ...)
#endif

#define LOG_ERR(...) CMSISSTREAM_LOG_ERR(__VA_ARGS__);
#define LOG_DBG(...) CMSISSTREAM_LOG_DBG(__VA_ARGS__);
