#ifndef RUNTIME_SCHED_H 
#define RUNTIME_SCHED_H

#define RUNTIME (-1)

#include <cstdint>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <iostream>

#include "GenericRuntimeNodes.h"

#define INFINITE_SCHEDULING (-1)


namespace arm_cmsis_stream {


typedef int (*run_f)(NodeBase*);
typedef int (*prepareForRunning_f)(NodeBase*);
typedef NodeBase* (*mkNode_f)(const runtime_context &ctx, 
                              const arm_cmsis_stream::Node *desc);

struct _rnode_t {
   run_f run;
   prepareForRunning_f prepareForRunning;
   mkNode_f mkNode;
};

struct SchedulerHooks{
  bool (*before_schedule)(int *error,uint32_t *nbSchedule);
  bool (*before_iteration)(int *error,uint32_t *nbSchedule);
  
  bool (*async_before_node_check)(int *error,uint32_t *nbSchedule,const int nodeID);
  bool (*async_after_node_check)(int *error,uint32_t *nbSchedule,const int nodeID);
  bool (*async_node_not_executed)(int *error,uint32_t *nbSchedule,const int nodeID);


  bool (*before_node_execution)(int *error,uint32_t *nbSchedule,const int nodeID);
  bool (*after_node_execution)(int *error,uint32_t *nbSchedule,const int nodeID);
  
  bool (*after_iteration)(int *error,uint32_t *nbSchedule);
  bool (*after_schedule)(int *error,uint32_t *nbSchedule);

};


struct UUID_KEY
{
    explicit UUID_KEY(const uint8_t *r):v(r){};
    ~UUID_KEY(){v=nullptr;};

    UUID_KEY(UUID_KEY&&other):v(other.v){};
    UUID_KEY& operator=(UUID_KEY&&other){
      v = other.v;
      return(*this);
    };

    UUID_KEY(const UUID_KEY&other):v(other.v){};
    UUID_KEY& operator=(const UUID_KEY&other){
      v = other.v;
      return(*this);
    };


    bool operator< ( const UUID_KEY &other ) const 
    {
      for(int i=0;i<16;i++)
      {
         if (v[i] < other.v[i])
         {
           return(true);
         }
         if (v[i] > other.v[i])
         {
           return(false);
         }
      }
      return(false);
    }


protected:
  const uint8_t* v;
};


using registry_t = std::map<UUID_KEY,rnode_t,std::less<UUID_KEY>>;

extern runtime_context create_graph(const unsigned char * data,
                                    const uint32_t nb, 
                                    const registry_t &map);

extern uint32_t run_graph(const SchedulerHooks &hooks,
                          const runtime_context& ctx,
                          int *error,
                          int nbIterations=INFINITE_SCHEDULING);

extern NodeBase* get_node(const runtime_context& ctx,const std::string &name);

template<typename T>
struct Component 
{
   static void reg(registry_t& res)
   {
      rnode_t t;
      t.mkNode            = &T::mkNode;
      t.prepareForRunning = &T::prepareForRunningNode;
      t.run               = &T::runNode;

      res[UUID_KEY(T::uuid.data())] = t;
   };
};

}

#endif
