import re
import subprocess
import shutil
import os.path

shutil.copyfile(os.path.join("..","Include","cg","src","GenericNodes.h"), 
                os.path.join("cmsis_stream","cg","scheduler","templates","GenericNodes.h"))

shutil.copyfile(os.path.join("..","Include","cg","src","cg_status.h"), 
                os.path.join("cmsis_stream","cg","scheduler","templates","cg_status.h"))
