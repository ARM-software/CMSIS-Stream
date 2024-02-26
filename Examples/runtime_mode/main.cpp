#include <cstdio>
#include <cstdint>

#include <iostream>
#include <map>
#include <cstdint>
#include "runtime_sched.h"
#include "AppNodes.h"
#include <fstream>

using namespace arm_cmsis_stream;

static registry_t register_nodes()
{
    registry_t res;

    Component<Source>::reg(res);
    Component<Sink>::reg(res);
    Component<ProcessingNode>::reg(res);
    Component<AdderNode>::reg(res);
    Component<RuntimeDuplicate>::reg(res);
   
    return(res);
};


static bool after(int *error,uint32_t *nbSchedule)
{
    std::cout << "After schedule hook\r\n";
    return(false);
}

void run_demo(const registry_t &registered_nodes)
{
    int error;
    std::ifstream input( "sched_flat.dat", std::ios::binary );
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    auto ctx = create_graph(buffer.data(),buffer.size(), registered_nodes);

    //auto ctx = create_graph(sched,SCHED_LEN, registered_nodes);

    SchedulerHooks hooks;
    hooks.before_schedule=nullptr;
    hooks.before_iteration=nullptr;

    // Only used in async mode
    hooks.async_before_node_check=nullptr;
    hooks.async_after_node_check=nullptr;
    hooks.async_node_not_executed=nullptr;

    hooks.before_node_execution=nullptr;
    hooks.after_node_execution=nullptr;

    hooks.after_iteration=nullptr;
    hooks.after_schedule=after;

    // Before running the graph, we try to access some nodes
    // to check the optional identification feature:
    // 

    using Proc = ProcessingNode;
    auto proca = static_cast<Proc*>(get_node(ctx,"processing"));

    

    if (proca!=nullptr)
    {
       std::cout << "processinga increment = " << proca->inc() << "\r\n";
    }
    else 
    {
       std::cout << "can't access processinga node\r\n";
    }


    uint32_t nbIterations = run_graph(hooks,ctx,&error,1);

    printf("Nb iterations = %d\r\n",nbIterations);

    switch(error)
    {
       case CG_BUFFER_UNDERFLOW:
        printf("Buffer underflow\r\n");
        break;
       case CG_BUFFER_OVERFLOW:
        printf("Buffer overflow\r\n");
        break;
       case CG_MEMORY_ALLOCATION_FAILURE:
        printf("Memory allocation failure\r\n");
        break;
       case CG_INIT_FAILURE:
        printf("Initialization failure\r\n");
        break;
       case CG_BUFFER_ERROR:
        printf("Buffer allocation error\r\n");
        break;
       case CG_OS_ERROR:
        printf("OS Error\r\n");
        break;
       case CG_STOP_SCHEDULER:
        printf("Normal end of the scheduler\r\n");
        break;
       case CG_SUCCESS:
        break;
       default:
        printf("Unknown error %d\r\n",error);
    }
}

int main(int argc, char const *argv[])
{
    
    printf("Start\n");
    auto registered_nodes = register_nodes();
     
    run_demo(registered_nodes);
  
    
    return 0;
}