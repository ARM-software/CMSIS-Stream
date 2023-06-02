import sys 
from cmsis_stream.cg.scheduler import *

def print_usage():
    print("Usage: cmsis-stream create directory")

if len(sys.argv)==3:
   if sys.argv[1] == "create":
      project_name = sys.argv[2]
      print(f"Create project: {project_name}")
      createEmptyProject(project_name)
      print("""
Project created with scheduling iterations set to 1.
When run, the scheduling will stop after one iteration.
You can disable this setting in graph.py.""")
   else:
     print_usage()

else:
   print_usage()

exit(0)