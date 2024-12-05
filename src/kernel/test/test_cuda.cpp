#include <cuda_runtime.h>
#include "../kernel.h"
#include <iostream>
#include <unistd.h>

void testIncrementArray(){
  const int size = 10;
  int host_arr[size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // CPU上的数组
  int* device_arr; // GPU上的数组指针

  cudaMalloc((void**)&device_arr, size * sizeof(int));
  cudaMemcpy(device_arr, host_arr, size * sizeof(int), cudaMemcpyHostToDevice);
  int threadsPerBlock = 256;
  int blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock;
  incrementArray<<<blocksPerGrid, threadsPerBlock>>>(device_arr, size);
  cudaDeviceSynchronize();
  cudaMemcpy(host_arr, device_arr, size * sizeof(int), cudaMemcpyDeviceToHost);

  for (int i = 0; i < size; ++i) {
      std::cout << host_arr[i] << " ";
  }
  std::cout << std::endl;

  cudaFree(device_arr);
}

void testMatixMuilt(){
  int matix1[3][2] {{1, 1}, {2, 2}, {3, 3}};
  int matix2[2][3] {{1, 2, 3}, {4, 5, 6}};

  int *matix1_gpu, *matix2_gpu, *output_gpu;
  cudaMalloc(&matix1_gpu, 6 * sizeof(int));
  cudaMalloc(&matix2_gpu, 6 * sizeof(int));
  cudaMalloc(&output_gpu, 9 * sizeof(int));

  cudaMemcpy(matix1_gpu, matix1, 6 * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(matix2_gpu, matix2, 6 * sizeof(int), cudaMemcpyHostToDevice);

  dim3 thread_block(3, 3);
  dim3 graph_block(1, 1);

  matrixMuilt<<<graph_block, thread_block>>>(matix1_gpu, matix2_gpu, output_gpu, 3, 2, 3);
  cudaDeviceSynchronize();
  int* output = (int *)malloc(sizeof(int) * 9);
  cudaMemcpy(output, output_gpu, 9 * sizeof(int), cudaMemcpyDeviceToHost);

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j ++){
      std::cout << *(output + i * 3 + j) << ",";
    }
    std::cout << std::endl;
  }
  cudaFree(matix1_gpu);
  cudaFree(matix2_gpu);
  cudaFree(output_gpu);
  free(output);
}

void testConv2d(){
  int matix1[5][6] {{1}};
  int ttt = 1;
  for(int i =0; i < 5; i++)
    for(int j = 0; j < 6; j++)
      matix1[i][j] = ttt++;
      
  int matix2[3][3] {{1, 0, -1}, {1, 0, -1}, {1, 0, -1}};
  int *matix1_gpu, *matix2_gpu, *output_gpu;
  cudaMalloc(&matix1_gpu, 5 * 6 * sizeof(int));
  cudaMalloc(&matix2_gpu, 9 * sizeof(int));
  cudaMalloc(&output_gpu, 5 * 6 * sizeof(int));

  cudaMemcpy(matix1_gpu, matix1, 5 * 6 * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(matix2_gpu, matix2, 9 * sizeof(int), cudaMemcpyHostToDevice);
  
  int *padding_matix;
  cudaMalloc(&padding_matix, 7 * 8 * sizeof(int));
  // cudaMemset按每个字节设置，int是四个字节所以只能设置为0
  cudaMemset(padding_matix, 0, 7 * 8 * sizeof(int));
  addPadding2d<<<dim3(2,2), dim3(32, 32)>>>(matix1_gpu, padding_matix, dim3(5,6), dim3(1,1));
  cudaDeviceSynchronize();
  
  dim3 thread_block(32, 32);
  dim3 graph_block(2, 2);

  conv2d<<<graph_block, thread_block>>>(padding_matix, matix2_gpu, output_gpu, dim3(7,8), dim3(3,3));
  cudaDeviceSynchronize();
  int* output = (int *)malloc(sizeof(int) * 5 * 6 );
  cudaMemcpy(output, output_gpu, 5 * 6 * sizeof(int), cudaMemcpyDeviceToHost);

  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 6; j ++){
      std::cout << *(output + i * 4 + j) << ",";
    }
    std::cout << std::endl;
  }
  cudaFree(matix1_gpu);
  cudaFree(matix2_gpu);
  cudaFree(output_gpu);
  cudaFree(padding_matix);
  free(output);
}

int main(int argc, char** argv){
  // testIncrementArray();
  // testMatixMuilt();
  testConv2d();
  return 0;
}