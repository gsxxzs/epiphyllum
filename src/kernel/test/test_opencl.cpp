// Copyright (c) 2024 Horizon Robotics.All Rights Reserved.
//
// The material in this file is confidential and contains trade secrets
// of Horizon Robotics Inc. This is proprietary information owned by
// Horizon Robotics Inc. No part of this work may be disclosed,
// reproduced, copied, transmitted, or used in any way for any purpose,
// without the express written permission of Horizon Robotics Inc.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "opencv2/opencv.hpp"
#include "CL/cl.h"
#include "util/log.h"



/** convert the kernel file into a string */
int convertToString(const char *filename, std::string &s) {
  size_t size;
  char *str;
  std::fstream f(filename, (std::fstream::in | std::fstream::binary));

  if (f.is_open()) {
    size_t fileSize;
    f.seekg(0, std::fstream::end);
    size = fileSize = (size_t)f.tellg();
    f.seekg(0, std::fstream::beg);
    str = new char[size + 1];
    if (!str) {
      f.close();
      return 0;
    }

    f.read(str, fileSize);
    f.close();
    str[size] = '\0';
    s = str;
    delete[] str;
    return 0;
  }
  LOGE("Error: failed to open file :{}", filename);
  return -1;
}

void printPlatformInfo(const cl_platform_id &platform) {
  char buffer[1024];
  clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(buffer), buffer,
                    nullptr);
  LOGI(" Platform Name: {}", buffer);
  clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(buffer), buffer,
                    nullptr);
  LOGI(" Platform Vendor: {}", buffer);
  clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(buffer), buffer,
                    nullptr);
  LOGI(" Platform Version: {}", buffer);
}

void printDeviceInfo(const cl_device_id &device) {
  char buffer[1024];
  clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buffer), buffer, nullptr);
  LOGI(" Device Name: {}", buffer);
  clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(buffer), buffer, nullptr);
  LOGI(" Device Vendor: {}", buffer);
  clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(buffer), buffer, nullptr);
  LOGI(" Device Version: {}", buffer);

  cl_ulong mem_size;
  clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size),
                  &mem_size, nullptr);
  LOGI(" Global Memory Size: {} MB", mem_size / (1024 * 1024));

  cl_uint compute_units;
  clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units),
                  &compute_units, nullptr);
  LOGI(" Compute Units: {}", compute_units);

  size_t work_group_size;
  clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
                  sizeof(work_group_size), &work_group_size, nullptr);
  LOGI(" Max Work Group Size: {}", work_group_size);
}

/**Getting platforms and choose an available one.*/
int getPlatform(cl_platform_id &platform) {
  platform = NULL;  // the chosen platform

  cl_uint numPlatforms;  // the NO. of platforms
  cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
  if (status != CL_SUCCESS) {
    LOGE("Error: Getting platforms!");
    return -1;
  }
  LOGI("numPlatforms : {}", numPlatforms);
  /**For clarity, choose the first available platform. */
  if (numPlatforms > 0) {
    cl_platform_id *platforms = reinterpret_cast<cl_platform_id *>(
        malloc(numPlatforms * sizeof(cl_platform_id)));
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    platform = platforms[0];
    free(platforms);
  } else {
    return -1;
  }
  printPlatformInfo(platform);
  return 0;
}

/**Step 2:Query the platform and choose the first GPU device if has one.*/
cl_device_id *getCl_device_id(cl_platform_id &platform) {
  cl_uint numDevices = 0;
  cl_device_id *devices = NULL;
  clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
  LOGI(" numDevices : {}", numDevices);
  if (numDevices > 0) {  // GPU available.
    devices = reinterpret_cast<cl_device_id *>(
        malloc(numDevices * sizeof(cl_device_id)));
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
    printDeviceInfo(devices[0]);
  }
  return devices;
}

