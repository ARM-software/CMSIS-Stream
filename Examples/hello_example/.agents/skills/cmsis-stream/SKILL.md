---
name: cmsis-stream
description: This skill must be used to create new components for CMSIS Stream. It generates the Python description and C++ wrapper for a component.
---

# CMSIS Stream

## When to use this skill
When the user needs to create a new component for CMSIS Stream, they can use this skill to generate the Python description and C++ wrapper for the component. This skill will help the user to quickly create the necessary files and structure for the new component, allowing them to focus on implementing the functionality rather than setting up the project.

## Standard workflow for using this skill
1. The user will provide the necessary information about the new component, such as its name, C++ class name, and any other relevant details.
2. The skill will generate the Python description and C++ wrapper for the component.
3. The user can then implement the functionality of the component in the generated files.

If the user does not provide all the necessary information, the skill will ask for the missing details before proceeding with the generation of the files.
Don't make any assumptions about the component and ask for all the necessary information to create a complete and accurate description of the component.

You can use following template to ask for the necessary information from the user:
```text
C++ class: DSP
Ports: 1 dataflow input i, 1 dataflow output o
Types: F32
Lengths: variable
Parameters: none
Visible: no
State: no
Events: list of names or default (for no new event introduced)
Parameters: list of names or default (for no parameter)
```
The name of the ports are given in the description (i and o in the above template) but the user can choose other names if they want. The type and length of the ports can be fixed or variable. If they are variable, it means that the component can work with different types and lengths and those information will be passed as arguments to the constructor of the component.

## Information needed from the user to describe the component
- Component name: The name of Python class for the new component.
- C++ class name: The name of the C++ class that will be generated for the component.
- Number of inputs and output ports
- For each port : is it a dataflow port or event port
- For a dataflow port, the data type and the size of the data (in number of samples) if this number cannot been changed. If it can be changed it will be a variable
- If events are used, the node may introduce new events and the user should provide the name of the new events.
- The user must say if the component has parameters than can be set at initialization
- The user must say if the component is visible from the outside world
- The user must specify if the component has a state that must be managed during context switches


## Python description for the component

The Python description must be generated in the folder containing all Python descriptions. Generally it is named `nodes` and contain a `__init__.py` file.
The `__init__.py`  must be updated to import the new component.

If the name of the component is `MyComponent`, the Python description must be named `MyComponent.py` .

The component file must include CMSIS Stream definitions from the CMSIS Stream python package:
```python
from cmsis_stream.cg.scheduler import *
```

The component file must contain a class named `MyComponent`. The class must have an `__init__` method that initializes the component and defines its ports, parameters, events, and state if necessary. The class must have an `typeName` property that returns the C++ class name as a string.

If the class only has event ports, it inherits from `BaseNode`.
If the class has some dataflow ports it must inherit from `GenericSink` if those dataflow ports are only input ports, from `GenericSource` if those dataflow ports are only output ports, and from `GenericNode` if those dataflow ports are both input and output ports.

The `__init__` method must define the dataflow port using:
- `self.addInput(inputPortName,theType,inLength)`
- `self.addOutput(outputPortName,theType,inLength)`

The input port name is generally named `i` or in case of several input ports `i1`, `i2`, etc. The output port name is generally named `o` or in case of several output ports `o1`, `o2`, etc.
The type is defined with `CType(DT)` where `DT` as defined by CMSIS Stream can be : 
- F64
- F32
- F16
- Q31
- Q15
- Q7
- UINT32
- UINT16
- UINT8
- SINT32
- SINT16
- SINT8

Those types are generally passed as argument of the constructor if the port can work with different types. If the type is fixed, it can be directly defined in the component description.
The length is also generally passed as argument of the constructor if the port can work with different lengths. If the length is fixed, it can be directly defined in the component description.

If the component has parameters, they must be defined in the `__init__` method using  `self.addVariableArg(f"params->{name}")`.
It will add a new argument to the C++ constructor and code generated by CMSIS Stream will automatically set the value of this argument to the value of the variable `params->{name}` where name is the name of the node instantiating this component.

