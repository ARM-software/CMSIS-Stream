#pragma once

#include <cstdint>
#include "stream_generated.h"
#include "cg_status.h"

#define CG_SUCCESS_ID_CODE (0)
#define CG_SKIP_EXECUTION_ID_CODE (-5)
#define CG_BUFFER_ERROR_ID_CODE (-6)

namespace arm_cmsis_stream {

using IOVector = flatbuffers::Vector<const arm_cmsis_stream::IODesc*>;

/**
 * @brief      Virtual class for edges (FIFO or buffer)
 */
class RuntimeEdge
{
public:
   RuntimeEdge(){};
   virtual ~RuntimeEdge(){};

   /**
    * @brief      Gets the write buffer.
    *
    * @param[in]  nb_bytes   The number of bytes to write
    *
    * @return     The write buffer.
    */
   virtual char* getWriteBuffer(const int nb_bytes)=0;

    /**
    * @brief      Gets the read buffer.
    *
    * @param[in]  nb_bytes   The number of bytes to read
    *
    * @return     The read buffer.
    */
   virtual char* getReadBuffer(const int nb_bytes)=0;

  /**
   * @brief      Check if input will underflow (asynchronous mode)
   *
   * @param[in]  nb_bytes   The number of bytes to read
   *
   * @return     True if underflow would occur with this read
   */
  virtual bool willUnderflowWith(const int nb_bytes) const=0;

    /**
   * @brief      Check if output will overflow (asynchronous mode)
   *
   * @param[in]  nb_bytes   The number of bytes to write
   *
   * @return     True if overflow would occur with this write
   */
  virtual bool willOverflowWith(const int nb_bytes) const=0;
        
  /**
   * @brief      Number of bytes in the FIFO (asynchronous mode)
   *
   * @return     Number of bytes in the FIFO
   */
  virtual int nbBytesInFIFO() const =0;

  /**
   * @brief      Number of available bytes in the FIFO (asynchronous mode)
   *
   * @return     Number of available bytes in the FIFO
   */
  virtual int nbOfFreeBytesInFIFO() const=0;
};


/**
 * @brief      This class describes a runtime fifo.
 */
class RuntimeFIFO:public RuntimeEdge
{
    public:
        /**
         * @brief      Constructs a new instance.
         *
         * @param      buf    The buffer
         * @param[in]  nb     The number of bytes in the buffer
         * @param[in]  delay  The delay in bytes
         */
        explicit RuntimeFIFO(int8_t *buf,const uint32_t nb,int delay=0):
        mBuffer(buf),readPos(0),writePos(delay),length(nb) {
        };

        
        
        RuntimeFIFO(RuntimeFIFO&&) = default;
        RuntimeFIFO& operator=(RuntimeFIFO&&) = default;

        RuntimeFIFO(const RuntimeFIFO&) = default;
        RuntimeFIFO& operator=(const RuntimeFIFO&) = default;

        /* 

        Check for overflow must have been done
        before using this function 
        
        */

        char* getWriteBuffer(const int nb)  final
        {
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
        
        char* getReadBuffer(const int nb)    final
        {
            int8_t *ret = mBuffer + readPos;
            readPos += nb;
            return((char*)ret);
        }

        bool willUnderflowWith(const int nb) const   final
        {
            return((writePos - readPos - nb)<0);
        }

        bool willOverflowWith(const int nb) const   final
        {
            return((writePos - readPos + nb)>length);
        }

        int nbBytesInFIFO() const final   {return (writePos - readPos);};
        
        int nbOfFreeBytesInFIFO() const  final  {return (length - writePos + readPos);};



    protected:
        int8_t *mBuffer;
        int readPos,writePos;
        const uint32_t length;
};

/**
 * @brief      This class describes a runtime buffer.
 * 
 * Used when a FIFO is used as a simple buffer (detected by the Python
 * scheduling and recorded in the flatbuffer)
 */
class RuntimeBuffer:public RuntimeEdge
{
    public:
        /* No delay argument for this version of the FIFO.
           This version will not be generated when there is a delay
        */

        /**
         * @brief      Constructs a new instance.
         *
         * @param      buf   The buffer
         * 
         * No delay argument. When there is a delay, it cannot be
         * a buffer and a FIFO is generated instead.
         */
        explicit RuntimeBuffer(int8_t *buf):mBuffer(buf) {
        };

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

