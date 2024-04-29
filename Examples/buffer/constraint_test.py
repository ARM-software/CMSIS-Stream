# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
# Include definition of the nodes
from nodes import * 

# Include definition of the nodes
from nodes import * 

from cmsis_stream.cg.yaml import *

class DidNotFail(Exception):
   pass

floatType=CType(F32)


# external buffer constraint
def test1(conf):
    conf.schedulerCFileName="cv_scheduler1"
    src=SourceC1("source",floatType,5)
    processing1=ProcessingNode("processing1",floatType,5,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i)
    g.connect(processing1.o,sink2.i)
    return g

# internal buffer constraint
def test2(conf):
    conf.schedulerCFileName="cv_scheduler2"
    src=SourceC2("source",floatType,5)
    processing1=ProcessingNode("processing1",floatType,5,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i)
    g.connect(processing1.o,sink2.i)
    return g

# Compatible constraint
def test3(conf):
    conf.schedulerCFileName="cv_scheduler3"
    src=SourceC1("source",floatType,5)
    processing1=ProcessingNodeCC("processing1",floatType,5,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i)
    g.connect(processing1.o,sink2.i)
    return g

# Incompatible constraint
def test4(conf):
    conf.schedulerCFileName="cv_scheduler4"
    src=SourceC1("source",floatType,5)
    processing1=ProcessingNodeIC("processing1",floatType,5,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i)
    g.connect(processing1.o,sink2.i)
    return g

# Constraint on a FIFO with one-to-many connection
# so a duplicate node needs to be inserted and
# it is not possible
def test5(conf):
    conf.schedulerCFileName="cv_scheduler5"
    src=Source("source",floatType,5)
    processing1=ProcessingNodeIC("processing1",floatType,5,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i,buffer="Test")
    g.connect(processing1.o,sink2.i)
    return g


# Must be array but fifo not used as array
def test6(conf):
    conf.schedulerCFileName="cv_scheduler6"
    src=SourceC1("source",floatType,5)
    processing1=ProcessingNode("processing1",floatType,7,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i)
    g.connect(processing1.o,sink2.i)
    return g

# Buffer can be used as fifo
def test7(conf):
    conf.schedulerCFileName="cv_scheduler7"
    src=SourceC3("source",floatType,5)
    processing1=ProcessingNode("processing1",floatType,7,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i)
    g.connect(processing1.o,sink2.i)
    return g

# Check contraint on an edge
def test8(conf):
    conf.schedulerCFileName="cv_scheduler8"
    src=Source("source",floatType,5)
    processing1=ProcessingNodeIC("processing1",floatType,5,5)
    sink1=Sink("sink1",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i,buffer="Test")
    return g

# Use the same custom buffer Test twice
# It is forbidden
def test9(conf):
    conf.schedulerCFileName="cv_scheduler9"
    src=SourceC1("source",floatType,5)
    processing1=ProcessingNode("processing1",floatType,5,5)
    sink1=Sink("sink1",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,processing1.i)
    g.connect(processing1.o,sink1.i,buffer="Test")
    return g

# Use a duplicate node connected to a buffer constraint
# but the buffer constraint is
# inherited from io and it may be shared with buffers of same
# size.
# In this case, duplicate node is allowed and buffer
# sharing is allowed
def test10(conf):
    conf.schedulerCFileName="cv_scheduler10"
    # Buffer that must be array and be shared
    src=SourceC3("source",floatType,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,sink1.i)
    g.connect(src.o,sink2.i)
    return g

