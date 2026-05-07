
from cmsis_stream.cg.scheduler import *

class DebugEvtSink(BaseNode):
    def __init__(self,name):
        BaseNode.__init__(self,name,selectors=["message"])
        # Stereo output
        self.addEventInput()

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "DebugEvtSink"
    