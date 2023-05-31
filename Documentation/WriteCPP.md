## How to write the C++ nodes

This is a continuation of the [Simple graph creation example](Examples/simple/README.md).

The files are in the `Examples/simple` folder.

This document details how to write the C++ wrappers needed to use your code in the graph. It assumes some minimal knowledge about C++ templates and inheritance.

For each node datatype defined in the Python side, we need to provide an implementation on the C++ side.

The C++ class templates that we will define are just wrappers around algorithms. In this example, since the algorithms are very simple, they have been implemented directly in the wrappers. It does not have to be the case for a more complex algorithms. The C++ template are serving the same purposes as the Python definitions : defining the datatype of a node.

* The number of IOs
* Their datatype
* The number of samples consumed or produced on each IO

The C++ template is also providing some entry points to enable the scheduler to do its works :

* Access to the FIFOs
* Running the code

Those C++ templates should thus be very light and that's why we prefer to speak of C++ wrappers rather than C++ objects. The code for the algorithms will generally be outside of those wrappers (and will often be in C).

Those templates are defined in a file `AppNodes.h` included by the scheduler (it is possible to change the name from the Python script). This file must be provided by the user of the CMSIS-Stream framework.

### The C++ wrapper for Source

First, like with Python, we need to define the datatype:

* Number of IOs
* Their type
* The number of samples

It is done through arguments of C++ templates.

```C++
template<typename OUT,int outputSize>
class Source;
```

The previous line is defining a new class template with two arguments:

* A datatype `OUT`
* The number of samples `outputSize`

This template can be used to implement different kind of `Source` classes : with different datatypes or number of samples. We can also (when it makes sense) define a `Source` implementation that can work with any datatype and any number of samples.

You don't need to be knowledgeable in C++ template to start using them in the context of the compute graph. They are just here to define the plumbing.

The only thing to understand is that:

* `Source<X,Y>` is the datatype where the template argument has been replaced by the types `X` and `Y`. 
* `Source<X',Y'>` is a different datatype than `Source<X,Y>` if `X` and `X'` are different types
* `X` and `Y` may be numbers (so a number is considered as a type in this context)

When you have declared a C++ template, you need to implement it. There are two ways to do it:

* You can define a generic implementation for `Source` that will work for any template argument
* And/or you can define specialized implementations for specific types (`Source<X,Y>`).

For the `Source` we have defined a generic implementation.

We need (like in Python case) to inherit from `GenericSource`:

```C++
template<typename OUT,int outputSize>
class Source: GenericSource<OUT,outputSize>
```

Then, like in the Python case, we need to define a constructor. But contrary to the Python case, here we are defining an implementation. The constructor is not defining the IOs. The IOs are coming from the template and its arguments.

```C++
public:
    Source(FIFOBase<OUT> &dst):public GenericSource<OUT,outputSize>(dst){};
```

Our `Source` has only one IO : the output. It needs the FIFO for this output. The first argument, `dst`, of  the `Source` constructor is the FIFO. This FIFO is coming from the generated scheduler. The generated scheduler creates a `Source` object by passing the FIFOs to its constructor.

We also need to initialize the `GenericSource` parent since we are inheriting from it. `GenericSource` constructor is called with the `FIFO` argument `dst`.

The constructor is here doing nothing more than initializing the parent and the implementation is empty `{}`

The implementation of `Source` needs to provide an entry point to be usable from the scheduler. It is the `run` function. As said before, since the algorithm is very simple it has been implemented in `run`. In general, `run` is just calling an external function with the buffers coming from the FIFOs.

```C++
int run() final {
        OUT *b=this->getWriteBuffer();

        printf("Source\n");
        for(int i=0;i<outputSize;i++)
        {
            b[i] = (OUT)i;
        }
        return(0);
    };
```

The first line is the important one:

```C++
OUT *b=this->getWriteBuffer();
```

We get a pointer to be able to write in the output FIFO. This pointer has the datatype OUT coming from the template so can be anything. 

The code in the loop is casting an `int` (the loop index) into the `OUT` datatype. If it is not possible it won't typecheck and build.

```C++
for(int i=0;i<outputSize;i++)
{
    b[i] = (OUT)i;
}
```

So, although we have not provided a specific implementation of the template, this template can only work with specific `OUT` datatypes because of the implementation. It is not a generic implementation.