/* Save cl kernel binary */
void saveBinary(const std::string &kernel, cl_program &program) {
  size_t binarySize;
  cl_int status;
  status = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t),
                            &binarySize, nullptr);
  if (status != CL_SUCCESS) {
    LOGE(" Error in clGetProgramInfo (CL_PROGRAM_BINARY_SIZES)");
    return;
  }

  std::vector<unsigned char> binary(binarySize);
  unsigned char *binaries[] = {binary.data()};
  status = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(binaries),
                            binaries, nullptr);
  if (status != CL_SUCCESS) {
    LOGE(" Error in clGetProgramInfo (CL_PROGRAM_BINARIES)");
    return;
  }

  std::ofstream file(kernel, std::ios::binary);
  file.write(reinterpret_cast<const char *>(binary.data()), binary.size());
  file.close();
}

/* Load cl kernel binary */
int loadBinary(std::vector<unsigned char> &binary, const std::string &kernel) {
  std::ifstream file(kernel, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    return -1;
  }
  std::ifstream::pos_type fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  binary.resize(fileSize);
  file.read(reinterpret_cast<char *>(binary.data()), fileSize);
  if (!file) {
    LOGE(" Failed to read binary file : {}", kernel);
    return -1;
  }
  file.close();
  return 0;
}


#define CHECK_STATUS(status)         \
  {                                  \
    auto code = status;              \
    if ((code) != CL_SUCCESS) {      \
      LOGE("error code: {} ", code); \
      return -1;                     \
    }                                \
  }


