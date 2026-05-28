from cmsis_stream.cg.scheduler import *

class DebugSink(GenericSink):
    def __init__(self, name, theType):
        GenericSink.__init__(self, name, selectors=["message"])
        self.addInput("i", theType, 1)
        self.addEventOutput()

    @property
    def typeName(self):
        return "DebugSink"
