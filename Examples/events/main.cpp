#include <cstdio>
#include <cstdint>
#include "cstream_node.h"
#include "scheduler.h"
#include "cg_enums.h"

#include "config_events.h"
#include "cg_enums.h"
#include "custom.hpp"
#include "posix_thread.hpp"
#include "StreamNode.hpp"
#include "EventQueue.hpp"
#include "cg_queue.hpp"

#include "host_com.hpp"
#include "cg_pack.hpp"

#include <iostream>

 std::pmr::synchronized_pool_resource pool;

// When event system is enabled, we provide a global event queue
#if defined(CG_EVENTS)

// When event multi-threading is enabled, we define a thread to process the event queue
#if defined(CG_EVENTS_MULTI_THREAD)
PlatformThread *cg_eventThread = nullptr;
bool cg_eventThreadDone = false;

#if defined(CG_HOST)
#include "host_com.hpp"
bool cg_hostThreadDone = false;

#endif // CG_HOST
#endif // CG_MULTI_THREAD

#endif // CG_EVENTS

using namespace arm_cmsis_stream;

std::ostream &operator<<(std::ostream &os, const std::vector<uint8_t> &obj)
{

    for (uint32_t i = 0; i < obj.size(); ++i)
    {
        printf("\\x%02x", obj[i]);
    }
    return os;
}

struct MyObj
{
    int v;
    void test1(float a, int b)
    {
        std::cout << "Test1 function called with v: " << v << " and a: " << a << " and b: " << b << std::endl;
    }

    void test2(float a, int b)
    {
        std::cout << "Test2 function called with v: " << v << " and a: " << a << " and b: " << b << std::endl;
    }

    void test3(uint8_t c)
    {
        std::cout << "Test3 function called with v: " << v << " and c: " << int(c) << std::endl;
    }
};

void debug()
{
    // Host::send(0,kValue,std::string("ABCD"));
    printf("Event size %zd\n", sizeof(Event));
    printf("List Value size %zd\n", sizeof(ListValue));
    printf("cg_value size %zd\n", sizeof(cg_value));
    printf("shared_ptr size %zd\n", sizeof(std::shared_ptr<void>));
    printf("std::string size %zd\n", sizeof(std::string));
    printf("Buffer size %zd\n", sizeof(BufferPtr));
    printf("Tensor size %zd\n", sizeof(TensorPtr<double>));

#if 0
    Pack pack;


    
    Tensor t;
    t.nb_dims = 2;
    t.dims[0] = 2;
    t.dims[1] = 3;
    std::shared_ptr<float[]> p(new float[6]);
    for(int i=0;i<6;i++)
    {
        p[i]=(float)i+0.1;
    }
    t.data = std::move(p);


    pack.pack(0, Event(1, kNormalPriority,std::move(t)));


    //pack.pack(0, Event(1, kNormalPriority,"Hello"));
    
    
    std::cout << "Serialized data size: " << pack.vector() << std::endl;

    
    Unpack unpack(pack.vector().data(), pack.vector().size());
    uint32_t nodeid;
    Event evt = unpack.unpack(nodeid);
    std::cout << "Unpacked event ID: " << evt.event_id << std::endl;
    std::cout << "Unpacked event " << evt << std::endl;
#endif
    // Delete the old event that may contain shared_ptr to the
    // cbv pool.
    // If cbv pool is deleted before "pointer" to it are
    // released then it will crash because the pointer will
    // be released after the pool has been deleted.
    Event evt(0, kNormalPriority, 1.0f,int32_t(2));
    MyObj obj{4};

    if (evt.wellFormed<float, int32_t>())
    {
        std::cout << "Well formed" << std::endl;
        evt.apply<float, int32_t>(&MyObj::test1, obj);
        evt.apply<float, int32_t>(&MyObj::test2, obj);
        std::cout << "Test function applied" << std::endl;
    }
    else
    {
        std::cout << "Not well formed" << std::endl;
    }

    obj.v = 5;

    evt = Event(0, kNormalPriority, uint8_t(42));
    if (evt.wellFormed<uint8_t>())
    {
        std::cout << "Well formed" << std::endl;
        evt.apply<uint8_t>(&MyObj::test3, obj);
        std::cout << "Test function applied" << std::endl;
    }
    else
    {
        std::cout << "Not well formed" << std::endl;
    }
}

bool app_handler(void *data,Event &&evt)
{
      printf("App received event with id %d\n", evt.event_id);
      return true;
}

int main(int argc, char const *argv[])
{
    // debug();
    // exit(0);
#if 1
    int err = init_scheduler();
    if (err == CG_MEMORY_ALLOCATION_FAILURE)
    {
        std::cerr << "Error: Memory allocation failure during scheduler initialization." << std::endl;
        return -1;
    }
#endif
#if defined(CG_EVENTS)

    arm_cmsis_stream::EventQueue::cg_eventQueue = new MyQueue();
    arm_cmsis_stream::EventQueue::cg_eventQueue->setHandler(nullptr, app_handler);

#if defined(CG_EVENTS_MULTI_THREAD)

// Thread for host communication
#if defined(CG_HOST)
    PosixThread hostComThread(listen_to_host);
    hostComThread.start();
    hostComThread.setPriority(ThreadPriority::Low);
    hostComThread.waitUntilStarted();
#endif

    // Thread is processing the event queue an sleeping when no events are available
    PosixThread t([]
                  {
        std::cout << "Thread started!" << std::endl;
        // If we are done with the scheduling, we exit the thread
        while(!cg_eventThreadDone)
        {
            arm_cmsis_stream::EventQueue::cg_eventQueue->execute();
            if (cg_eventThread)
               cg_eventThread->sleep();
        } });

    // Thread is started and we wait until the thread is ready
    // before we start the scheduler.
    cg_eventThread = &t;
    t.start();
    t.setPriority(ThreadPriority::Low);
    t.waitUntilStarted(); // Wait until the thread is ready to process events
#endif

#endif

    // Initialize and start the scheduler
    int error;
    printf("Start\n");
    uint32_t nbSched = scheduler(&error);
    printf("Nb sched = %d\n", nbSched);
    if (error != CG_SUCCESS)
    {
        std::cerr << "Error during scheduling: " << error << std::endl;
        return -1;
    }
#if defined(CG_EVENTS)
#if defined(CG_EVENTS_MULTI_THREAD)

    // Notify the thread we are done and wait for the end
    // of the thread.
    cg_eventThreadDone = true;
    t.wakeup(); // Wake up the thread to finish processing
    t.join();

#if defined(CG_HOST)
    close_host();
    hostComThread.join(); // Wait for the host communication thread to finish
#endif
#endif

    // Delete the event queue
    delete arm_cmsis_stream::EventQueue::cg_eventQueue;
    arm_cmsis_stream::EventQueue::cg_eventQueue = nullptr;
#endif
    free_scheduler();
    return 0;
}