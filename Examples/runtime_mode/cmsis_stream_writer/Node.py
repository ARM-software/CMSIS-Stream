# automatically generated by the FlatBuffers compiler, do not modify

# namespace: arm_cmsis_stream

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class Node(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAs(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Node()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def GetRootAsNode(cls, buf, offset=0):
        """This method is deprecated. Please switch to GetRootAs."""
        return cls.GetRootAs(buf, offset)
    # Node
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # Node
    def Uuid(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            x = o + self._tab.Pos
            from arm_cmsis_stream.UUID import UUID
            obj = UUID()
            obj.Init(self._tab.Bytes, x)
            return obj
        return None

    # Node
    def Id(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, o + self._tab.Pos)
        return 0

    # Node
    def Inputs(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            x = self._tab.Vector(o)
            x += flatbuffers.number_types.UOffsetTFlags.py_type(j) * 4
            from arm_cmsis_stream.IODesc import IODesc
            obj = IODesc()
            obj.Init(self._tab.Bytes, x)
            return obj
        return None

    # Node
    def InputsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # Node
    def InputsIsNone(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        return o == 0

    # Node
    def Outputs(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            x = self._tab.Vector(o)
            x += flatbuffers.number_types.UOffsetTFlags.py_type(j) * 4
            from arm_cmsis_stream.IODesc import IODesc
            obj = IODesc()
            obj.Init(self._tab.Bytes, x)
            return obj
        return None

    # Node
    def OutputsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # Node
    def OutputsIsNone(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        return o == 0

    # Node
    def NodeData(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(12))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Int8Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 1))
        return 0

    # Node
    def NodeDataAsNumpy(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(12))
        if o != 0:
            return self._tab.GetVectorAsNumpy(flatbuffers.number_types.Int8Flags, o)
        return 0

    # Node
    def NodeDataLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(12))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # Node
    def NodeDataIsNone(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(12))
        return o == 0

    # Node
    def Name(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(14))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

def NodeStart(builder): builder.StartObject(6)
def Start(builder):
    return NodeStart(builder)
def NodeAddUuid(builder, uuid): builder.PrependStructSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(uuid), 0)
def AddUuid(builder, uuid):
    return NodeAddUuid(builder, uuid)
def NodeAddId(builder, id): builder.PrependUint16Slot(1, id, 0)
def AddId(builder, id):
    return NodeAddId(builder, id)
def NodeAddInputs(builder, inputs): builder.PrependUOffsetTRelativeSlot(2, flatbuffers.number_types.UOffsetTFlags.py_type(inputs), 0)
def AddInputs(builder, inputs):
    return NodeAddInputs(builder, inputs)
def NodeStartInputsVector(builder, numElems): return builder.StartVector(4, numElems, 2)
def StartInputsVector(builder, numElems):
    return NodeStartInputsVector(builder, numElems)
def NodeAddOutputs(builder, outputs): builder.PrependUOffsetTRelativeSlot(3, flatbuffers.number_types.UOffsetTFlags.py_type(outputs), 0)
def AddOutputs(builder, outputs):
    return NodeAddOutputs(builder, outputs)
def NodeStartOutputsVector(builder, numElems): return builder.StartVector(4, numElems, 2)
def StartOutputsVector(builder, numElems):
    return NodeStartOutputsVector(builder, numElems)
def NodeAddNodeData(builder, nodeData): builder.PrependUOffsetTRelativeSlot(4, flatbuffers.number_types.UOffsetTFlags.py_type(nodeData), 0)
def AddNodeData(builder, nodeData):
    return NodeAddNodeData(builder, nodeData)
def NodeStartNodeDataVector(builder, numElems): return builder.StartVector(1, numElems, 1)
def StartNodeDataVector(builder, numElems):
    return NodeStartNodeDataVector(builder, numElems)
def NodeAddName(builder, name): builder.PrependUOffsetTRelativeSlot(5, flatbuffers.number_types.UOffsetTFlags.py_type(name), 0)
def AddName(builder, name):
    return NodeAddName(builder, name)
def NodeEnd(builder): return builder.EndObject()
def End(builder):
    return NodeEnd(builder)