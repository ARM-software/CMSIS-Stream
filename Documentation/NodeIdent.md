# Node identification

When node identification is enabled, the nodes can be identified by a number.

A function is generated to get a pointer to the node with a given identification number.

By default, the function generated is:

`extern CStreamNode* get_scheduler_node(int32_t nodeID);`

and `CStreamNode` is a `C` struct defined in the generated file `cstream_node.h`.

Nodes are C++ classes that can use multiple inheritance to organize the features into interfaces.

It is impossible to use those classes directly from C. And, from C++, since RTTI is often disabled in embedded applications, it is also very difficult.

To solve this issue, a C struct is generated and enable access to one or several interfaces from C.

By default, only some subset of the `StreamNode` interface is made accessible to the C world through the C structure `CStreamNode`.

To use this structure, you can do something like:

```C
CStreamNode *n = get_scheduler_node(NODEID);
if ((!n) || (!(n->obj)))
{
    // Error the node was not found
}
if (n->stream_intf)
{
    // The node implements the stream
    // interface
    int nodeID = n.stream_intf->nodeID(n->obj);
}
```

If `stream_intf` is not `NULL` then it means this node implements the stream interface.

You could expose several interfaces to the C world, and the nodes may not have to implement all of them. 

The `CStreamNode` objects are created with a C++ template defined in `IdentifiedNode.h` and that is used by the generated scheduler.

If you need to expose other interfaces, or several different interfaces you can create your own struct and your own C++ template.

The code generation can be customized with following configuration options:

- `config.cnodeAPI` : default value `"cstream_node.h"`

   * Use to define the header to include for the C interfaces

- `config.cnodeTemplate` : default value `"IdentifiedNode.h"`

   * Use to define the header to include for the C++ template

- `config.cNodeStruct` : default value `"CStreamNode"`

   * The name of the C struct to be returned by the configuration function

- `config.cNodeStructCreation` : default value `"createStreamNode"`

   * The name of the C++ function template used to generate the C struct from a given C++ type (a different function for different C++ classes)
   
When building the graph, if the node identification is enabled, you can generate a json file using:

```python
scheduling.genJsonIdentification(".",conf)
```

This file contains a mapping from node names to their identification numbers.
This file could be used if you need to identify nodes by their names.
