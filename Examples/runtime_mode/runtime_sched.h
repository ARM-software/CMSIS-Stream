/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        runtime_sched.h
 * Description:  Header the runtime mode : parsing and execution
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
*
 * Copyright (C) 2024 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
struct RuntimeContext;


/**
 * Function pointer type to create a node
 */
typedef NodeBase* (*mkNode_f)(const RuntimeContext &ctx, 
                              const Node *desc);


/**
 * @brief      Runtime context
 * 
 * The buffers, FIFOs and nodes created at runtime from the graph
 * description.
 */
struct RuntimeContext {
  //! Flat buffer description of the schedule
  const Schedule *schedobj;
  //! Memory buffers
  std::vector<std::unique_ptr<std::vector<int8_t>>> buffers;
  //! FIFOs or Buffers
  std::vector<std::unique_ptr<RuntimeEdge>> fifos;
  //! Nodes
  std::vector<std::unique_ptr<NodeBase>> nodes;
  //! Node identification (some nodes are named)
  std::map<const std::string,NodeBase*> identification;
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
using Registry = std::map<UUID_KEY,mkNode_f,std::less<UUID_KEY>>;


/**
 * @brief      Creates a graph.
 *
 * @param[in]  data  The flatbuffer data
 * @param[in]  nb    The number of bytes of data
 * @param[in]  map   The registry
 *
 * @return     An optional runtime context. Nothing in case of error.
 */
extern std::optional<RuntimeContext> create_graph(const unsigned char * data,
                                                  const uint32_t nb, 
                                                  const Registry &map);

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
                          const RuntimeContext& ctx,
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
extern NodeBase* get_node(const RuntimeContext& ctx,const std::string &name);

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
   static void reg(Registry& res)
   {
      res[UUID_KEY(T::uuid.data())] = &T::mkNode;
   };
};

}

#endif
