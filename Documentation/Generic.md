# Description of the nodes

The generic and function nodes are the basic nodes that you use to create other kind of nodes in the graph. 

There are several generic classes provided by the framework to be used to create new nodes.

There are also classes provided to define the datatype of the samples processed by the nodes.

- [Description of the nodes](#description-of-the-nodes)
  - [1 Overview](#1-overview)
    - [1.1 Basic generic nodes](#11-basic-generic-nodes)
    - [1.2 Complex generic nodes](#12-complex-generic-nodes)
      - [1.2.1 Many-to-many cases:](#121-many-to-many-cases)
      - [1.2.2 Function nodes](#122-function-nodes)
  - [2 Details about basic generic nodes](#2-details-about-basic-generic-nodes)
    - [2.1 Methods](#21-methods)
    - [2.2 Datatypes](#22-datatypes)
      - [2.2.1 CType](#221-ctype)
      - [2.2.2 CStructType](#222-cstructtype)
      - [2.2.3 PythonClassType](#223-pythonclasstype)
  - [3 Details about many-to-many nodes](#3-details-about-many-to-many-nodes)
    - [3.1 GenericToManyNode](#31-generictomanynode)
    - [3.2 GenericFromManyNode](#32-genericfrommanynode)
    - [3.3 GenericManyToManyNode](#33-genericmanytomanynode)
  - [4 Function and constant nodes](#4-function-and-constant-nodes)
    - [`GenericFunction`](#genericfunction)

## 1 Overview

### 1.1 Basic generic nodes

To create a new kind of node, you generally inherit from one of those classes. Those are the simplest and most used:

* `GenericSource`
* `GenericNode`
* `GenericSink`

They are defined in `cmsis_stream.cg.scheduler`.

### 1.2 Complex generic nodes

#### 1.2.1 Many-to-many cases:

* `GenericToManyNode`
* `GenericFromManyNode`
* `GenericManyToManyNode`

#### 1.2.2 Function nodes

And finally, there are other classes that can be used to create new nodes from functions. A function has no state and a C++ wrapper is not required. In this case, the tool is generating code for calling the function directly rather than using a C++ wrapper.

* `Unary` (unary operators like `negate`, `inverse` ...)
* `Binary` (binary operators like `add`, `mul` ...)
* `GenericFunction` to map any function

## 2 Details about basic generic nodes

When you define a new kind of node, it must inherit from one of those classes. Those classes are providing the methods `addInput` and/or `addOutput` to define new inputs / outputs.

A new kind of node is generally defined as:

```python
class ProcessingNode(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "ProcessingNode"
```

The method `typeName` from the parent class must be overridden and provide the name of the `C++` wrapper to be used for this node.

The object constructor is defining the inputs / outputs : number of samples and datatype.

The object constructor is also defining the name used to identity this node in the generated code (so it must be a valid C variable name).

`GenericSink` is only providing the `addInput` function.

`GenericSource` is only providing the `addOutput` function

`GenericNode` is providing both.

You can use each function as much as you want to create several inputs and / or several outputs for a node.

The `Generic` node constructors are accepting another named argument : `identified` and by default it is `True`. This value is used when the code generation is using the `nodeIdentification` mode.

This value can also be changed on the node after creation.

When this value is `True`, the node will be accessible from outside the scheduler using the provided scheduler API in the `C` generated header.

See the [simple](../Examples/simple/README.md) example for more explanation about how to define a new node.

### 2.1 Methods

The constructor of the node is using the `addInput` and/or `addOutput` to define new IOs.

```python
def addInput(self,name,theType,theLength):
```

* `name` is the name of the input. It will becomes a property of the Python object so it must not conflict with existing properties. If `name` is, for instance, `"i"` then it can be accessed with `node.i` in the code
* `theType` is the datatype of the input. It must inherit from `CGStaticType` (see below for more details about defining the types)
* `theLength` is the amount of **samples** consumed by this input at each execution of the node in synchronous mode. In asynchronous mode it would correspond to the average case.

```python
def addOutput(self,name,theType,theLength):
```

* `name` is the name of the output. It will becomes a property of the Python object so it must not conflict with existing properties. If `name` is, for instance, `"o"` then it can be accessed with `node.o` in the code
* `theType` is the datatype of the output. It must inherit from `CGStaticType` (see below for more details about defining the types)
* `theLength` is the amount of **samples** produced by this output at each execution of the node in synchronous mode. In asynchronous mode it would correspond to the average case.

```python
@property
def typeName(self):
    return "ProcessingNode"
```

This method defines the name of the C++ class implementing the wrapper for this node.

A FIFO constraint can come from an IO. Some software components may allocate the memory for the buffers they use for their inputs or outputs. Or they may have to use a specific memory buffer already defined in the system.

It is possible to define those constraint on an input or on an output. The constraint will be inherited by the FIFO connected to this input or output. A buffer constraint can be defined with:

```python
def setBufferConstraint(self,name=None,mustBeArray=True,assignedByNode=True,canBeShared=True)
```

For instance you may define a constraint on an output by writing:

```python
self.o.setBufferConstraint(name="Test",mustBeArray=True,assignedByNode=False)
```

Any FIFO connected to this `o` output will have to use the `Test` buffer and will have to be scheduled in such a way that this buffer is used as an array.

The meaning of the arguments is:

* `name` : C code to access the buffer. It can be the name of a global variable. of a variable passed as argument of the scheduler, a function call returning an address ...
* `mustBeArray` : True if the buffer can only be used as an array (and not as a real FIFO)
* `assignedByNode` :True if buffer is allocated by the node during the node creation (in the node constructors). In that case, the `name` argument is useless. The FIFO will be initialized with a `nullptr` buffer when creating the scheduler. Then the node will set the FIFO buffer during its construction
*  `canBeShared` True if the buffer may be shared with other FIFOs. The logic to decide if the buffer can really be shared is complex. It depends on the scheduling, the compatibility of this buffer with other ones etc ...

### 2.2 Datatypes

Datatypes for the IOs are inheriting from `CGStaticType`.

Currently there are 3 classes defined in the project:

* `CType` for the standard CMSIS-DSP types like `q15_t`, `float32_t` ...
* `CStructType` for a C struct
* `PythonClassType` to create structured datatype for the Python scheduler

#### 2.2.1 CType

You create such a type with `CType(id)` where `id` is one of the constant coming from the Python wrapper:

* F64
* F32
* F16
* Q31
* Q15
* Q7
* UINT32
* UINT16
* UINT8
* SINT32
* SINT16
* SINT8

For instance, to define a `float` type for an IO you can use `CType(F32)`

If you want to use the CMSIS-DSP naming (because you are using CMSIS-DSP), you can pass the option `cmsis_dsp=True`:

```python
CType(F32,cmsis_dsp=True)
```

The datatype will be `float32_t` in the generated code and as consequence you'll need to include the `"arm_math.h"` header.

#### 2.2.2 CStructType

The constructor has the following definition

```python
def __init__(self,name,size_in_bytes): 
```

* `name` is the name of the C struct
* `size_in_bytes` is the size of the C struct. It should take into account padding. It is used when the compute graph memory optimization is used since size of the datatype is needed. 

#### 2.2.3 PythonClassType

```python
def __init__(self,python_name)
```

In Python, there is no `struct`. This datatype is mapped to an object. Object have reference type. Compute graph FIFOs are assuming a value type semantic.

As consequence, in Python side you should never copy those structs since it would copy the reference. You should instead copy the members of the struct and they should be scalar values.

## 3 Details about many-to-many nodes

All the IOs of a node must be described in its C++ template. When a node is heterogenous (IOs of different datatypes and/or length) the listing of all IOs in the template arguments cannot be avoided.

But when all the inputs or all the outputs have the same datatype and length, it can be very annoying to have to define a long list of template arguments.

Also, we'd like to have the same implementation but working with as many inputs or output of the same kind.

With the normal mechanism, two nodes differing by the number of inputs or outputs have a different datatype : different C++ template.

To make it easier to implement some regular and homogeneous nodes (like a Duplicate node copying its input to its outputs), some new classes have been introduced:

### 3.1 GenericToManyNode

`GenericToManyNode` is very similar to `GenericNode` but instead of providing a function `addOutput`, it is providing:

```python
def addManyOutput(self,theType,theLength,nb):
```

It will define `nb` outputs with the same type `theType` and the same length `theLength`. The node is said homogenous (at least for the outputs) since they are all of the same nature.

Note that you cannot add other outputs of different nature (datatype or length). The function `addOutput` is not available in this context.

The name of those outputs is defined by the node, and different nodes may have a different naming strategy. The naming can be accessed with the function provided by the node:

```python
def outputNameFromIndex(self,i):
```

Let's assume `p` is a node with many outputs. To connect the second output, one could do:

```python
g.connect(p[p.outputNameFromIndex(1)],node.i)
```

The method `outputNameFromIndex` can be customized by a node. It should always return names that are in alphabetical order so that the index used in this method is also the index to use in the C code.

By default the names generated are `oa`, `ob`, ... It means you should not use more than 26 outputs with this naming ...

### 3.2 GenericFromManyNode

It is similar to the previous case but with many inputs. The `addInput` function is replaced with:

```python
def addManyInput(self,theType,theLength,nb):
```

The function to get the input names if:

```python
def inputNameFromIndex(self,i):
```

### 3.3 GenericManyToManyNode

The node is providing the two following functions:

```python
def addManyInput(self,theType,theLength,nb):
```

```python
def addManyOutput(self,theType,theLength,nb):
```

## 4 Function and constant nodes

A Compute graph C++ wrapper is useful when the software components you use have a state that needs to be initialized in the C++ constructor, and preserved between successive calls to the `run` method of the wrapper.

Most CMSIS-DSP functions have no state. The compute graph framework is providing some ways to easily use functions in the graph without having to write a wrapper.

This feature is relying on the nodes:

* `Unary`
  * To use an unary operator like `negate`, `inverse` ...
* `Binary`
  * To use a binary operator like `add`, `mul` ...
* `Constant`
  * Special node to be used **only** with function nodes when some arguments cannot be connected to a FIFO. For instance, with `arm_scale_f32` the scaling factor is a scalar value and a FIFO cannot be connected to this argument. The function is a binary operator but between a stream and a scalar.


All of this is explained in detail in the [simple example with CMSIS-DSP](../Examples/simpledsp/README.md).

The API to create the `Unary`,`Binary` nodes are taking some arguments to customize the name of the inputs and output.

* `input_name` by default it is `"i"`
* `output_name` by default it is `"o"`
* `input_names` (when several inputs) and by default it is `["ia","ib"]`

In case of binary mode, the input names are sorted in alphabetical order. The first argument to the function is the first input in alphabetical order.

### `GenericFunction`

With the `GenericFunction` it is possible to map most pure functions (with no state) to the graph without having to write a wrapper.

Let's see how to use a C function will following prototype:

```C
void myfunc(float *       i0,
            int           nb_samples0,
            custom_type_t testVar,
            float *       i1,
            int           nb_bytes1,
            float *       o,
            int           nb_samples2,
            int           someInt,
            const char*   someStr);
```

This is a complex function with lots of arguments to demonstrate all the features of `GenericFunction`.

The function corresponds to a node with two inputs and one outputs:

* Input buffers `i0` and `i1`
* Output buffer `o`

The  size of each buffer is either given as a number of samples (for `i0` and `o`) or a number of bytes for `i1`.

In addition to those buffers, the function is taking 3 additional arguments:

* An argument `testVar` with a custom type
* An int `someInt`
* A string `someStr`

As with any other nodes, we define the input and outputs and the additional arguments of the node in the graph.

```python
class MyFunction(GenericFunction):
    def __init__(self, length_a
                     , length_b
                     , length_c):
        GenericFunction.__init__(self,"myfunc",
           THIS SECTION IS DESCRIBED BELOW
            )


        self.addInput("ia",CType(F32),length_a)
        self.addInput("ib",CType(F32),length_b)
        self.addOutput("o",CType(Q15),length_c)

        self.addVariableArg("testVar")
        self.addLiteralArg(4,"5")

```

Since IOs are always in alphabetical order, `"ia"` corresponds to `i0`, `ib` to `i1`.

We define 3 additional arguments in the same order:

* A variable `"testVar`"
* An int with value '4' and a string with value "5"

Now we need to define how those node arguments are mapped to the C function.

For this, we pass an array to the `GenericFunction` that describes how the node IO and arguments are mapped to the C arguments.

```C
GenericFunction.__init__(self,"myfunc",
            ["ia"
            ,ArgLength("ia")
            , 0
            ,"ib"
            ,ArgLength("ib",sample_unit=False)
            ,"o"
            ,ArgLength("o")
            ,1,2
            ])
```

The array contains:

* The name of a port (`"ia"`,`"ib"`,`"o"`). It will be mapped to a FIFO. A pointer or a constant when the FIFO argument is connected to a constant node.
* `ArgLength` is the length of the FIFO either in number of samples or number of bytes (`sample_unit=false`)
* An int is the index of an additional argument in the order they have been created with `addVariableArg` and `addLiteralArg`
  * `testVar` is the additional argument created first and has index `0`
  * The int with value `4` is the second argument and has index `1`
  * And finally the string has index `2`
  * The int and the string are mapped at the end of the function since the list is ending with `1,2`

The above definition will generate the following function call:

```C
float* i0;
float* i1;
q15_t* o2;
i0=fifo0.getReadBuffer(7);
i1=fifo1.getReadBuffer(7);
o2=fifo2.getWriteBuffer(5);
myfunc(i0,7,testVar,i1,sizeof(float)*7,o2,5,4,"5");
```

A pure function cannot generate an error. If you need error handling, you need a C++ wrapper.

In this example, the int `someInt` has been introduced as a separate argument. It won't be visible in the graphical representation and cannot be connected to a `Constant` node.

Another way to introduce this argument would be to define a FIFO of type `int` and connect it to a `Constant` node in the graph. 

The generated code would be:

```C
float* i0;
float* i1;
q15_t* o3;
i0=fifo0.getReadBuffer(7);
i1=fifo1.getReadBuffer(7);
o3=fifo2.getWriteBuffer(5);
myfunc(i0,7,testVar,i1,sizeof(float)*7,o3,5,SomeConst,"5");
cgStaticError = 0;
```

The only difference is that the output is now named `o3` instead of `o2` because the virtual FIFO has been counted in the list of FIFO. But since it is connected to a `Constant` node, the FIFO has not been generated in the code. Instead we have the constant `SomeConst` replacing the `4` value at the end.