`self.addVariableArg` must be added after defining all the ports.

If the port is an event port, it can be defined with:
`self.addEventInput()` or `self.addEventOutput()`

Those functions take an optional argument `nb` if several event inputs or event outputs must be created. It is also possible to use those functions several times to create several event inputs or outputs.

The parent class supports some optional named parameters in the constructor.
- `identified` that can be `True` or `False`
- `selectors` that can be set to a list of strings to define event names recognized by the component

If the component has no event output port but must still be able to send events, the Python should define a new argument for the C++ constructor
```python
self.addVariableArg("evtQueue")
```

The name `evtQueue` is special and will be used by the code generator to set the value of this argument to the pointer of the EventQueue that is used internally by the component to send events.

## C++ description for the component

Generate a new header file for the component in the folder containing all C++ descriptions. 
The header file uses the C++ name defined during description of the component and is named `MyComponent.hpp` if the C++ class name is `MyComponent`.

The generated file will contain the definition for the C++ component.

The file must start with
```C++
#pragma once



#include "cg_enums.h"
#include "app_config.hpp"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"


using namespace arm_cmsis_stream;
```

If the component has any dataflow port then it will be implemented as a template. If the component has no dataflow port then it will be defined as a normal class inheriting from public StreamNode.

If the component has input and output dataflow ports, it must inherit from `GenericNode` and the template parameters of `GenericNode` must be set according to the type and length of the ports. If the component has only input dataflow ports, it must inherit from `GenericSink` and if it has only output dataflow ports, it must inherit from `GenericSource`.

The template arguments are as follow:
- The list of inputs come first
- The list of output follows

For each port, the template has two arguments:
- A typename for the sample type
- An int for the number of samples processed by the port (consumed or produced)

If the component has any dataflow port, it must implement the `run` method that will be called by the CMSIS Stream scheduler to process the data. The `run` method must read the input data from the input ports, process it, and write the output data to the output ports.

If the component has any event output port, it must implement the 
```C++
void subscribe(int outputPort,StreamNode &dst,int dstPort)
```

and must define protected member variables of type `EventOutput` for each event output.

The argument of the constructor are the FIFOs for the input, followed by the FIFOs for the output and followed by an EventQueue if there is any event output port. 

Example:
```C++
DebugSink(FIFOBase<IN> &dst,EventQueue *queue)
```

If the component defines any new event, it must define a static array to make a link between global ID of the event and a local ID.
Example:
```C++
public:
  enum selector {selMessage=0};
  static std::array<uint16_t,1> selectors;
```

If the component needs any parameter, it can take as last argument of the constructor a parameter struct. For instance:
```C++
  DebugSource(FIFOBase<IN> &src, emptySourceParams_t &params)
```

If the component has any input event port, it must implement the processEvent method:
```C++
void processEvent(int dstPort,Event &&evt) final
```

Testing for a event defined by the component is done with the `selectors` array. 

Example:
```C++
 if (evt.event_id == selectors[selMessage])
```

If the event is a default event as define in `cg_enums.h` CMSIS stream header then the test can use directly the global ID since this ID will not change for different graphs:
Example:
```C++
 if (evt.event_id == kValue)
```

To process an event, some convenience functions are defined in the CMSIS Stream headers. For instance
```C++
if (evt.wellFormed<float>())
{
                evt.apply<float>(&DebugEvtSink::messageReceived, *this);
}
```

IN this example, the function messageReceived has type:
```C++
void messageReceived(float v)
```

If the node has no output event port but must still be able to send events, it can define an internal event queue and use it to send events. In this case, the constructor of the component must take as argument a pointer to an EventQueue and store it in a member variable. The component can then create events and push them to the queue to send them to other components.

In that case the EventQueue argument is the first after the dataflow FIFO arguments.
It is defined in the Python by using the specific argument `evtQueue` in the `addVariableArg` function as described in the previous section. And the C++ constructor will have an argument named `EventQueue *queue` and the component will store this pointer in a member variable to use it to send events when needed.
