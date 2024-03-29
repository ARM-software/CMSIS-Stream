/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        ICFFT.h
 * Description:  Node for CMSIS-DSP icfft
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
#ifndef _ICFFT_H_
#define _ICFFT_H_ 

namespace arm_cmsis_stream {

template<typename IN, int inputSize,typename OUT,int outputSize>
class ICFFT;

/*

The CMSIS-DSP ICFFT F32

*/
template<int inputSize>
class ICFFT<float32_t,inputSize,float32_t,inputSize>: public GenericNode<float32_t,inputSize,float32_t,inputSize>
{
public:
    ICFFT(FIFOBase<float32_t> &src,FIFOBase<float32_t> &dst):GenericNode<float32_t,inputSize,float32_t,inputSize>(src,dst){
         status=arm_cfft_init_f32(&sifft,inputSize>>1);
    };

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        if (status!=ARM_MATH_SUCCESS)
        {
            return(CG_INIT_FAILURE);
        }
        float32_t *a=this->getReadBuffer();
        float32_t *b=this->getWriteBuffer();
        memcpy((void*)b,(void*)a,inputSize*sizeof(float32_t));
        arm_cfft_f32(&sifft,b,1,1);
        return(CG_SUCCESS);
    };

    arm_cfft_instance_f32 sifft;
    arm_status status;

};

#if defined(ARM_FLOAT16_SUPPORTED)
/*

The CMSIS-DSP ICFFT F32

*/
template<int inputSize>
class ICFFT<float16_t,inputSize,float16_t,inputSize>: public GenericNode<float16_t,inputSize,float16_t,inputSize>
{
public:
    ICFFT(FIFOBase<float16_t> &src,FIFOBase<float16_t> &dst):GenericNode<float16_t,inputSize,float16_t,inputSize>(src,dst){
         status=arm_cfft_init_f16(&sifft,inputSize>>1);
    };

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        if (status!=ARM_MATH_SUCCESS)
        {
            return(CG_INIT_FAILURE);
        }
        float16_t *a=this->getReadBuffer();
        float16_t *b=this->getWriteBuffer();
        memcpy((void*)b,(void*)a,inputSize*sizeof(float16_t));
        arm_cfft_f16(&sifft,b,1,1);
        return(CG_SUCCESS);
    };

    arm_cfft_instance_f16 sifft;
    arm_status status;

};
#endif

/*

The CMSIS-DSP ICFFT Q15

*/
template<int inputSize>
class ICFFT<q15_t,inputSize,q15_t,inputSize>: public GenericNode<q15_t,inputSize,q15_t,inputSize>
{
public:
    ICFFT(FIFOBase<q15_t> &src,FIFOBase<q15_t> &dst):GenericNode<q15_t,inputSize,q15_t,inputSize>(src,dst){
         status=arm_cfft_init_q15(&sifft,inputSize>>1);
    };

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    int run() final
    {
        if (status!=ARM_MATH_SUCCESS)
        {
            return(CG_INIT_FAILURE);
        }
        q15_t *a=this->getReadBuffer();
        q15_t *b=this->getWriteBuffer();
        memcpy((void*)b,(void*)a,inputSize*sizeof(q15_t));
        arm_cfft_q15(&sifft,b,1,1);
        return(CG_SUCCESS);
    };

    arm_cfft_instance_q15 sifft;
    arm_status status;

};

} // end namespace
#endif