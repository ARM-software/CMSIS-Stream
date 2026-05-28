from cmsis_stream.cg.scheduler import *

class DebugSource(GenericSource):
    def __init__(self, name, theType, inLength):
        GenericSource.__init__(self, name, identified=True)
        self.addOutput("o", theType, inLength)
        self.addVariableArg(f"params->{name}")

    @property
    def typeName(self):
        return "DebugSource"
