#include "runtime_sched.h"
#include <map>
#include "cg_status.h"



using namespace arm_cmsis_stream;

runtime_context create_graph(const unsigned char * data,
                             const uint32_t nb, 
                             const registry_t &map)
{
   runtime_context c;
   bool ok = VerifyScheduleBuffer(flatbuffers::Verifier(data, nb));
   if (ok)
   {
      c.schedobj = GetSchedule(data);
      auto fifos = c.schedobj->fifos();

      for (unsigned int i = 0; i < fifos->size(); i++)
      {
          RuntimeEdge *f = nullptr;

          if (fifos->Get(i)->buffer())
          {
             f = new RuntimeBuffer(fifos->Get(i)->length());
          }
          else
          {
             f = new RuntimeFIFO(fifos->Get(i)->length(),
                                 fifos->Get(i)->delay());
          }
          c.fifos.push_back(std::unique_ptr<RuntimeEdge>(f));
      }
 
      auto nodes = c.schedobj->nodes();
      for (unsigned int i = 0; i < nodes->size(); i++)
      {
          auto n = nodes->Get(i);
          const UUID_KEY uuid = UUID_KEY(n->uuid()->v()->data());
          const rnode_t *api = &map.at(uuid);
          c.node_api.push_back(api);
          auto node = api->mkNode(c,n);
          c.nodes.push_back(std::unique_ptr<NodeBase>(node));
      }


   }
   else
   {
     std::cout << "Incorrect graph\r\n";
   }
   return(c);
}

uint32_t run_graph(const runtime_context& ctx,int *error,int nbIterations)
{
    const bool is_async = ctx.schedobj->async_mode();
    auto sched_desc = ctx.schedobj->schedule();
    uint32_t nb = 0;
    *error = CG_SUCCESS;

    while(true)
    {
        for (uint32_t i:*sched_desc) 
        {
            const rnode_t *api = ctx.node_api[i];
            NodeBase *node = ctx.nodes[i].get();
            int res = api->run(node);
            if (res != CG_SUCCESS)
            {
                *error=res;
                break;
            }
        }
        nb++;
        if (nbIterations>0)
        {
            nbIterations--;
            if (nbIterations==0)
            {
                *error=CG_STOP_SCHEDULER;
                return(nb);
            }
        }
    }

    
    return(nb);
}


