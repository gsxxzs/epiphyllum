#include <cuda_runtime.h>

__global__ void incrementArray(int* arr, int size);

__global__ void matrixMuilt(int* input_1, int* input_2, int* output, int m, int n, int t);

__global__ void conv2d(int* input, int* kernel, int* output, dim3 input_size, dim3 kernel_size);

__global__ void addPadding2d(int* input, int* output, dim3 input_size, dim3 padding);