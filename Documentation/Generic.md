# Description of the nodes

The generic and function nodes are the basic nodes that you use to create other kind of nodes in the graph. 

There are several generic classes provided by the framework to be used to create new nodes.

There are also classes provided to define the datatype of the samples processed by the nodes.

* ## [Overview](#1-overview)

  * ### [Basic generic nodes](#1.1-basic-generic-nodes)

  * ### [Complex generic nodes](#1.2-complex-generic-nodes)

    * #### [Many-to-many cases](#1.2.1-many-to-many-cases)

    * #### [Function nodes](#1.2.2-function-nodes)

* ## [Details about basic generic nodes](#2-details-about-basic-generic-nodes)

  * ### [Methods](#2.1-methods)

  * ### [Datatypes](#2.2-datatypes)

    * #### [CTypes](#2.2.1-ctype)

    * #### [CStructType](#2.2.2-cstructtype)

    * #### [PythonClassType](#2.2.3-pythonclasstype)

* ## [Details about many-to-many nodes](#3-details-about-many-to-many-nodes)

  * ### [GenericToManyNode](#3.1-generictomanynode)

  * ### [GenericFromManyNode](#3.2-genericfrommanynode)

  * ### [GenericManyToManyNode](#3.3-genericmanytomanynode)

* ## [Functions and constant nodes](#4-functions-and-constants-nodes)

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

And finally, there are 3 other classes that can be used to create new nodes from functions. A function has no state and a C++ wrapper is not required. In this case, the tool is generating code for calling the function directly rather than using a C++ wrapper.

* `Unary` (unary operators like `negate`, `inverse` ...)
* `Binary` (binary operators like `add`, `mul` ...)
* `Dsp` (Some CMSIS-DSP function either binary or unary)

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

* `Dsp`
  * Should detect if the CMSIS-DSP operator is unary or binary and use the datatype to compute the name of the function. In practice, only a subset of CMSIS-DSP function is supported so you should use `Unary` or `Binary` nodes

* `Constant`
  * Special node to be used **only** with function nodes when some arguments cannot be connected to a FIFO. For instance, with `arm_scale_f32` the scaling factor is a scalar value and a FIFO cannot be connected to this argument. The function is a binary operator but between a stream and a scalar.


All of this is explained in detail in the [simple example with CMSIS-DSP](../Examples/simpledsp/README.md).

The API to create the `Unary`,`Binary` and `Dsp` nodes are taking some arguments to customize the name of the inputs and output.

* `input_name` by default it is `"i"`
* `output_name` by default it is `"o"`
* `input_names` (when several inputs) and by default it is `["ia","ib"]`

In case of binary mode, the input names are sorted in alphabetical order. The first argument to the function is the first input in alphabetical order.
