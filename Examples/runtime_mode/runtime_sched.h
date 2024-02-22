#ifndef RUNTIME_SCHED_H 
#define RUNTIME_SCHED_H

#define RUNTIME (-1)

#include <cstdint>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <string>

#include "stream_generated.h"
#include "GenericNodes.h"

#define INFINITE_SCHEDULING (-1)

using namespace arm_cmsis_stream;

using IOVector = flatbuffers::Vector<const arm_cmsis_stream::IODesc*>;

class RuntimeEdge
{
public:
   RuntimeEdge(){};
   virtual ~RuntimeEdge(){};

   virtual char* getWriteBuffer(const IOVector &io,
                                const std::size_t sample_size,
                                const int io_id,
                                const int nb_samples=0)=0;

   virtual char* getReadBuffer(const IOVector &io,
                               const std::size_t sample_size,
                               const int io_id,
                               const int nb_samples=0)=0;

  virtual bool willUnderflowWith(const IOVector &io,
                                 const std::size_t sample_size,
                                 const int io_id,
                                 const int nb_samples=0) const=0;

  virtual bool willOverflowWith(const IOVector &io,
                                const std::size_t sample_size,
                                const int io_id,
                                const int nb_samples=0) const=0;
        
  virtual int nbBytesInFIFO() const =0;

  virtual int nbOfFreeBytesInFIFO() const=0;
};


class RuntimeFIFO:public RuntimeEdge
{
    public:
        explicit RuntimeFIFO(uint32_t nb,int delay=0):
        readPos(0),writePos(delay),length(nb) {
            mBuffer.resize(nb);
        };

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
        char* getWriteBuffer(const IOVector &io,
                             const std::size_t sample_size,
                             const int io_id,
                             const int nb_samples=0)  final
        {
            int nb;
            if (nb_samples==0) 
            {
                nb = io.Get(io_id)->nb()*sample_size;
            }
            else 
            {
                nb = nb_samples*sample_size;
            }

            if (readPos > 0)
            {
                memcpy((void*)mBuffer.data(),(void*)(mBuffer.data()+readPos),(writePos-readPos));
                writePos -= readPos;
                readPos = 0;
            }
            
            int8_t *ret = mBuffer.data() + writePos;
            writePos += nb; 
            return((char*)ret);
        };

        /* 
        
        Check for undeflow must have been done
        before using this function 
        
        */
        
        char* getReadBuffer(const IOVector &io,
                            const std::size_t sample_size,
                            const int io_id,
                            const int nb_samples=0)    final
        {
            int nb;
            if (nb_samples==0) 
            {
                nb = io.Get(io_id)->nb()*sample_size;
            }
            else 
            {
                nb = nb_samples*sample_size;
            }

            int8_t *ret = mBuffer.data() + readPos;
            readPos += nb;
            return((char*)ret);
        }

        bool willUnderflowWith(const IOVector &io,
                               const std::size_t sample_size,
                               const int io_id,
                               const int nb_samples=0) const   final
        {
            int nb;
            if (nb_samples==0) 
            {
                nb = io.Get(io_id)->nb()*sample_size;
            }
            else 
            {
                nb = nb_samples*sample_size;
            }
            return((writePos - readPos - nb)<0);
        }

        bool willOverflowWith(const IOVector &io,
                              const std::size_t sample_size,
                              const int io_id,
                              const int nb_samples=0) const   final
        {
            int nb;
            if (nb_samples==0) 
            {
                nb = io.Get(io_id)->nb()*sample_size;
            }
            else 
            {
                nb = nb_samples*sample_size;
            }
            return((writePos - readPos + nb)>length);
        }

        int nbBytesInFIFO() const final   {return (writePos - readPos);};
        
        int nbOfFreeBytesInFIFO() const  final  {return (length - writePos + readPos);};



    protected:
        std::vector<int8_t> mBuffer;
        int readPos,writePos;
        uint32_t length;
};

class RuntimeBuffer:public RuntimeEdge
{
    public:
        /* No delay argument for this version of the FIFO.
           This version will not be generated when there is a delay
        */
        explicit RuntimeBuffer(uint32_t nb) {
            mBuffer.resize(nb);
        };

        /* 
        FIFO are fixed and not made to be copied or moved.
        */
        RuntimeBuffer(const RuntimeBuffer&) = default;
        RuntimeBuffer(RuntimeBuffer&&) = default;
        RuntimeBuffer& operator=(const RuntimeBuffer&) = default;
        RuntimeBuffer& operator=(RuntimeBuffer&&) = default;

        /* 
           Not used in synchronous mode 
           and this version of the FIFO is
           never used in asynchronous mode 
           so empty functions are provided.
        */
        bool willUnderflowWith(const IOVector &io,
                               const std::size_t sample_size,
                               const int io_id,
                               const int nb_samples=0) const final 
        {
            (void)nb_samples;
            (void)io;
            (void)sample_size;
            (void)io_id;
            return false;
        };
        
        bool willOverflowWith(const IOVector &io,
                              const std::size_t sample_size,
                              const int io_id,
                              const int nb_samples=0) const final   
        {
            (void)nb_samples;
            (void)io;
            (void)sample_size;
            (void)io_id;
            return false;
        };
        
        int nbBytesInFIFO() const final   
        {
            return(0);
        };
        
        int nbOfFreeBytesInFIFO() const final  
        {
            return( 0);
        };


        char* getWriteBuffer(const IOVector &io,
                             const std::size_t sample_size,
                             const int io_id,
                             const int nb_samples=0)  final  
        {
            (void)nb_samples;
            (void)io;
            (void)sample_size;
            (void)io_id;
            return(reinterpret_cast<char*>(mBuffer.data()));
        };

        char* getReadBuffer(const IOVector &io,
                            const std::size_t sample_size,
                            const int io_id,
                            const int nb_samples=0)    final
        {
            (void)nb_samples;
            (void)io;
            (void)sample_size;
            (void)io_id;
            return(reinterpret_cast<char*>(mBuffer.data()));
        }

    protected:
        std::vector<int8_t> mBuffer;
};

struct _rnode_t;

typedef struct _rnode_t rnode_t;

struct runtime_context {
  const arm_cmsis_stream::Schedule *schedobj;
  std::vector<std::unique_ptr<RuntimeEdge>> fifos;
  std::vector<std::unique_ptr<NodeBase>> nodes;
  std::vector<const rnode_t*> node_api;
  std::map<const std::string,NodeBase*> identification;
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

struct SchedulerHooks{
  bool (*before_schedule)(int *error,uint32_t *nbSchedule);
  bool (*before_iteration)(int *error,uint32_t *nbSchedule);
  bool (*before_node_execution)(int *error,uint32_t *nbSchedule,const int nodeID);
  bool (*after_node_execution)(int *error,uint32_t *nbSchedule,const int nodeID);
  bool (*after_iteration)(int *error,uint32_t *nbSchedule);
  bool (*after_schedule)(int *error,uint32_t *nbSchedule);

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

extern uint32_t run_graph(const SchedulerHooks &hooks,
                          const runtime_context& ctx,
                          int *error,
                          int nbIterations=INFINITE_SCHEDULING);

extern NodeBase* get_node(const runtime_context& ctx,const std::string &name);

#endif