int main(int argc, char *argv[]) {

  /**Step 0: Prepare data.*/
  std::string srcImgPath{"../../data/zebra_cls.jpg"};
  std::string dstImgPath{"./zebra_cls_threshold.jpg"};

  cv::Mat srcMat = cv::imread(srcImgPath.c_str(), cv::IMREAD_GRAYSCALE);
  if (srcMat.empty()) {
    LOGE("read image {} failed ", srcImgPath.c_str());
    return -1;
  }
  cv::Mat dstMat(srcMat.rows, srcMat.cols, CV_8UC1);
  cv::Mat dstMatCL(srcMat.rows, srcMat.cols, CV_8UC1);

  cl_int status;
  /**Step 1: Getting platforms and choose an available one(first).*/
  cl_platform_id platform;
  CHECK_STATUS(getPlatform(platform));

  /**Step 2:Query the platform and choose the first GPU device if has one.*/
  cl_device_id *devices = getCl_device_id(platform);

  /**Step 3: Create context.*/
  cl_context context =
      clCreateContext(nullptr, 1, devices, nullptr, nullptr, &status);
  CHECK_STATUS(status);

  /**Step 4: Creating command queue associate with the context.*/
  cl_command_queue commandQueue =
      clCreateCommandQueueWithProperties(context, devices[0], 0, &status);
  CHECK_STATUS(status);

  /**Step 5: Create program object */
  cl_program program;

  const char *filename = "../../src/kernel/opencl_kernel/threshold.cl";
  std::string sourceStr;
  CHECK_STATUS(convertToString(filename, sourceStr));

  const char *source = sourceStr.c_str();
  size_t sourceSize[] = {strlen(source)};
  program =
      clCreateProgramWithSource(context, 1, &source, sourceSize, &status);
  CHECK_STATUS(status);

  /**Step 6: Build program. */
  status = (clBuildProgram(program, 1, devices, nullptr, nullptr, nullptr));
  if (status != CL_SUCCESS) {
    char buildLog[16384];
    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG,
                          sizeof(buildLog), buildLog, nullptr);
    LOGE("Error in kernel : {}", buildLog);
    clReleaseProgram(program);
    return -1;
  }

  /**Step 7: Initial input,output for the host and create memory objects for the kernel*/
  cl_mem inputBuffer = clCreateBuffer(
      context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, srcMat.total(),
      reinterpret_cast<void *>(srcMat.data), &status);
  CHECK_STATUS(status);
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                       dstMatCL.total(), nullptr, &status);
  CHECK_STATUS(status);

  /**Step 8: Create kernel object */
  cl_kernel kernelThreshold = clCreateKernel(program, "threshold", &status);
  CHECK_STATUS(status);
  int width{srcMat.cols};
  int height{srcMat.rows};
  unsigned char threshold{100};
  unsigned char maxValue{200};
  const int workItemWidthSize{32};
  const int workItemHeightSize{16};

  /**Step 9: Sets Kernel arguments.*/
  CHECK_STATUS(clSetKernelArg(kernelThreshold, 0, sizeof(cl_mem),
                              reinterpret_cast<void *>(&outputBuffer)));
  CHECK_STATUS(clSetKernelArg(kernelThreshold, 1, sizeof(cl_mem),
                              reinterpret_cast<void *>(&inputBuffer)));
  CHECK_STATUS(clSetKernelArg(kernelThreshold, 2, sizeof(int), &width));
  CHECK_STATUS(clSetKernelArg(kernelThreshold, 3, sizeof(int), &height));
  CHECK_STATUS(
      clSetKernelArg(kernelThreshold, 4, sizeof(int), &workItemWidthSize));
  CHECK_STATUS(
      clSetKernelArg(kernelThreshold, 5, sizeof(int), &workItemHeightSize));
  CHECK_STATUS(
      clSetKernelArg(kernelThreshold, 6, sizeof(unsigned char), &threshold));
  CHECK_STATUS(
      clSetKernelArg(kernelThreshold, 7, sizeof(unsigned char), &maxValue));

  // set work group num and work group size
  size_t groupSize[] = {4, 4};
  size_t groupNumSize[] = {
      (static_cast<size_t>(width) + workItemWidthSize - 1) / workItemWidthSize,
      (static_cast<size_t>(height) + workItemHeightSize - 1) /
          workItemHeightSize};

  /**Step 10: Running the kernel.*/
  cl_event enentPoint;
  CHECK_STATUS(clEnqueueNDRangeKernel(commandQueue, kernelThreshold, 2, nullptr,
                                      groupNumSize, groupSize, 0, nullptr,
                                      &enentPoint));
  clWaitForEvents(1, &enentPoint);  /// wait
  clReleaseEvent(enentPoint);

  /**Step 11: Read the cout put back to host memory.*/
  int count = 0;
  CHECK_STATUS(clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0,
                                   dstMatCL.total(), dstMatCL.data, 0, nullptr,
                                   nullptr));

  // check result
  for (int i = 0; i < srcMat.rows; ++i) {
    for (int j = 0; j < srcMat.cols; ++j) {
      dstMat.at<unsigned char>(i, j) =
          srcMat.at<unsigned char>(i, j) > threshold ? maxValue : 0;
      if (dstMat.at<unsigned char>(i, j) != dstMatCL.at<unsigned char>(i, j)) {
        LOGE("[{} {}] srcMat {}, dstMat {}, dstMatCL {}", i, j,
             static_cast<int>(srcMat.at<unsigned char>(i, j)),
             static_cast<int>(dstMat.at<unsigned char>(i, j)),
             static_cast<int>(dstMatCL.at<unsigned char>(i, j)));
        if (count++ > 63) {
          return -1;
        }
      }
    }
  }

  cv::imwrite(dstImgPath, dstMatCL);

  /**Step 12: Clean the resources.*/
  CHECK_STATUS(clReleaseKernel(kernelThreshold));  // Release kernel.
  CHECK_STATUS(clReleaseProgram(program));        // Release the program object.
  CHECK_STATUS(clReleaseMemObject(inputBuffer));  // Release mem object.
  CHECK_STATUS(clReleaseMemObject(outputBuffer));
  CHECK_STATUS(clReleaseCommandQueue(commandQueue));  // Release  Command queue.
  CHECK_STATUS(clReleaseContext(context));            // Release context.

  if (devices != nullptr) {
    free(devices);
    devices = nullptr;
  }
  return 0;
}