# Similar to test10 but the buffer cannot be shared
def test11(conf):
    conf.schedulerCFileName="cv_scheduler11"
    # Buffer that must be array and cannot be be shared
    src=SourceC4("source",floatType,5)
    sink1=Sink("sink1",floatType,5)
    sink2=Sink("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,sink1.i)
    g.connect(src.o,sink2.i)
    return g

def test12(conf):
    conf.schedulerCFileName="cv_scheduler12"
    # Buffer that must be array and be shared
    src=SourceC3("source",floatType,5)
    sink=SinkC("sink",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,sink.i)
    return g


def test13(conf):
    conf.schedulerCFileName="cv_scheduler10"
    # Buffer that must be array and be shared
    src=SourceC3("source",floatType,5)
    sink1=SinkC("sink1",floatType,5)
    sink2=SinkC("sink2",floatType,5)
    
    g = Graph()
    
    g.connect(src.o,sink1.i)
    g.connect(src.o,sink2.i)
    return g
# Test and mustFail status
TESTS=[[1,test1,False]
      ,[2,test2,False]
      ,[3,test3,False]
      ,[4,test4,True]
      ,[5,test5,True]
      ,[6,test6,True]
      ,[7,test7,False]
      ,[8,test8,False]
      ,[9,test9,True]
      ,[10,test10,False]
      ,[11,test11,False]
      ,[12,test12,False]
      ,[13,test13,True]
  ]
#  
#TESTS =[[5,test5,True]]

# Create a configuration object
conf=Configuration()
# The number of schedule iteration is limited to 1
# to prevent the scheduling from running forever
# (which should be the case for a stream computation)
conf.debugLimit=1
# Disable inclusion of CMSIS-DSP headers so that we don't have
# to recompile CMSIS-DSP for such a simple example
conf.CMSISDSP = False
conf.asynchronous = False
conf.memoryOptimization=True
#conf.memStrategy = 'largest_first'
#conf.memStrategy = 'random_sequential'
#conf.memStrategy = 'smallest_last'
conf.memStrategy = 'independent_set'
#conf.memStrategy = 'connected_sequential_bfs'
#conf.memStrategy = 'connected_sequential_dfs'
#conf.memStrategy = 'saturation_largest_first'
#conf.memStrategy = 'largest_first'
conf.bufferAllocation = True
conf.cOptionalArgs=["uint8_t *myBuffer","uint8_t *myBufferB"]
conf.prefix = "cv"



def test(id,f,mustFail):
    print(f"\nTest {id}")
    try:
       g = f(conf)

       export_graph(g,f"constraint_graph_{id}.yml")
       export_config(conf,f"constraint_config{id}.yml")

       testg = import_graph(f"constraint_graph_{id}.yml")
       export_graph(testg,f"constraint_graph_b_{id}.yml")

       confb = import_config(f"constraint_config{id}.yml")
       export_config(confb,f"constraint_config_b{id}.yml")
   
       scheduling = g.computeSchedule(config=conf)
    
       # Print some statistics about the compute schedule
       # and the memory usage
       
       print("Schedule length = %d" % scheduling.scheduleLength)
       print("Memory usage %d bytes" % scheduling.memory)
       
       # Generate the C++ code for the static scheduler
       scheduling.ccode(".",conf)

       testg = import_graph(f"constraint_graph_{id}.yml")
       export_graph(testg,f"constraint_graph_b_{id}.yml")

       confb = import_config(f"constraint_config{id}.yml")
       export_config(confb,f"constraint_config_b{id}.yml")

       if mustFail:
          raise DidNotFail

    except FIFOWithCustomBufferMustBeUsedAsArray as e:
       print("Detected FIFOWithCustomBufferMustBeUsedAsArray")
       print(e)
       if not mustFail:
          raise e
    except BufferConstraintOnIOAreIncompatibles as e:
       print("Detected BufferConstraintOnIOAreIncompatibles")
       print(e)
       if not mustFail:
          raise e
    except CantHaveBufferConstraintOnFIFOWhenDuplicateIsInserted as e:
       print("Detected CantHaveBufferConstraintOnFIFOWhenDuplicateIsInserted")
       print(e)
       if not mustFail:
          raise e
    except CannotReuseCustomBufferMoreThanOnce as e:
       print("Detected CannotReuseCustomBufferMoreThanOnce")
       print(e)
       if not mustFail:
          raise e
    except Exception as e:
        raise e
    

for v in TESTS:
    test(v[0],v[1],v[2])