        bool willUnderflowWith(const int nb_bytes) const final 
        {
            (void)nb_bytes;
            return false;
        };
        
        bool willOverflowWith(const int nb_bytes) const final   
        {
            (void)nb_bytes;
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


        char* getWriteBuffer(const int nb_bytes)  final  
        {
            (void)nb_bytes;
            return(reinterpret_cast<char*>(mBuffer));
        };

        char* getReadBuffer(const int nb_bytes)    final
        {
            (void)nb_bytes;
            return(reinterpret_cast<char*>(mBuffer));
        }

    protected:
        int8_t *mBuffer;
};

/**
 * @brief      Virtual class for all nodes
 */
class NodeBase
{
public:
    /**
     * @brief      Virtual functino for running the node
     *
     * @return     Return the error code
     */
    virtual int run()=0;

    /**
     * @brief      Virtual function to check if node can be executed in asynchronous mode
     *
     * @return     Return the error code
     */
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

// Get number of bytes to read on input ID
// If number of samples is not provided, it is read from the flatbuffer
// otherwise the provided value is used
#define GET_INPUT_NB(IN,NB_SAMPLES,ID)                  \
    int nb;                                             \
                                                        \
    if ((NB_SAMPLES)==0)                                \
    {                                                   \
        nb = ndesc.inputs()->Get((ID))->nb()*sizeof(IN);\
    }                                                   \
    else                                                \
    {                                                   \
        nb = (NB_SAMPLES)*sizeof(IN);                   \
    }

// Get number of bytes to write on output ID
// If number of samples is not provided, it is read from the flatbuffer
// otherwise the provided value is used
#define GET_OUTPUT_NB(OUT,NB_SAMPLES,ID)                  \
    int nb;                                               \
                                                          \
    if ((NB_SAMPLES)==0)                                  \
    {                                                     \
        nb = ndesc.outputs()->Get((ID))->nb()*sizeof(OUT);\
    }                                                     \
    else                                                  \
    {                                                     \
        nb = (NB_SAMPLES)*sizeof(OUT);                    \
    }


/**
 * @brief      This class describes a generic runtime sink.
 *
 * @tparam     <unnamed>  Input datatype
 */
template<typename IN>
class GenericRuntimeSink:public NodeBase
{
public:
     explicit GenericRuntimeSink(const arm_cmsis_stream::Node &n,
                                 RuntimeEdge &src):ndesc(n),mSrc(src){};

     /**
      * @brief      Number of input samples read on this input as described by the flatbuffer
      *
      * @return     The number of bytes
      */
     std::size_t nb_input_samples() const {return(ndesc.inputs()->Get(0)->nb());};

protected:
     /**
     * @brief      Gets the read buffer.
     *
     * @param[in]  nb    The number of samples we want to read
     *
     * @return     The read buffer.
     * 
     * RuntimeFIFO is untyped. Those functions reintroduces types
     * so that the implementation of the run function can used
     * typed buffers.
     * Also, with those functions the developer is working with
     * samples instead of bytes.
     * 
     * Default value of 0 is interpreted by the FIFO code
     * and use the description from the flatbuffer to know
     * the number of samples.
     *
     */
     IN * getReadBuffer(const int nb_samples=0)
     {
        GET_INPUT_NB(IN,nb_samples,0);
        return (IN*)mSrc.getReadBuffer(nb);
     };

     /**
      * @brief      Check underflow (asynchronous mode)
      *
      * @param[in]  nb    The number of samples we would like to read
      *
      * @return     True if will underflow
      */
     bool willUnderflow(const int nb_samples=0) const 
     {
        GET_INPUT_NB(IN,nb_samples,0);
        return mSrc.willUnderflowWith(nb);
     };

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
};

/**
 * @brief      This class describes a generic runtime to many node.
 *
 * @tparam     <unnamed>  Input datatype
 * @tparam     <unnamed>  Output datatype
 */
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

     IN * getReadBuffer(const int nb_samples=0) 
     {
        GET_INPUT_NB(IN,nb_samples,0);
        return (IN*)mSrc.getReadBuffer(nb);
     };

     OUT * getWriteBuffer(const int io_id,
                          const int nb_samples=0) 
     {
        GET_OUTPUT_NB(OUT,nb_samples,io_id);
        return (OUT*)mDstList[io_id]->getWriteBuffer(nb);
     };

