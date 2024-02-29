ARRAY_C_START="""#include "sched_flat.h"

unsigned char sched[SCHED_LEN]={"""

ARRAY_C_STOP="""};

"""

ARRAY_H="""#ifndef SCHED_FLAT_H
#define SCHED_FLAT_H

#define SCHED_LEN %d

#ifdef   __cplusplus
extern "C"
{
#endif

extern unsigned char sched[SCHED_LEN];

#ifdef   __cplusplus
}
#endif

#endif
"""

import flatbuffers
import cmsis_stream_writer.UUID as UUID
import cmsis_stream_writer.Node as Node
import cmsis_stream_writer.FIFODesc as FIFODesc
import cmsis_stream_writer.IODesc as IODesc
import cmsis_stream_writer.BufferDesc as BufferDesc

import cmsis_stream_writer.Schedule as S
import uuid
from cmsis_stream.cg.scheduler import Duplicate

# We need a different uuid for different datatype
# For duplicate node it would be annoying
# So duplicate node is lways forced to int8
# and nb samples are expressed in bytes
# 
DUPLICATE_UUID = "bf9e5977aaf34a54b84394f4a929805b"
Duplicate.uuid = property(lambda self: DUPLICATE_UUID)
Duplicate.node_data = property(lambda self: None)

def mkUUID(builder,u):
    b = uuid.UUID(u).bytes
    return(UUID.CreateUUID(builder,b))

def gen(sched,conf):
    nbFifos = len(sched._graph._allFIFOs)
    fifos=sched._graph._allFIFOs
    nbNodes=len(sched.nodes)
    nodes=sched.nodes
    schedule=sched.schedule
    schedLen=len(sched.schedule)

    builder = flatbuffers.Builder(0)


    flat_nodes=[]
    for i,n in enumerate(nodes):
         inputs = None
         outputs = None
         node_data = None
         #print(n.nodeName)
         if len(n.inputNames)>0:
            #print("inputs")
            #print(len(n.inputNames))
            Node.NodeStartInputsVector(builder,len(n.inputNames))

            for ioName in n.inputNames:
                x = n._inputs[ioName]
                nb = 0
                if isinstance(x.nbSamples,int):
                    nb = x.nbSamples
                else:
                    # cyclo static scheduling
                    nb = np.max(x.nbSamples)

                # Duplicate node forced to int8
                # so that we have a global UUID for
                # any duplicate node instance
                # so nb samples converted to bytes
                if n.uuid == DUPLICATE_UUID:
                    nb = nb * x.theType.bytes

                fifoid = sched.fifoID(x.fifo)

                IODesc.CreateIODesc(builder,fifoid,nb)
                #print(f"{fifoid} {nb}")
                #print(f"{ioName},{nb},{fifoid}")

            inputs = builder.EndVector()


         if len(n.outputNames)>0:
            #print("outputs")
            #print(len(n.outputNames))
            Node.NodeStartOutputsVector(builder,len(n.outputNames))
            for ioName in n.outputNames:
                x = n._outputs[ioName]
                nb = 0
                if isinstance(x.nbSamples,int):
                    nb = x.nbSamples
                else:
                    # cyclo static scheduling
                    nb = np.max(x.nbSamples)
                
                # Duplicate node forced to int8
                # so that we have a global UUID for
                # any duplicate node instance
                # so nb samples converted to bytes
                if n.uuid == DUPLICATE_UUID:
                    nb = nb * x.theType.bytes
                fifoid = sched.fifoID(x.fifo)

                IODesc.CreateIODesc(builder,fifoid,nb)
                #print(f"{fifoid} {nb}")
                #print(f"{ioName},{nb},{fifoid}")

            outputs = builder.EndVector()


         if n.node_data is not None:
            Node.StartNodeDataVector(builder,len(n.node_data))
            for i in reversed(n.node_data):
                builder.PrependByte(i)
            node_data = builder.EndVector()

         node_name = None
         if conf.nodeIdentification:
            if n.identified:
               node_name = builder.CreateString(n.nodeName)

         Node.Start(builder)
         Node.AddUuid(builder, mkUUID(builder,n.uuid))
         Node.AddId(builder, i)
         #print(i)
         #print(n.uuid)
         if inputs is not None:
            Node.AddInputs(builder,inputs)
         if outputs is not None:
            Node.AddOutputs(builder,outputs)
         if node_data is not None:
            Node.AddNodeData(builder,node_data)
         if node_name is not None:
            Node.AddName(builder,node_name)
        

         node = Node.End(builder)
         flat_nodes.append(node)

    flat_buffers = [] 
    for i,buffer in enumerate(sched._graph._allBuffers):
        BufferDesc.Start(builder)
        l = buffer._length * buffer._theType.bytes
        BufferDesc.AddLength(builder,l)
        flat_buffers.append(BufferDesc.End(builder))

    flat_fifos=[]
    for i,fifo in enumerate(fifos):
        FIFODesc.Start(builder)
        FIFODesc.AddId(builder,i)
        nb = fifo.length
        nb = nb * fifo.theType.bytes
        FIFODesc.AddLength(builder,nb)

        FIFODesc.AddBufid(builder,fifo.buffer._bufferID)

        if fifo.hasDelay:
           FIFODesc.AddDelay(builder,fifo.delay*fifo.theType.bytes)
        else:
           FIFODesc.AddDelay(builder,0)
        if fifo.isArrayAsInt:
            FIFODesc.AddBuffer(builder,True)
        else:
            FIFODesc.AddBuffer(builder,False)
        flat_fifos.append(FIFODesc.End(builder))

    S.StartNodesVector(builder,len(flat_nodes))
    for n in reversed(flat_nodes):
       builder.PrependUOffsetTRelative(n)
    the_nodes=builder.EndVector()

    S.StartNodesVector(builder,len(flat_buffers))
    for n in reversed(flat_buffers):
       builder.PrependUOffsetTRelative(n)
    the_buffers=builder.EndVector()
    
    S.StartNodesVector(builder,len(flat_fifos))
    for f in reversed(flat_fifos):
       builder.PrependUOffsetTRelative(f)
    the_fifos=builder.EndVector()



    S.ScheduleStartScheduleVector(builder,len(sched.schedule))
    for s in reversed(sched.schedule):
        #print(sched.nodes[s].codeID)
        builder.PrependUint16(sched.nodes[s].codeID)
    the_sched=builder.EndVector()



    S.Start(builder)
    S.AddAsyncMode(builder,conf.asynchronous)
    S.AddNodes(builder,the_nodes)
    S.AddBuffers(builder,the_buffers)
    S.AddFifos(builder,the_fifos)
    S.AddSchedule(builder,the_sched)
    flat_sched=S.End(builder)


    builder.Finish(flat_sched)
    buf = builder.Output()
    return(buf)

    

def create_c_file(buf,name):
    with open(f"{name}.c", "w") as f:
        print(ARRAY_C_START,file=f)
        nb = 0
        for b in buf:
            print("%d, " % b,end="",file=f)
            nb = nb + 1
            if (nb==10):
                nb=0
                print("",file=f)
        print(ARRAY_C_STOP,file=f)

def create_h_file(buf,name):
    with open(f"{name}.h", "w") as f:
        print(ARRAY_H % len(buf),file=f)

def create_bin(buf,name):
    with open(f"{name}.dat", "wb") as f:
        f.write(buf)