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
      auto buffers = c.schedobj->buffers();

      for (unsigned int i = 0; i < buffers->size(); i++)
      {
           const uint32_t nb_bytes = buffers->Get(i)->length();
           std::vector<int8_t> *v = new std::vector<int8_t>(nb_bytes);
           c.buffers.push_back(std::unique_ptr<std::vector<int8_t>>(v));
      }

      auto fifos = c.schedobj->fifos();
      for (unsigned int i = 0; i < fifos->size(); i++)
      {
          RuntimeEdge *f = nullptr;
          const uint16_t bufid = fifos->Get(i)->bufid();

          if (fifos->Get(i)->buffer())
          {
             f = new RuntimeBuffer(c.buffers[bufid].get()->data());
          }
          else
          {
             f = new RuntimeFIFO(c.buffers[bufid].get()->data(),
                                 fifos->Get(i)->length(),
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

      for (unsigned int i = 0; i < nodes->size(); i++)
      {
        auto n = nodes->Get(i);
        if (n->name() != nullptr)
        {
             c.identification[n->name()->str()] = c.nodes[i].get();
        }
      }

      


   }
   else
   {
     std::cout << "Incorrect graph\r\n";
   }
   return(c);
}

NodeBase* get_node(const runtime_context& ctx,
                   const std::string &name)
{
   if (ctx.identification.find(name) == ctx.identification.end())
   {
      return(nullptr);
   }
   else 
   {
      return(ctx.identification.at(name));
     
   }
}

#define HOOK(A,...)                       \
if (hooks.##A !=nullptr)                  \
    {                                     \
        hook_res = hooks.##A(__VA_ARGS__);\
        if (hook_res)                     \
        {                                 \
            goto end;                     \
        }                                 \
    }

uint32_t run_graph(const SchedulerHooks &hooks,
                   const runtime_context& ctx,
                   int *error,
                   int nbIterations)
{
    const bool is_async = ctx.schedobj->async_mode();
    auto sched_desc = ctx.schedobj->schedule();
    uint32_t nb = 0;
    *error = CG_SUCCESS;
    bool hook_res;
    

    HOOK(before_schedule,error,&nb);
   
    while(true)
    {
        HOOK(before_iteration,error,&nb);
        for (uint32_t i:*sched_desc) 
        {
            const rnode_t *api = ctx.node_api[i];
            NodeBase *node = ctx.nodes[i].get();

            if (is_async)
            {
                HOOK(async_before_node_check,error,&nb,i);
                *error = api->prepareForRunning(node);
                HOOK(async_after_node_check,error,&nb,i);
                if (*error == CG_SKIP_EXECUTION)
                {
                    *error = CG_SUCCESS;
                    HOOK(async_node_not_executed,error,&nb,i);
                    continue;
                }
                if (*error != CG_SUCCESS)
                {
                    goto end;
                }
            }

            HOOK(before_node_execution,error,&nb,i);
            *error = api->run(node);
            HOOK(after_node_execution,error,&nb,i);

            if (*error != CG_SUCCESS)
            {
                goto end;
            }
        }
        HOOK(after_iteration,error,&nb);
        nb++;
        if (nbIterations>0)
        {
            nbIterations--;
            if (nbIterations==0)
            {
                *error=CG_STOP_SCHEDULER;
                goto end;
            }
        }
    }
    
end:
    HOOK(after_schedule,error,&nb);
    return(nb);
}


