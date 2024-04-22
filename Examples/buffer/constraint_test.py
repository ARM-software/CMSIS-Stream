# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
# Include definition of the nodes
from nodes import * 

# Include definition of the nodes
from nodes import * 

from cmsis_stream.cg.yaml import *

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

TESTS=[[test1,False]
      ,[test2,False]
      ,[test3,False]
      ,[test4,True]
      ,[test5,True]
      ,[test6,True]
      ,[test7,False]
      ,[test8,False]
      ,[test9,True]
  ]

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
conf.memStrategy = 'independent_set'
conf.bufferAllocation = True
conf.cOptionalArgs=["uint8_t *myBuffer","uint8_t *myBufferB"]
conf.prefix = "cv"



def test(id,f,mustFail):
    print(f"\nTest {id+1}")
    try:
       g = f(conf)

       export_graph(g,f"constraint_graph_{id+1}.yml")
       export_config(conf,f"constraint_config{id+1}.yml")

       testg = import_graph(f"constraint_graph_{id+1}.yml")
       export_graph(testg,f"constraint_graph_b_{id+1}.yml")

       confb = import_config(f"constraint_config{id+1}.yml")
       export_config(confb,f"constraint_config_b{id+1}.yml")
   
       scheduling = g.computeSchedule(config=conf)
    
       # Print some statistics about the compute schedule
       # and the memory usage
       
       print("Schedule length = %d" % scheduling.scheduleLength)
       print("Memory usage %d bytes" % scheduling.memory)
       
       # Generate the C++ code for the static scheduler
       scheduling.ccode(".",conf)

       testg = import_graph(f"constraint_graph_{id+1}.yml")
       export_graph(testg,f"constraint_graph_b_{id+1}.yml")

       confb = import_config(f"constraint_config{id+1}.yml")
       export_config(confb,f"constraint_config_b{id+1}.yml")

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
    

for i,v in enumerate(TESTS):
    test(i,v[0],v[1])
