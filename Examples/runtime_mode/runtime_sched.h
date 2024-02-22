#ifndef RUNTIME_SCHED_H 
#define RUNTIME_SCHED_H

#define RUNTIME (-1)

#include <cstdint>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include "stream_generated.h"
#include "GenericNodes.h"

using namespace arm_cmsis_stream;

class RuntimeFIFO
{
    public:
        explicit RuntimeFIFO(uint32_t nb_bytes,int delay=0):
        readPos(0),writePos(delay),length(nb_bytes) {
            mBuffer.resize(nb_bytes);
        };

        ~RuntimeFIFO(){};
        /* 
        FIFO are fixed and not made to be copied or moved.
        */
        
        RuntimeFIFO(RuntimeFIFO&&) = default;
        RuntimeFIFO& operator=(RuntimeFIFO&&) = default;

        RuntimeFIFO(const RuntimeFIFO&) = default;
        RuntimeFIFO& operator=(const RuntimeFIFO&) = default;

        /* 

        Check for overflow must have been done
        before using this function 
        
        */
        template<typename T>
        T * getWriteBuffer(int nb) 
        {
            
            if (readPos > 0)
            {
                memcpy((void*)mBuffer.data(),(void*)(mBuffer.data()+readPos),(writePos-readPos));
                writePos -= readPos;
                readPos = 0;
            }
            
            int8_t *ret = mBuffer.data() + writePos;
            writePos += nb*sizeof(T); 
            return((T*)ret);
        };

        /* 
        
        Check for undeflow must have been done
        before using this function 
        
        */
        template<typename T>
        T* getReadBuffer(int nb)  
        {
           
            int8_t *ret = mBuffer.data() + readPos;
            readPos += nb*sizeof(T);
            return((T*)ret);
        }

        template<typename T>
        bool willUnderflowWith(int nb) const   
        {
            return((writePos - readPos - nb*sizeof(T))<0);
        }

        template<typename T>
        bool willOverflowWith(int nb) const   
        {
            return((writePos - readPos + nb*sizeof(T))>length);
        }

        int nbBytesInFIFO() const    {return (writePos - readPos);};
        
        int nbOfFreeBytesInFIFO() const    {return (length - writePos + readPos);};



    protected:
        std::vector<int8_t> mBuffer;
        int readPos,writePos;
        uint32_t length;
};

struct _rnode_t;

typedef struct _rnode_t rnode_t;

struct runtime_context {
  const arm_cmsis_stream::Schedule *schedobj;
  std::vector<std::unique_ptr<RuntimeFIFO>> fifos;
  std::vector<std::unique_ptr<NodeBase>> nodes;
  std::vector<const rnode_t*> node_api;
};

typedef int (*run_f)(NodeBase*);
typedef int (*prepareForRunning_f)(NodeBase*);
typedef NodeBase* (*mkNode_f)(const runtime_context &ctx, 
                          const arm_cmsis_stream::Node *desc);

struct _rnode_t {
   run_f run;
   prepareForRunning_f prepareForRunning;
   mkNode_f mkNode;
};

#include <iostream>

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

extern uint32_t run_graph(const runtime_context& ctx,int *error,int nbIterations=-1);

#endif
