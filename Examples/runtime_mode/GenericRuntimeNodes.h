#pragma once

#include <cstdint>
#include "stream_generated.h"
#include "cg_status.h"

#define CG_SUCCESS_ID_CODE (0)
#define CG_SKIP_EXECUTION_ID_CODE (-5)
#define CG_BUFFER_ERROR_ID_CODE (-6)

namespace arm_cmsis_stream {

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
        explicit RuntimeFIFO(int8_t *buf,const uint32_t nb,int delay=0):
        mBuffer(buf),readPos(0),writePos(delay),length(nb) {
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
                memcpy(mBuffer,mBuffer+readPos,writePos-readPos);
                writePos -= readPos;
                readPos = 0;
            }
            
            int8_t *ret = mBuffer + writePos;
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

            int8_t *ret = mBuffer + readPos;
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
        int8_t *mBuffer;
        int readPos,writePos;
        const uint32_t length;
};

class RuntimeBuffer:public RuntimeEdge
{
    public:
        /* No delay argument for this version of the FIFO.
           This version will not be generated when there is a delay
        */
        explicit RuntimeBuffer(int8_t *buf):mBuffer(buf) {
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
            return(reinterpret_cast<char*>(mBuffer));
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
            return(reinterpret_cast<char*>(mBuffer));
        }

    protected:
        int8_t *mBuffer;
};

class NodeBase
{
public:
    virtual int run()=0;
    virtual int prepareForRunning()=0;
    virtual ~NodeBase() {};

    NodeBase(){};

    /* 
    Nodes are fixed and not made to be copied or moved.
    */
    NodeBase(const NodeBase&) = delete;
    NodeBase(NodeBase&&) = delete;
    NodeBase& operator=(const NodeBase&) = delete;
    NodeBase& operator=(NodeBase&&) = delete;

};

template<typename IN>
class GenericRuntimeSink:public NodeBase
{
public:
     explicit GenericRuntimeSink(const arm_cmsis_stream::Node &n,
                                 RuntimeEdge &src):ndesc(n),mSrc(src){};

     std::size_t nb_input_samples() const {return(ndesc.inputs()->Get(0)->nb());};

protected:
     IN * getReadBuffer(const int nb=0) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};

     bool willUnderflow(const int nb=0) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
};

template<typename IN,typename OUT>
class GenericRuntimeToManyNode:public NodeBase
{
public:
     explicit GenericRuntimeToManyNode(const arm_cmsis_stream::Node &n,
                                       RuntimeEdge &src,
                                       std::vector<RuntimeEdge*> dst):ndesc(n),mSrc(src),mDstList(dst){};

     std::size_t nb_input_samples() const {return(ndesc.inputs()->Get(0)->nb());};
     std::size_t nb_output_samples(const int i) const {return(ndesc.outputs()->Get(i)->nb());};

protected:
     size_t getNbOutputs() const {return(mDstList.size());};

