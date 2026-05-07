from cmsis_stream.cg.scheduler import *

class DebugSource(GenericSource):
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name,identified=True)
        # Stereo output
        self.addOutput("o",theType,outLength)
        self.addVariableArg(f"params->{name}")

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "DebugSource"