     bool willUnderflow(const int nb_samples=0) const 
     {
        GET_INPUT_NB(IN,nb_samples,0);
        return mSrc.willUnderflowWith(nb);
     };

     bool willOverflow(const int io_id,
                       const int nb_samples=0) const 
     {
        GET_OUTPUT_NB(OUT,nb_samples,io_id);
        return mDstList[io_id]->willOverflowWith(nb);
     };

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
    const std::vector<RuntimeEdge*> mDstList;
};

/**
 * @brief      This class describes a generic runtime node.
 *
 * @tparam     <unnamed>  Input datatype
 * @tparam     <unnamed>  Output datatype
 */
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
     OUT * getWriteBuffer(const int nb_samples=0 ) 
     {
        GET_OUTPUT_NB(OUT,nb_samples,0);
        return (OUT*)mDst.getWriteBuffer(nb);
     };
     
     IN * getReadBuffer(const int nb_samples=0 ) 
     {
        GET_INPUT_NB(IN,nb_samples,0);
        return (IN*)mSrc.getReadBuffer(nb);
     };

     bool willOverflow(const int nb_samples=0 ) const 
     {
        GET_OUTPUT_NB(OUT,nb_samples,0);
        return mDst.willOverflowWith(nb);
     };
     
     bool willUnderflow(const int nb_samples=0 ) const 
     {
        GET_INPUT_NB(IN,nb_samples,0);
        return mSrc.willUnderflowWith(nb);
     };

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
    RuntimeEdge &mDst;
};

/**
 * @brief      This class describes a generic runtime node 21.
 *
 * @tparam     IN1        Input datatype for 1st input
 * @tparam     IN2        Input datatype for 2nd input
 * @tparam     <unnamed>  Output datatype
 */
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
     OUT * getWriteBuffer(const int nb_samples=0 ) 
     {
        GET_OUTPUT_NB(OUT,nb_samples,0);
        return (OUT*)mDst.getWriteBuffer(nb);
     };
     
     IN1 * getReadBuffer1(const int nb_samples=0 ) 
     {
        GET_INPUT_NB(IN1,nb_samples,0);
        return (IN1*)mSrc1.getReadBuffer(nb);
     };
     
     IN2 * getReadBuffer2(const int nb_samples=0 ) 
     {
        GET_INPUT_NB(IN2,nb_samples,1);
        return (IN2*)mSrc2.getReadBuffer(nb);
     };

     bool willOverflow(const int nb_samples=0 ) const 
     {
        GET_OUTPUT_NB(OUT,nb_samples,0);
        return mDst.willOverflowWith(nb);
     };
     
     bool willUnderflow1(const int nb_samples=0 ) const 
     {
        GET_INPUT_NB(IN1,nb_samples,0);
        return mSrc1.willUnderflowWith(nb);
     };
     
     bool willUnderflow2(const int nb_samples=0 ) const 
     {
        GET_INPUT_NB(IN2,nb_samples,1);
        return mSrc2.willUnderflowWith(nb);
     };

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc1;
    RuntimeEdge &mSrc2;
    RuntimeEdge &mDst;
};

/**
 * @brief      This class describes a generic runtime source.
 *
 * @tparam     <unnamed>  Output datatype
 */
template<typename OUT>
class GenericRuntimeSource:public NodeBase
{
public:
     explicit GenericRuntimeSource(const arm_cmsis_stream::Node &n,
                                   RuntimeEdge &dst):ndesc(n),mDst(dst){};

     std::size_t nb_output_samples() const {return(ndesc.outputs()->Get(0)->nb());};

protected:
     OUT * getWriteBuffer(const int nb_samples=0) 
     {
        GET_OUTPUT_NB(OUT,nb_samples,0);
        return (OUT*)mDst.getWriteBuffer(nb);
    };

     bool willOverflow(const int nb_samples=0) const 
     {
        GET_OUTPUT_NB(OUT,nb_samples,0);
        return mDst.willOverflowWith(nb);
    };

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mDst;
};


/**
 * @brief      This class describes a runtime duplicate.
 * 
 * Duplicate nodes are inserted in the graph by the scheduling
 * Python script to handle one-to-many connections : when an output is
 * connected to several nodes,
 */
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
