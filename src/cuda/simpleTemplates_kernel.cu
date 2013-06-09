
#ifndef _TEMPLATE_KERNEL_H_
#define _TEMPLATE_KERNEL_H_

#include <stdio.h>



////////////////////////////////////////////////////////////////////////////////
//! Simple test kernel for device functionality
//! @param g_idata  input data in global memory
//! @param g_odata  output data in global memory
////////////////////////////////////////////////////////////////////////////////
template<class T> __global__ void testKernel( T* g_idata, T* g_odata,long size,T mss_val,int has_mss_val) 
{
  // access thread id
  const unsigned int tid = threadIdx.x+ blockDim.x*blockIdx.x;
	if(tid<size )
		{
		if(has_mss_val)
			{
			if( g_idata[tid]==mss_val || g_odata[tid]==mss_val)
				g_odata[tid]=mss_val;
			else 
				g_odata[tid]=g_idata[tid]+g_odata[tid];
			}
		else
			g_odata[tid] = g_idata[tid]+g_odata[tid];
		}
}

#endif // #ifndef _TEMPLATE_KERNEL_H_
