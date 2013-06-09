/*
* Copyright 1993-2006 NVIDIA Corporation.  All rights reserved.
*
* NOTICE TO USER:
*
* This source code is subject to NVIDIA ownership rights under U.S. and
* international Copyright laws.
*
* NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
* CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
* IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
* REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
* OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
* OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
* OR PERFORMANCE OF THIS SOURCE CODE.
*
* U.S. Government End Users.  This source code is a "commercial item" as
* that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting  of
* "commercial computer software" and "commercial computer software
* documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995)
* and is provided to the U.S. Government only as a commercial end item.
* Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
* 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
* source code with only those rights set forth herein.
*/

/* This sample is a templatized version of the template project.
* It also shows how to correctly templatize dynamically allocated shared
* memory arrays.
* Host code.
*/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cuda.h>

// includes, kernels
#include "simpleTemplates_kernel.cu"
#define BLOCKSIZE 256
template<class T> T*
nco_cuda_var_add /* [fnc] Add first operand to second operand */
( /* I [enm] netCDF type of operands */
 const long size, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 T mss_val, /* I [flg] Value of missing value */
 T* op1, /* I [val] Values of first operand */
 T* op2) /* I/O [val] Values of second operand on input, values of sum on output */
{
  /* Purpose: Add value of first operand to value of second operand
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does _not_ increment tally counter
     nco_var_add_tll_ncra() does increment tally counter */
  
  /* Addition is currently defined as op2:=op1+op2 */
 

  T* op_in_D, *op_out_D;GA
  cudaMalloc((void**)&op_in_D, sizeof(T)*size);
  cudaMalloc((void**)&op_out_D,sizeof(T)*size);
  
  cudaMemcpy(op_in_D, op1, sizeof(T)*size,cudaMemcpyHostToDevice);
  cudaMemcpy(op_out_D, op2, sizeof(T)*size,cudaMemcpyHostToDevice);

  uint gridsize=size%BLOCKSIZE ==0?size/BLOCKSIZE :size/BLOCKSIZE +1;
	uint blocksize=BLOCKSIZE;
	dim3 dimBlock(blocksize);
	dim3 dimGrid(gridsize);

  testKernel<T><<<gridsize,blocksize>>>(op_in_D, op_out_D, size, mss_val, has_mss_val);
  cudaMemcpy(op2, op_out_D, sizeof(T)*size,cudaMemcpyDeviceToHost);

  return op2;
    
} /* end nco_var_add() */
