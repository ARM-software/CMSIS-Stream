# Classes used to identify objects in the C code
# and know how to use them as argument, or to call 
# a method of the object
# It depends if the initialization is stacj based or
# heap based. 
class ArgsObject:
    def __init__(self,name):
        self._name = name 

    @property
    def variable(self):
        return self._name

    # For use in argument of constructors
    @property
    def reference(self):
        return f"{self._name}"

    @property
    def pointer(self):
        return f"&{self._name}"

    @property
    def access(self):
        return f"{self._name}."

class ArgsPtrObj(ArgsObject):
    def __init__(self,name,owner=None):
        ArgsObject.__init__(self,name)
        self.owner=owner

    @property
    def reference(self):
        if self.owner:
           return f"*({self.owner}.{self._name})"
        else:
           return f"*{self._name}"

    @property
    def pointer(self):
        if self.owner:
          return f"{self.owner}.{self._name}"
        else:
           return f"{self._name}"

    @property
    def access(self):
        if self.owner:
           return f"{self.owner}.{self._name}->"
        else:
           return f"{self._name}->"

class FifoID(ArgsObject):
    def __init__(self,i):
        ArgsObject.__init__(self,f"fifo{i}")

class FifoPtrID(ArgsPtrObj):
    def __init__(self,i,owner=None):
        ArgsPtrObj.__init__(self,f"fifo{i}",owner=owner)

class NodeID(ArgsObject):
    def __init__(self,i):
        ArgsObject.__init__(self,"i")

class NodePtrID(ArgsPtrObj):
    def __init__(self,i,owner=None):
        ArgsPtrObj.__init__(self,"i",owner=owner)