The return of the function `run` is to inform the scheduler that no error occurred. In synchronous mode, errors (like underflow or overflow) cannot occur due to the scheduling but only because of a broken real time. So any error returned by a node will stop the scheduling.

### The C++ wrapper for the Processing node

It is similar but now we have one input and one output. The template is:

```C++
template<typename IN, int inputSize,
         typename OUT,int outputSize>
class ProcessingNode;
```

In this example, we have decided to implement only a specific version of the processing node. We want to enforce the constraint that the output datatype must be equal to the input datatype and that the number of sample produced must be equal to the number of sample consumed. If it is not the case, it won't type check and the solution won't build.

Remember from the Python definition that this constraint has not been enforced in the Python description of the processing node.

Here is how we implement a specialized version of the template.

First we define the arguments of the template. It is no more generic and we have to give all the arguments:

```C++
class ProcessingNode<IN,inputOutputSize,
                     IN,inputOutputSize>
```

This enforces that the `OUT` datatype is equal to the `IN` datatype since `IN` is used in both arguments.

It also enforces that the input and output sizes are the same since `inputOutputSize` is used in the two arguments for the size.

Since the arguments of the template are still not fully specified and there is some remaining degree of freedom, we need to continue to define some template parameters:

```C++
template<typename IN, int inputOutputSize>
class ProcessingNode<IN,inputOutputSize,
                     IN,inputOutputSize>
```

And finally, like before, we inherit from `GenericNode` using the same template arguments:

```C++
template<typename IN, int inputOutputSize>
class ProcessingNode<IN,inputOutputSize,
                     IN,inputOutputSize>: 
      public GenericNode<IN,inputOutputSize,
                         IN,inputOutputSize>
```

To be compared with the generic implementation:

```C++
template<typename IN, int inputSize, 
         typename OUT, int outputSize>
class ProcessingNode: 
      public GenericNode<IN,inputSize,
                         OUT,outputSize>
```

In a generic implementation, we do not use `<>` after `ProcessingNode` since we do not specify specific values of the template arguments.

It is possible to have several specialization of the same class.

One could also have another specialization like:

```C++
template<int inputOutputSize>
class ProcessingNode<q15_t,inputOutputSize,
                     q15_t,inputOutputSize>: 
      public GenericNode<q15_tIN,inputOutputSize,
                         q15_t,inputOutputSize>
```

Just working `q15_t` datatype

The `run` function of the processing node has access to `getReadBuffer` and `getWriteBuffer` to access to the FIFO buffers.

### The C++ wrapper for the Sink

The definition of the `Sink` should be clear now:

```C++
template<typename IN, int inputSize>
class Sink: public GenericSink<IN, inputSize>
{
public:
    Sink(FIFOBase<IN> &src):GenericSink<IN,inputSize>(src){};
```

## How to call the C++ scheduler

The API to the scheduler is:

```C
extern uint32_t scheduler(int *error);
```

It is a C API that can be used from C code.

In case of error, the function is returning :

* the number of schedule iterations computed since the beginning
* an error code.

It is possible, from the Python script, to add arguments to this API when there is the need to pass additional information to the nodes.

## Source code files

There are only 2 files to compile:

* `generated/scheduler.cpp`
  * This file is generated by Python

* `main.c`

The directory to use for headers are:

* `generated`
* `../../Include/cg/src`
  * (For the `GenericNodes.h` header)

* `.` the current directory

The headers required by the software are:

* `generated/scheduler.h`

  * The is the C API to the scheduler function generated by Python
  
* `AppNodes.h`

  * `AppNodes.h` is where the implementation of the nodes is defined. This file could also just include nodes from a standard library.
  
* `custom.h`

  * This is the first include in the `scheduler.cpp` and this file can contain whatever is needed or just be empty
  
* `GenericNodes.h` 

  * It is coming from the `../../Include/cg/src` folder.
  * It provides the basic definitions needed by the framework like `GenericNode`, `GenericSink`,`GenericSource`, `FIFO` ...
  
  

In summary, the Python is generating `scheduler.cpp` and `scheduler.h` and the developer must provide `AppNodes.h` and `custom.h`

`GenericNodes.h` must be included. To use any additional node from CMSIS-Stream, its header must be included too (In this example no additional node is used beyond the one implemented in `AppNodes.h`).