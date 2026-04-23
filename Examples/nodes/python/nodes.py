###########################################
# Project:      CMSIS Stream Library
# Title:        nodes.py
# Description:  Nodes for the examples. Don't use in a production environment, 
#               they are not optimized and not fully tested. They are only here to show how to 
#               create new nodes and use them in a graph.
# 
# 
# Target Processor: Cortex-M and Cortex-A cores
# -------------------------------------------------------------------- */
# 
# Copyright (C) 2021-202320266 ARM Limited or its affiliates. All rights reserved.
# 
# SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
############################################
"""Standard nodes available to describe a network in addition to the generic nodes"""

from cmsis_stream.cg.scheduler import *
import yaml

#from .node import GenericNode,GenericToManyNode,GenericSource,GenericSink, joinit
from copy import deepcopy

floatType=CType(F32)

class Unzip(GenericNode):

    def __init__(self,name,theType,length):
        GenericNode.__init__(self,name)
        self._length = length 
        self.addInput("i",theType,2*length)
        self.addOutput("o1",theType,length)
        self.addOutput("o2",theType,length)
    
    @property
    def typeName(self):
        return "Unzip"

class Zip(GenericNode):

    def __init__(self,name,theType,length):
        GenericNode.__init__(self,name)
        self._length = length 
        self.addInput("i1",theType,length)
        self.addInput("i2",theType,length)
        self.addOutput("o",theType,length)
    
    @property
    def typeName(self):
        return "Zip"



class CFFT(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)

        self.addInput("i",theType,2*inLength)
        self.addOutput("o",theType,2*inLength)

    @property
    def typeName(self):
        return "CFFT"

class ICFFT(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)

        self.addInput("i",theType,2*inLength)
        self.addOutput("o",theType,2*inLength)

    @property
    def typeName(self):
        return "ICFFT"

class ToComplex(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)

        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,2*inLength)

    @property
    def typeName(self):
        return "ToComplex"

class ToReal(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)

        self.addInput("i",theType,2*inLength)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "ToReal"


class NullSink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        return "NullSink"

class InterleavedStereoToMono(GenericNode):
    def __init__(self,name,theType,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,2*outLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "InterleavedStereoToMono"


class MFCC(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)

        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "MFCC"


#############################
#
# Host only Nodes
#

class FileSource(GenericSource):
    def __init__(self,name,inLength):
        GenericSource.__init__(self,name)
        floatType=CType(F32)
        self.addOutput("o",floatType,inLength)

    @property
    def typeName(self):
        return "FileSource"

class FileSink(GenericSink):
    def __init__(self,name,inLength):
        GenericSink.__init__(self,name)
        floatType=CType(F32)
        self.addInput("i",floatType,inLength)

    @property
    def typeName(self):
        return "FileSink"

#############################
#
# Python and host only Nodes
#


class WavSource(GenericSource):
    def __init__(self,name,inLength):
        GenericSource.__init__(self,name)
        q15Type=CType(Q15)
        self.addOutput("o",q15Type,inLength)

    @property
    def typeName(self):
        return "WavSource"

class WavSink(GenericSink):
    def __init__(self,name,inLength):
        GenericSink.__init__(self,name)
        q15Type=CType(Q15)
        self.addInput("i",q15Type,inLength)

    @property
    def typeName(self):
        return "WavSink"

class NumpySink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

    @property
    def typeName(self):
        return "NumpySink"

##################
#
# Node to communicates with a VHT block running in Modelica
#
# It is requiring the VHT Modelica extensions which can be found
# in the VHTSystemModeling repository on ArmSoftware GitHub

class VHTSource(GenericSource):
    def __init__(self,name,inLength,theID):
        GenericSource.__init__(self,name)

        self.addOutput("o",CType(Q15),inLength)
        self.addLiteralArg(theID)


    @property
    def typeName(self):
        return "VHTSource"

class VHTSink(GenericSink):
    def __init__(self,name,inLength,theID):
        GenericSink.__init__(self,name)

        self.addInput("i",CType(Q15),inLength)
        self.addLiteralArg(theID)

    @property
    def typeName(self):
        return "VHTSink"
        
###########################################
# Project:      CMSIS Stream Library
# Title:        sds_nodes.py
# Description:  Node for SDS framework
# 
# 
# Target Processor: Cortex-M and Cortex-A cores
# -------------------------------------------------------------------- */
# 
# Copyright (C) 2021-2023 ARM Limited or its affiliates. All rights reserved.
# 
# SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
############################################

class DelegateFunctionAndDelegateDataNeeded(Exception):
    pass

def _getDataTypeSizeInBytes(data_type):
    match data_type:
        case "int8_t":
            return 1
        case "uint8_t":
            return 1
        case "int16_t":
            return 2
        case "uint16_t":
            return 2
        case "int32_t":
            return 4
        case "uint32_t":
            return 4
        case "float":
            return 4
        case "double":
            return 8
        case _:
            print(f"Unknown data type: {data_type}\n")
            return None

def _getSensorSampleSize(file_name):
    if ".yml" in file_name:
        with open(file_name, "r") as meta_file:
             meta_data = yaml.load(meta_file, Loader=yaml.FullLoader)["sds"]
    else: 
        with open(file_name, "rb") as meta_file:
             meta_data = yaml.load(meta_file, Loader=yaml.FullLoader)["sds"]
    data_name = meta_data["name"]
    struct_name = data_name + "Type"
    data_desc = meta_data["content"]
    data_freq = meta_data["frequency"]
    sampleSize = 0
    dt = []
    for c in meta_data["content"]:
        entryDataType = c['type'] 
        dt.append(entryDataType)
        sampleSize += _getDataTypeSizeInBytes(entryDataType)
    
    sensorType=CType(UINT8)

    return(sampleSize)

class SDSSensor(GenericSource):
    def __init__(self,name,nb_samples,
        sds_yml_file=None,
        sds_connection=None,
        drift_delegate=None,
        drift_delegate_data=None):
        GenericSource.__init__(self,name)
        self.sample_size = _getSensorSampleSize(sds_yml_file)
        self.addOutput("o",CType(UINT8),self.sample_size*nb_samples)
        
        self.addVariableArg(sds_connection)

        if (drift_delegate is not None) and (drift_delegate_data is not None):
           self.addVariableArg(drift_delegate)
           self.addVariableArg(drift_delegate_data)
        elif (drift_delegate is not None) or (drift_delegate_data is not None):
            raise DelegateFunctionAndDelegateDataNeeded


    @property
    def typeName(self):
        return ("SDSSensor")

class SDSRecorder(GenericSink):
    def __init__(self,name,nb_samples,
                 the_type = None,
                 sds_yml_file=None,
                 sds_connection=None):
        GenericSink.__init__(self,name)

        if the_type is not None:
            nb_input_samples = nb_samples
            input_type = the_type
        else:
            sample_size = _getSensorSampleSize(sds_yml_file)
            nb_input_samples = sample_size*nb_samples
            input_type = CType(UINT8)

        self.addInput("i",input_type,nb_input_samples)
        
        self.addVariableArg(sds_connection)

    @property
    def typeName(self):
        return ("SDSRecorder")
