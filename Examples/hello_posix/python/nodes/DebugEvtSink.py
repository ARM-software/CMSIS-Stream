from cmsis_stream.cg.scheduler import *

class DebugEvtSink(BaseNode):
    def __init__(self, name):
        BaseNode.__init__(self, name, selectors=["message"])
        self.addEventInput()

    @property
    def typeName(self):
        return "DebugEvtSink"
