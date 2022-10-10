#ifndef CUDA_CHECK
#define CUDA_CHECK

#include <cuda_runtime.h>
#include <iostream>
template <typename T>
inline void check(T result, char const* const func, const char* const file, int const line)
{
    if (result)
    {
        fprintf(stderr, "CUDA error at %s:%d code=%d(%s) \"%s\" \n", file, line, static_cast<unsigned int>(result), cudaGetErrorName(result), func);
        cudaDeviceReset();
        exit(EXIT_FAILURE);
    }
}

#define checkCudaErrors(val) check((val), #val, __FILE__, __LINE__)

#endif