     IN * getReadBuffer(const int nb=0) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};
     OUT * getWriteBuffer(const int io_id,
                          const int nb=0) {return (OUT*)mDstList[io_id]->getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),io_id,nb);};

     bool willUnderflow(const int nb=0) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};
     bool willOverflow(const int io_id,
                       const int nb=0) const {return mDstList[io_id]->willOverflowWith(*(ndesc.outputs()),sizeof(OUT),io_id,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
    const std::vector<RuntimeEdge*> mDstList;
};


template<typename IN,typename OUT>
class GenericRuntimeNode:public NodeBase
{
public:
     explicit GenericRuntimeNode(const arm_cmsis_stream::Node &n,
                                 RuntimeEdge &src,
                                 RuntimeEdge &dst):ndesc(n),mSrc(src),mDst(dst){};

     std::size_t nb_input_samples() const {return(ndesc.inputs()->Get(0)->nb());};
     std::size_t nb_output_samples() const {return(ndesc.outputs()->Get(0)->nb());};


protected:
     OUT * getWriteBuffer(const int nb=0 ) {return (OUT*)mDst.getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     IN * getReadBuffer(const int nb=0 ) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};

     bool willOverflow(const int nb=0 ) const {return mDst.willOverflowWith(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     bool willUnderflow(const int nb=0 ) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
    RuntimeEdge &mDst;
};

template<typename IN1,typename IN2,typename OUT>
class GenericRuntimeNode21:public NodeBase
{
public:
     explicit GenericRuntimeNode21(const arm_cmsis_stream::Node &n,
                                 RuntimeEdge &src1,
                                 RuntimeEdge &src2,
                                 RuntimeEdge &dst):
     ndesc(n),mSrc1(src1),mSrc2(src2),mDst(dst){};

     std::size_t nb_input_samples1() const {return(ndesc.inputs()->Get(0)->nb());};
     std::size_t nb_input_samples2() const {return(ndesc.inputs()->Get(1)->nb());};

     std::size_t nb_output_samples() const {return(ndesc.outputs()->Get(0)->nb());};


protected:
     OUT * getWriteBuffer(const int nb=0 ) {return (OUT*)mDst.getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     IN1 * getReadBuffer1(const int nb=0 ) {return (IN1*)mSrc1.getReadBuffer(*(ndesc.inputs()),sizeof(IN1),0,nb);};
     IN2 * getReadBuffer2(const int nb=0 ) {return (IN2*)mSrc2.getReadBuffer(*(ndesc.inputs()),sizeof(IN2),1,nb);};

     bool willOverflow(const int nb=0 ) const {return mDst.willOverflowWith(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     bool willUnderflow1(const int nb=0 ) const {return mSrc1.willUnderflowWith(*(ndesc.inputs()),sizeof(IN1),0,nb);};
     bool willUnderflow2(const int nb=0 ) const {return mSrc2.willUnderflowWith(*(ndesc.inputs()),sizeof(IN2),1,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc1;
    RuntimeEdge &mSrc2;
    RuntimeEdge &mDst;
};

template<typename OUT>
class GenericRuntimeSource:public NodeBase
{
public:
     explicit GenericRuntimeSource(const arm_cmsis_stream::Node &n,
                                   RuntimeEdge &dst):ndesc(n),mDst(dst){};

     std::size_t nb_output_samples() const {return(ndesc.outputs()->Get(0)->nb());};

protected:
     OUT * getWriteBuffer(const int nb=0) {return (OUT*)mDst.getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),0,nb);};

     bool willOverflow(const int nb=0) const {return mDst.willOverflowWith(*(ndesc.outputs()),sizeof(OUT),0,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mDst;
};

struct _rnode_t;

typedef struct _rnode_t rnode_t;

struct runtime_context {
  const arm_cmsis_stream::Schedule *schedobj;
  std::vector<std::unique_ptr<std::vector<int8_t>>> buffers;
  std::vector<std::unique_ptr<RuntimeEdge>> fifos;
  std::vector<std::unique_ptr<NodeBase>> nodes;
  std::vector<const rnode_t*> node_api;
  std::map<const std::string,NodeBase*> identification;
};


class RuntimeDuplicate:
public GenericRuntimeToManyNode<char,char>
{
public:

    explicit RuntimeDuplicate(const arm_cmsis_stream::Node &n,
              RuntimeEdge &src,
              std::vector<RuntimeEdge*> dst):
    GenericRuntimeToManyNode<char,char>(n,src,dst)
    {
    };

    constexpr static std::array<uint8_t,16> uuid    = {0xbf,0x9e,0x59,0x77,0xaa,0xf3,0x4a,0x54,0xb8,0x43,0x94,0xf4,0xa9,0x29,0x80,0x5b};


    static int runNode(NodeBase* obj)
    {
        RuntimeDuplicate *n = reinterpret_cast<RuntimeDuplicate *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(NodeBase* obj)
    {
        RuntimeDuplicate *n = reinterpret_cast<RuntimeDuplicate *>(obj);
        return(n->prepareForRunning());
    }

    static NodeBase* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        auto inputs = ndesc->inputs();
        auto outputs = ndesc->outputs();

        RuntimeEdge &i = *ctx.fifos[inputs->Get(0)->id()];
        std::vector<RuntimeEdge*> o;

        for(auto out:*outputs)
        {
            o.push_back(ctx.fifos[out->id()].get());
        }
        
        RuntimeDuplicate *node=new RuntimeDuplicate(*ndesc,i,o);
        return(static_cast<NodeBase*>(node));
    }

    int prepareForRunning() final
    {

        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           if (this->willOverflow(i))
           {
              return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
           }
        }


        return(CG_SUCCESS_ID_CODE);
    };

    int run() final {

        char *a=this->getReadBuffer();
        
        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           char *b=this->getWriteBuffer(i);
           memcpy(b,a,this->nb_input_samples());
        }
        
        return(CG_SUCCESS_ID_CODE);
    };

};

}
