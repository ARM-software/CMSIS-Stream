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
    * @param[in]  io           The flatbuffer IO description for the node
    * @param[in]  sample_size  The sample size in bytes
    * @param[in]  io_id        The output id
    * @param[in]  nb_samples   The number of samples to write
    *
    * @return     The write buffer.
    */
   virtual char* getWriteBuffer(const IOVector &io,
                                const std::size_t sample_size,
                                const int io_id,
                                const int nb_samples=0)=0;

    /**
    * @brief      Gets the read buffer.
    *
    * @param[in]  io           The flatbuffer IO description for the node
    * @param[in]  sample_size  The sample size in bytes
    * @param[in]  io_id        The input  id
    * @param[in]  nb_samples   The number of samples to read
    *
    * @return     The write buffer.
    */
   virtual char* getReadBuffer(const IOVector &io,
                               const std::size_t sample_size,
                               const int io_id,
                               const int nb_samples=0)=0;

  /**
   * @brief      Check if input will underflow (asynchronous mode)
   *
   * @param[in]  io           The flatbuffer i/o description
   * @param[in]  sample_size  The sample size in bytes
   * @param[in]  io_id        The input id
   * @param[in]  nb_samples   The number of samples to read
   *
   * @return     True if underflow would occur with this read
   */
  virtual bool willUnderflowWith(const IOVector &io,
                                 const std::size_t sample_size,
                                 const int io_id,
                                 const int nb_samples=0) const=0;

    /**
   * @brief      Check if output will overflow (asynchronous mode)
   *
   * @param[in]  io           The flatbuffer i/o description
   * @param[in]  sample_size  The sample size in bytes
   * @param[in]  io_id        The output id
   * @param[in]  nb_samples   The number of samples to write
   *
   * @return     True if overflow would occur with this write
   */
  virtual bool willOverflowWith(const IOVector &io,
                                const std::size_t sample_size,
                                const int io_id,
                                const int nb_samples=0) const=0;
        
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
            // Default value : we use the flat buffer description
            // to know how many samples are needed
            if (nb_samples==0) 
            {
                nb = io.Get(io_id)->nb()*sample_size;
            }
            // otherwise we use thr provided value
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


        /**
         * @brief      Gets the write buffer.
         *
         * @param[in]  io           not used
         * @param[in]  sample_size  not used
         * @param[in]  io_id        not used
         * @param[in]  nb_samples   not used
         *
         * @return     The write buffer.
         */
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

        /**
         * @brief      Gets the read buffer.
         *
         * @param[in]  io           not used
         * @param[in]  sample_size  not used
         * @param[in]  io_id        not used
         * @param[in]  nb_samples   not used
         *
         * @return     The read buffer.
         */
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
     * It may be cleaner to do it here rather than in the FIFO
     * since FIFO should not have knowledge of the IO.
     * The test for nb == 0 should be done here
     * and FIFO should only receive the number of bytes and nothing
     * else (future improvement)
     */
     IN * getReadBuffer(const int nb=0) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};

     /**
      * @brief      Check underflow (asynchronous mode)
      *
      * @param[in]  nb    The number of samples we would like to read
      *
      * @return     True if will underflow
      */
     bool willUnderflow(const int nb=0) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};

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
     OUT * getWriteBuffer(const int nb=0 ) {return (OUT*)mDst.getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     IN * getReadBuffer(const int nb=0 ) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};

     bool willOverflow(const int nb=0 ) const {return mDst.willOverflowWith(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     bool willUnderflow(const int nb=0 ) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};

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
     OUT * getWriteBuffer(const int nb=0) {return (OUT*)mDst.getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),0,nb);};

     bool willOverflow(const int nb=0) const {return mDst.willOverflowWith(*(ndesc.outputs()),sizeof(OUT),0,nb);};

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
