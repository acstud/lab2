// Copyright 2018 Delft University of Technology
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>

#include "../baseline/water.hpp"

#include "water_cuda.hpp"
#include "cuda_check.hpp"

// An example CUDA kernel
__global__ void mul(float *a, float *b, float *c, int n) {
  for (int i = 0; i < n; i++) {
    c[i] = a[i] * b[i];
  }
}

std::shared_ptr<Image> runWaterEffectCUDA(const Image *src, const WaterEffectOptions *options) {
  /* REPLACE THIS CODE WITH YOUR OWN WATER EFFECT PIPELINE */

  std::cout << "Hello world compiled by the NVCC compiler!" << std::endl;

  float *a, *b, *c;

  // Allocate some CUDA unified memory
  checkCudaErrors(cudaMallocManaged(&a, 4 * sizeof(float)));
  checkCudaErrors(cudaMallocManaged(&b, 4 * sizeof(float)));
  checkCudaErrors(cudaMallocManaged(&c, 4 * sizeof(float)));

  // Fill some dummy data
  for (int i = 0; i < 4; i++) {
    a[i] = 1.337f;
    b[i] = (float) i;
    c[i] = 0.0f;
  }

  // Start a kernel
  mul<<<1,1>>> (a, b, c, 4);
  checkCudaErrors(cudaGetLastError());

  // Wait for completion
  checkCudaErrors(cudaDeviceSynchronize());

  // Print the output
  for (int i = 0; i < 4; i++) {
    std::cout << a[i] << " * " << b[i] << " = " << c[i] << std::endl;
  }

  // Free the allocations
  checkCudaErrors(cudaFree(a));
  checkCudaErrors(cudaFree(b));
  checkCudaErrors(cudaFree(c));

  return nullptr;
  /* REPLACE THIS CODE WITH YOUR OWN WATER EFFECT PIPELINE */
}