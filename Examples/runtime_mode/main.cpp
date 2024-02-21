#include <cstdio>
#include <cstdint>
#include "scheduler.h"

#include "stream_generated.h"
#include "sched_flat.h"
#include "runtime_sched.h"
#include <iostream>
#include <map>
#include <cstdint>
#include "custom.h"
#include "GenericNodes.h"
#include "cg_status.h"
#include "AppNodes.h"
#include <fstream>

using namespace arm_cmsis_stream;


const std::array<uint8_t,16> uuid_src = {0xc0,0x08,0x9f,0x59,0x2f,0x33,0x4e,0xc4,0x90,0x23,0x30,0xf6,0x9f,0x0f,0x48,0x33};
const std::array<uint8_t,16> uuid_proc = {0x3f,0xf6,0x2b,0x0c,0x9a,0xd8,0x44,0x5d,0xbb,0xe9,0x20,0x8d,0x87,0x42,0x34,0x46};
const std::array<uint8_t,16> uuid_dst = {0xc3,0x0e,0xa9,0xea,0xe9,0xc3,0x46,0x38,0xbb,0xc6,0x02,0x1f,0xa3,0x54,0x9d,0x93};

static registry_t register_nodes()
{
    registry_t res;

    rnode_t t;

    auto key_src  = UUID_KEY(uuid_src.data());
    auto key_proc = UUID_KEY(uuid_proc.data());
    auto key_dst  = UUID_KEY(uuid_dst.data());

    /*  Register src */
    t.mkNode = &Source<float,RUNTIME>::mkNode;
    t.deleteNode = &Source<float,RUNTIME>::deleteNode;
    t.prepareForRunning = &Source<float,RUNTIME>::prepareForRunningNode;
    t.run = &Source<float,RUNTIME>::runNode;

    res[key_src] = t;

    /*  Register proc */
    using FloatProc = ProcessingNode<float,RUNTIME,float,RUNTIME>;
    t.mkNode = &FloatProc::mkNode;
    t.deleteNode = &FloatProc::deleteNode;
    t.prepareForRunning = &FloatProc::prepareForRunningNode;
    t.run = &FloatProc::runNode;

    res[key_proc] = t;

    /*  Register dst */
    t.mkNode = &Sink<float,RUNTIME>::mkNode;
    t.deleteNode = &Sink<float,RUNTIME>::deleteNode;
    t.prepareForRunning = &Sink<float,RUNTIME>::prepareForRunningNode;
    t.run = &Sink<float,RUNTIME>::runNode;

    res[key_dst] = t;

    return(res);
};

void read_sched()
{
    bool ok = VerifyScheduleBuffer(flatbuffers::Verifier(sched, SCHED_LEN));
    if (ok)
    {
        auto schedobj = GetSchedule(sched);
        std::cout << "Schedule\r\n";
        std::cout << "Async mode=" << schedobj->async_mode() << "\r\n";
        auto sched_desc = schedobj->schedule();
        for (auto s:*sched_desc) 
        {
            std::cout << s << "\r\n";
        }

        std::cout << "FIFOs\r\n";
        auto fifos = schedobj->fifos();
        for (unsigned int i = 0; i < fifos->size(); i++) 
        {
           auto f = fifos->Get(i);
           std::cout << "id=" << f->id() << " , length=" << f->length() << "\r\n";
        }

        std::cout << "NODEs\r\n";
        auto nodes = schedobj->nodes();
        for (unsigned int i = 0; i < nodes->size(); i++) 
        {
           auto n = nodes->Get(i);
           std::cout << "id=" << n->id() << "\r\n";
           auto uuid = n->uuid();
           auto uuid_v = uuid->v();
           for (auto s:*uuid_v) 
           {
             printf("%02x",s);
           }
           printf("\n");

           auto inputs = n->inputs();
           if (inputs)
           {
               std::cout << "inputs\r\n";
               for (auto s:*inputs) 
               {
                  std::cout << "id=" << s->id() << ", nb=" << s->nb() << " ";
               }
               std::cout << "\r\n";
           }

           auto outputs = n->outputs();
           if (outputs)
           {
               std::cout << "outputs\r\n";
               for (auto s:*outputs) 
               {
                  std::cout << "id=" << s->id() << ", nb=" << s->nb() << " ";
               }
               std::cout << "\r\n";
           }
           std::cout << "\r\n";
        }
    }
    else 
    {
        std::cout << "Invalid buffer\r\n";
    }
}

int main(int argc, char const *argv[])
{
    int error;
    printf("Start\n");
    auto registered_nodes = register_nodes();
     
    std::ifstream input( "sched_flat.dat", std::ios::binary );
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    auto ctx = create_graph(buffer.data(),buffer.size(), registered_nodes);

    //auto ctx = create_graph(sched,SCHED_LEN, registered_nodes);

    uint32_t nbIterations = run_graph(ctx,&error,1);

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
  
    
    return 0;
}