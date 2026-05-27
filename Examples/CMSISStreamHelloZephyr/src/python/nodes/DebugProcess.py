from cmsis_stream.cg.scheduler import *

class DebugProcess(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name,identified=True)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,1)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "DebugProcess"