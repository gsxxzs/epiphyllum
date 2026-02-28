#include "kernel.h"
#include "stdio.h"

#define FLOAT4(pointer) (reinterpret_cast<float4*>(&(pointer))[0])

__device__ int divUp(int a, int b){
  return (a + b - 1) / b;
}

__global__ void incrementArray(int* arr, int size) {
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index < size) {
    arr[index] += 1;
  }
}

__global__ void matrixMuilt(int* input_1, int* input_2, int* output, int m, int n, int t) {
  int row = blockIdx.x * blockDim.x + threadIdx.x;
  int col = blockIdx.y * blockDim.y + threadIdx.y;
  
  if(row < m && col < t){
    int sum = 0;
    for(int i = 0; i < n; i++){
      sum += input_1[row * n + i] * input_2[i * t + col];
    }
    output[row * t + col] = sum;
  }
}

// __global__ void matrixMuiltV2(int* input_1, int* input_2, int* output, int m, int n, int t, int k=4) {
//   int col_size = divUp(blockDim.x, k);
//   int row_size = divUp(blockDim.y, k);
//   __shared__ int A[blockDim.y][k];
//   __shared__ int B[k][blockDim.x];
//   for(int i = 0; i < blockDim.y; i++){
//     for(int j = 0; j < col_size; j ++){
//       FLOAT4(A[i][j * k]) = FLOAT4(input_1[i]);
//     }
//   }
//   for(int i = 0; i < row_size; i++){
//     for(int j = 0; j < blockDim.x; j ++){
//       FLOAT4(A[i][j * k]) = FLOAT4(input_2[i]);
//     }
//   }
//   __syncthreads();
  
// }

__global__ void conv2d(int* input, int* kernel, int* output, dim3 input_size, dim3 kernel_size) {
  int row = blockIdx.x * blockDim.x + threadIdx.x;
  int col = blockIdx.y * blockDim.y + threadIdx.y;
  if(row <= input_size.x - kernel_size.x && col <= input_size.y - kernel_size.y){
    int sum = 0;
    for(int i = 0; i < kernel_size.x; i++)
      for(int j = 0; j < kernel_size.y; j++){
        sum += input[(row + i) * input_size.y + col + j] * kernel[kernel_size.y * i + j];
      }
    output[row * (input_size.y - kernel_size.y + 1) + col] = sum;
  }
}

__global__ void addPadding2d(int* input, int* output, dim3 input_size, dim3 padding) {
  int row = blockIdx.x * blockDim.x + threadIdx.x;
  int col = blockIdx.y * blockDim.y + threadIdx.y;
  if(row < input_size.x && col < input_size.y){
    output[(row + padding.x)*(input_size.y + 2 * padding.y) + col + padding.y]= input[row * input_size.y + col];
  }
}
