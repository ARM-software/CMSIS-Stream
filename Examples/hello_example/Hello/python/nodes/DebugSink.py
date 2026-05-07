
from cmsis_stream.cg.scheduler import *

class DebugSink(GenericSink):
    def __init__(self,name,theType,outLength):
        GenericSink.__init__(self,name)
        # Stereo output
        self.addInput("i",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "DebugSink"
    