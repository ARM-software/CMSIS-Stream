import os
import sys
import flatbuffers
import arm_cmsis_stream.UUID as UUID
import arm_cmsis_stream.Node as Node
import arm_cmsis_stream.FIFODesc as FIFODesc
import arm_cmsis_stream.IODesc as IODesc

import arm_cmsis_stream.Schedule as S
import uuid

builder = flatbuffers.Builder(0)

uuida = "c0089f592f334ec4902330f69f0f4833"
uuidb = "3ff62b0c9ad8445dbbe9208d87423446"
uuidc = "c30ea9eae9c34638bbc6021fa3549d93"

uuida = uuid.UUID(uuida).bytes
uuidb = uuid.UUID(uuidb).bytes
uuidc = uuid.UUID(uuidc).bytes
uuids=[uuida,uuidb,uuidc]




nodes=[]
for i in range(3):
    if i == 0:
        Node.NodeStartOutputsVector(builder,1)
        IODesc.CreateIODesc(builder,0,5)
        print("o 1,0,5")
        outputs = builder.EndVector()
    if i == 1:
        Node.NodeStartInputsVector(builder,1)
        IODesc.CreateIODesc(builder,0,5)
        print("i 1,0,5")
        inputs = builder.EndVector()

        Node.NodeStartOutputsVector(builder,1)
        print("o 1,1,5")
        IODesc.CreateIODesc(builder,1,5)
        outputs = builder.EndVector()
    if i == 2:
        Node.NodeStartInputsVector(builder,1)
        print("i 1,1,5")
        IODesc.CreateIODesc(builder,1,5)
        inputs = builder.EndVector()
    Node.Start(builder)
    Node.AddUuid(builder, UUID.CreateUUID(builder,uuids[i]))
    Node.AddId(builder, i)
    if i == 0:
        Node.AddOutputs(builder,outputs)
    if i == 1:
        Node.AddInputs(builder,inputs)
        Node.AddOutputs(builder,outputs)
    if i == 2:
        Node.AddInputs(builder,inputs)

    node = Node.End(builder)
    nodes.append(node)

fifos=[]
for i in range(2):
    FIFODesc.Start(builder)
    FIFODesc.AddId(builder,i)
    FIFODesc.AddLength(builder,5*4)
    fifos.append(FIFODesc.End(builder))



S.StartNodesVector(builder,len(nodes))
for n in reversed(nodes):
   builder.PrependUOffsetTRelative(n)
the_nodes=builder.EndVector()

S.StartNodesVector(builder,len(fifos))
for f in reversed(fifos):
   builder.PrependUOffsetTRelative(f)
the_fifos=builder.EndVector()

S.ScheduleStartScheduleVector(builder,3)
builder.PrependUint32(2)
builder.PrependUint32(1)
builder.PrependUint32(0)
the_sched=builder.EndVector()


S.Start(builder)
S.AddAsyncMode(builder,False)
S.AddNodes(builder,the_nodes)
S.AddFifos(builder,the_fifos)
S.AddSchedule(builder,the_sched)
sched=S.End(builder)


builder.Finish(sched)
buf = builder.Output()

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

with open("sched_flat.c", "w") as f:
    print(ARRAY_C_START,file=f)
    nb = 0
    for b in buf:
        print("%d, " % b,end="",file=f)
        nb = nb + 1
        if (nb==10):
            nb=0
            print("",file=f)
    print(ARRAY_C_STOP,file=f)

with open("sched_flat.h", "w") as f:
    print(ARRAY_H % len(buf),file=f)