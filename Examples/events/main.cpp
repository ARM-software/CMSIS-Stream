#include <cstdio>
#include <cstdint>
#include "cstream_node.h"
#include "scheduler.h"
#include "cg_enums.h"

#include "cg_enums.h"
#include "app_config.hpp"
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

    Event evt;

#if 0
    Pack pack;


    
    UniquePtr<float> p(6);
    
    for(int i=0;i<6;i++)
    {
        p[i]=(float)i+0.1;
    }
    TensorPtr<float> t = TensorPtr<float>::create_with((uint8_t)2,
                                                       cg_tensor_dims_t{2,3},
                                                       std::move(p));
            


    pack.pack(0, Event(1, kNormalPriority,std::move(t)));


    //pack.pack(0, Event(1, kNormalPriority,"Hello"));
    
    
    std::cout << "Serialized data size: " << pack.vector() << std::endl;

    
    Unpack unpack(pack.vector().data(), pack.vector().size());
    uint32_t nodeid;
    evt = unpack.unpack(nodeid);
    std::cout << "Unpacked event ID: " << evt.event_id << std::endl;
    std::cout << "Unpacked event " << std::move(evt) << std::endl;
#endif
    // Delete the old event that may contain shared_ptr to the
    // cbv pool.
    // If cbv pool is deleted before "pointer" to it are
    // released then it will crash because the pointer will
    // be released after the pool has been deleted.
    evt = Event(0, kNormalPriority, 1.0f, int32_t(2));
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

bool app_handler(int src_node_id, void *data, Event &&evt)
{
    printf("App received event with id %d from node %d\n", evt.event_id, src_node_id);
    return true;
}

int main(int argc, char const *argv[])
{
    // debug();
    // exit(0);


    MyQueue *queue = new MyQueue();
    arm_cmsis_stream::EventQueue::setHandler(nullptr, app_handler);

    PosixThread t([queue]
                  {
                      std::cout << "Event thread started!" << std::endl;
                      // If we are done with the scheduling, we exit the thread
                      queue->execute();
                      std::cout << "Event thread quitted!" << std::endl;
                  });

    queue->setThread(&t);
    t.setPriority(ThreadPriority::High);
    t.start();
    t.waitUntilStarted(); // Wait until the thread is ready to process events



    int error = init_scheduler(queue);
    if (error != CG_SUCCESS)
    {
        std::cout << "Error during scheduler initialization : " << error << std::endl;
        queue->end();
        t.join();
        return error;
    }

// Thread for host communication
#if defined(CG_HOST)
    PosixThread hostComThread([queue]
                              { listen_to_host(queue); });
    hostComThread.start();
    hostComThread.setPriority(ThreadPriority::Low);
    hostComThread.waitUntilStarted();
#endif

    
   
    // Initialize and start the scheduler

    PosixThread ts([&error]
                  {
                      std::cout << "Scheduler thread started!" << std::endl;
                      uint32_t nb = scheduler(&error);
                      if ((error != CG_SUCCESS) && (error != CG_STOP_SCHEDULER))
                      {
                          std::cout << "Error during scheduler execution : " << error << std::endl;
                          std::cout << "Scheduler executed " << nb << " iterations" << std::endl;
                      }
                      else
                      {
                          std::cout << "Scheduler executed " << nb << " iterations" << std::endl;
                      }
                      std::cout << "Scheduler thread quitted!" << std::endl;
                  });

    ts.setPriority(ThreadPriority::RealTime);
    ts.start();
    ts.waitUntilStarted(); // Wait until the thread is ready to process events


    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue->end();

    // Wait for event queue to finish
    t.join();

   

#if defined(CG_HOST)
    close_host();
    hostComThread.join(); // Wait for the host communication thread to finish
#endif

    // Delete the event queue
    free_scheduler();

    delete queue;
    return 0;
}