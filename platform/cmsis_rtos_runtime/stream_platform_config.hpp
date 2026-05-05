#pragma once 

/*
 * CMSIS-RTOS platform configuration for CMSIS-Stream.
 *
 * The CMSIS Pack supplies this header with the runtime. Client projects
 * customize runtime values in the copied stream_runtime_config.hpp file. That
 * file is included first so any CMSISSTREAM_* definition provided by the
 * application overrides the defaults below.
 */

extern "C"
{
#include "RTE_Components.h"
#include CMSIS_device_header

#include "cmsis_os2.h" /* CMSIS-RTOS2 API */
}

#include "stream_runtime_config.hpp"

#ifndef CMSISSTREAM_EVENT_QUEUE_LENGTH
#define CMSISSTREAM_EVENT_QUEUE_LENGTH 20
#endif

#ifndef CMSISSTREAM_EVT_THREAD_STACK_SIZE
#define CMSISSTREAM_EVT_THREAD_STACK_SIZE 4096
#endif 

#ifndef CMSISSTREAM_STREAM_THREAD_STACK_SIZE
#define CMSISSTREAM_STREAM_THREAD_STACK_SIZE 4096
#endif

#ifndef CMSISSTREAM_NB_MAX_EVENTS
#define CMSISSTREAM_NB_MAX_EVENTS 16
#endif

#ifndef CMSISSTREAM_NB_MAX_BUFS
#define CMSISSTREAM_NB_MAX_BUFS 16
#endif

#ifndef CMSISSTREAM_EVT_HIGH_PRIORITY
#define CMSISSTREAM_EVT_HIGH_PRIORITY osPriorityHigh
#endif

#ifndef CMSISSTREAM_STREAM_THREAD_PRIORITY
#define CMSISSTREAM_STREAM_THREAD_PRIORITY osPriorityRealtime
#endif

#ifndef CMSISSTREAM_EVT_NORMAL_PRIORITY
#define CMSISSTREAM_EVT_NORMAL_PRIORITY osPriorityNormal
#endif

#ifndef CMSISSTREAM_EVT_LOW_PRIORITY
#define CMSISSTREAM_EVT_LOW_PRIORITY osPriorityLow
#endif

#ifndef CMSISSTREAM_SHARED_OVERHEAD
#define CMSISSTREAM_SHARED_OVERHEAD 16
#endif

#ifndef CMSISSTREAM_POOL_SECTION
#define CMSISSTREAM_POOL_SECTION ".bss.evt_pool"
#endif

#ifndef CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS
#define CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS 8
#endif

#ifndef CMSISSTREAM_TENSOR_MAX_DIMENSIONS 
#define CMSISSTREAM_TENSOR_MAX_DIMENSIONS 3
#endif

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



#define CG_MAX_VALUES CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS


#define CG_TENSOR_NB_DIMS CMSISSTREAM_TENSOR_MAX_DIMENSIONS

#ifndef CMSISSTREAM_LOG_DBG
#define CMSISSTREAM_LOG_DBG(fmt, ...)
#endif

#ifndef CMSISSTREAM_LOG_ERR
#define CMSISSTREAM_LOG_ERR(fmt, ...)
#endif

#define LOG_ERR(...) CMSISSTREAM_LOG_ERR(__VA_ARGS__);
#define LOG_DBG(...) CMSISSTREAM_LOG_DBG(__VA_ARGS__);

class ContextSwitch
{
  public:
    virtual ~ContextSwitch()
    {
    }
    /*

    Event queue running but posting event disabled.
    Run from data flow thread except for pure event graphs.
    In that case, it is run from event thread.

    */
    virtual int pause() = 0;

    /*

    Run from data  flow thread.
    Posting events is possible but event thread is not yet
    restarted.

    */
    virtual int resume() = 0;
};
