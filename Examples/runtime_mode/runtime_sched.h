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
#include <optional>

#include "stream_generated.h"

/** @brief Infinite scheduling
 * 
 * For debug, scheduling can be limited to a finite 
 * number of iterations. When this macro is used,
 * the scheduling is running forever or until it is stopped
 * by a node.
 * 
 */
#define INFINITE_SCHEDULING (-1)


namespace arm_cmsis_stream {

class RuntimeEdge;
class NodeBase;
struct runtime_context;

/**
 * Function pointer type to run a node
 */
typedef int (*run_f)(NodeBase*);

/**
 * Function pointer type to check if a node can be run in asynchronous mode
 */
typedef int (*prepareForRunning_f)(NodeBase*);

/**
 * Function pointer type to create a node
 */
typedef NodeBase* (*mkNode_f)(const runtime_context &ctx, 
                              const arm_cmsis_stream::Node *desc);

struct _rnode_t;

typedef struct _rnode_t rnode_t;

/**
 * @brief      Runtime context
 * 
 * The buffers, FIFOs and nodes created at runtime from the graph
 * description.
 */
struct runtime_context {
  //! Flat buffer description of the schedule
  const arm_cmsis_stream::Schedule *schedobj;
  //! Memory buffers
  std::vector<std::unique_ptr<std::vector<int8_t>>> buffers;
  //! FIFOs or Buffers
  std::vector<std::unique_ptr<RuntimeEdge>> fifos;
  //! Nodes
  std::vector<std::unique_ptr<NodeBase>> nodes;
  //! API to use for interacting with a node
  std::vector<const rnode_t*> node_api;
  //! Node identification (some nodes are named)
  std::map<const std::string,NodeBase*> identification;
};

/**
 * @brief      API for a clss of nodes
 */
struct _rnode_t {
   //! How to run the node
   run_f run;
   //! How to check a node can run in asynchronous mode
   prepareForRunning_f prepareForRunning;
   //! Making an instance of this node
   mkNode_f mkNode;
};

/**
 * @brief      Customization hooks for the scheduling
 */
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

/**
 * @brief      UUID object used in std::map
 * 
 * The objects needs to implement the operator <
 */
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

//! Datatype for the registry of node categories (API for each kind of node)
using registry_t = std::map<UUID_KEY,rnode_t,std::less<UUID_KEY>>;


/**
 * @brief      Creates a graph.
 *
 * @param[in]  data  The flatbuffer data
 * @param[in]  nb    The number of bytes of data
 * @param[in]  map   The registry
 *
 * @return     An optional runtime context. Nothing in case of error.
 */
extern std::optional<runtime_context> create_graph(const unsigned char * data,
                                                   const uint32_t nb, 
                                                   const registry_t &map);

/**
 * @brief      Run the graph
 *
 * @param[in]  hooks         The customization hooks
 * @param[in]  ctx           The runtime context
 * @param      error         The returned error
 * @param[in]  nbIterations  The number of iterations
 *
 * @return     Number of iterations run until the end of the schedulng
 */
extern uint32_t run_graph(const SchedulerHooks &hooks,
                          const runtime_context& ctx,
                          int *error,
                          int nbIterations=INFINITE_SCHEDULING);

/**
 * @brief      Access a node by name
 *
 * @param[in]  ctx   The runtime context
 * @param[in]  name  The name of the node
 *
 * @return     The node or nullptr.
 */
extern NodeBase* get_node(const runtime_context& ctx,const std::string &name);

/**
 * @brief      Template to register a component
 *
 * @tparam     T     Component class
 */
template<typename T>
struct Component 
{
   /**
    * @brief      Register the component in the registry
    *
    * @param      res   The registry
    */
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
