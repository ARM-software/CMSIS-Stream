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
from ..scheduler import *
import yaml

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
