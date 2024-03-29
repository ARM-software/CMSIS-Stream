/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        Unzip.h
 * Description:  Node to unzip a stream of pair
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
 *
 * Copyright (C) 2021-2023 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _UNZIP_H_
#define _UNZIP_H_ 

namespace arm_cmsis_stream {
/*

Unzip a stream a1 a2 b1 b2 c1 c2 ...
Into 2 streams:
a1 b1 c1 ...
a2 b2 c2 ...

*/
template<typename IN, int inputSize,typename OUT1,int output1Size,typename OUT2,int output2Size>
class Unzip;

template<typename IN, int inputSize,int output1Size,int output2Size>
class Unzip<IN,inputSize,IN,output1Size,IN,output2Size>: public GenericNode12<IN,inputSize,IN,output1Size,IN,output2Size>
{
public:
    Unzip(FIFOBase<IN> &src,FIFOBase<IN> &dst1,FIFOBase<IN> &dst2):
    GenericNode12<IN,inputSize,IN,output1Size,IN,output2Size>(src,dst1,dst2){};

    int prepareForRunning() final
    {
        if (this->willOverflow1() ||
            this->willOverflow2() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    /*
          2*outputSize1 == 2*outSize2 == inputSize
    */
    int run() final
    {
        IN *a=this->getReadBuffer();
        IN *b1=this->getWriteBuffer1();
        IN *b2=this->getWriteBuffer2();
        for(int i = 0; i<output1Size; i++)
        {
           b1[i] =(IN)a[2*i];
           b2[i] =(IN)a[2*i+1];
        }
        return(CG_SUCCESS);
    };

};

} // end namespace
#endif