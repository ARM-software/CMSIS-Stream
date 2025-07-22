# Python API 

When you describe a graph and generate a scheduler there are 4 mandatory steps and one optional one:

* Describing the nodes
* Adding the nodes to the graph
* Computing the schedule
* Generating the C++ and/or Python scheduler implementation
* Optional : Generating a graphviz `.dot` file with the graphical representation of the graph

1. ## [Description of new nodes](Generic.md)

2. ## [Adding nodes to the graph](Graph.md)

3. ## [Schedule computation and generation](SchedOptions.md)

4. ## [SDS Nodes (experimental)](PythonSDS.md)

5. ## Generation

   1. #### [C++ Code generation](CCodeGen.md)

   2. #### [Python code generation (deprecated)](PythonGen.md)

   3. #### [Graphviz generation](GraphvizGen.md)

   4. #### [Common options for all generators](CodegenOptions.md)









