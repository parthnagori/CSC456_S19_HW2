#include <stdio.h>
#include <stdlib.h>
#include "mysort.h"
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <string>
#ifdef APPLE
#include <OpenCL/opencl.h>
//#include "scoped_array.h"
#else
#include "CL/opencl.h"
#include "AOCL_Utils.h"
using namespace aocl_utils;
#endif
#ifdef APPLE
// OpenCL runtime configuration
cl_platform_id platform = NULL;
unsigned num_devices = 1;
cl_device_id device; // num_devices elements
cl_context context = NULL;
cl_command_queue queue; // num_devices elements
cl_program program = NULL;
cl_kernel kernel; // num_devices elements
#else
// OpenCL runtime configuration
cl_platform_id platform = NULL;
unsigned num_devices = 0;
scoped_array<cl_device_id> device; // num_devices elements
cl_context context = NULL;
scoped_array<cl_command_queue> queue; // num_devices elements
cl_program program = NULL;
scoped_array<cl_kernel> kernel; // num_devices elements
#endif

#ifdef APPLE
static int LoadTextFromFile(const char *file_name, char **result_string, size_t *string_len);
#define LOCAL_MEM_SIZE = 1024;
void _checkError(int line,
								 const char *file,
								 cl_int error,
                 const char *msg,
                 ...);

#define checkError(status, ...) _checkError(__LINE__, __FILE__, status, __VA_ARGS__)
#endif

bool init_opencl();

int fpga_sort(int num_of_elements, float *data)
{
    init_opencl();
	
    return 0;
}

// Initializes the OpenCL objects.
bool init_opencl() {
  int err;
  cl_int status;

  printf("Initializing OpenCL\n");
#ifdef APPLE
  int gpu = 1;
  err = clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device, NULL);
  if (err != CL_SUCCESS)
  {
    fprintf(stderr, "Error: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }
  // Create the context.
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
  checkError(status, "Failed to create context");
#else 
  if(!setCwdToExeDir()) {
    return false;
  }

  // Get the OpenCL platform.
  platform = findPlatform("Altera");
 if(platform == NULL) {
   printf("ERROR: Unable to find Altera OpenCL platform.\n");
   return false;
 }

  // Query the available OpenCL device.
  device.reset(getDevices(platform, CL_DEVICE_TYPE_ALL, &num_devices));
  printf("Platform: %s\n", getPlatformName(platform).c_str());
  printf("Using %d device(s)\n", num_devices);
  for(unsigned i = 0; i < num_devices; ++i) {
    printf("  %s\n", getDeviceName(device[i]).c_str());
  }
  // Create the context.
  context = clCreateContext(NULL, num_devices, device, NULL, NULL, &status);
  checkError(status, "Failed to create context");
#endif

  // Create the program for all device. Use the first device as the
  // representative device (assuming all device are of the same type).
#ifndef APPLE
  std::string binary_file = getBoardBinaryFile("fpgasort", device[0]);
  printf("Using AOCX: %s\n", binary_file.c_str());
  program = createProgramFromBinary(context, binary_file.c_str(), device, num_devices);

  // Build the program that was just created.
  status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
  checkError(status, "Failed to build program");

  //Create per-device objects.
  queue.reset(num_devices);
  kernel.reset(num_devices);
  for(unsigned i = 0; i < num_devices; ++i) {
    // Command queue.
    queue[i] = clCreateCommandQueue(context, device[i], CL_QUEUE_PROFILING_ENABLE, &status);
    checkError(status, "Failed to create command queue");

    // Kernel.
    const char *kernel_name = "fpgasort";
    kernel[i] = clCreateKernel(program, kernel_name, &status);
    checkError(status, "Failed to create kernel");

  }
#else
  char *source = 0;
  size_t length = 0;
  LoadTextFromFile("fpgasort.cl", &source, &length);
  const char *kernel_name = "fpgasort";
  program = clCreateProgramWithSource(context, 1, (const char **) & source, NULL, &err);

  // Build the program that was just created.
  status = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  checkError(status, "Failed to build program");

  queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);
  kernel = clCreateKernel(program, kernel_name, &status);
#endif
  return true;
}

void cleanup() {
#ifndef APPLE
  for(unsigned i = 0; i < num_devices; ++i) {
    if(kernel && kernel[i]) {
      clReleaseKernel(kernel[i]);
    }
    if(queue && queue[i]) {
      clReleaseCommandQueue(queue[i]);
    }
  }
#else
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
#endif
  if(program) {
    clReleaseProgram(program);
  }
  if(context) {
    clReleaseContext(context);
  }
}
#ifdef APPLE
static int LoadTextFromFile(
    const char *file_name, char **result_string, size_t *string_len)
{
    int fd;
    unsigned file_len;
    struct stat file_status;
    int ret;
 
    *string_len = 0;
    fd = open(file_name, O_RDONLY);
    if (fd == -1)
    {
        printf("Error opening file %s\n", file_name);
        return -1;
    }
    ret = fstat(fd, &file_status);
    if (ret)
    {
        printf("Error reading status for file %s\n", file_name);
        return -1;
    }
    file_len = file_status.st_size;
 
    *result_string = (char*)calloc(file_len + 1, sizeof(char));
    ret = read(fd, *result_string, file_len);
    if (!ret)
    {
        printf("Error reading from file %s\n", file_name);
        return -1;
    }
 
    close(fd);
 
    *string_len = file_len;
    return 0;
}

// High-resolution timer.
double getCurrentTimestamp() {
#ifdef _WIN32 // Windows
  // Use the high-resolution performance counter.

  static LARGE_INTEGER ticks_per_second = {};
  if(ticks_per_second.QuadPart == 0) {
    // First call - get the frequency.
    QueryPerformanceFrequency(&ticks_per_second);
  }

  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);

  double seconds = double(counter.QuadPart) / double(ticks_per_second.QuadPart);
  return seconds;
#else         // Linux
  timespec a;
  clock_gettime(CLOCK_MONOTONIC, &a);
  return (double(a.tv_nsec) * 1.0e-9) + double(a.tv_sec);
#endif
}

void _checkError(int line,
								 const char *file,
								 cl_int error,
                 const char *msg,
                 ...) {
	// If not successful
	if(error != CL_SUCCESS) {
		// Print line and file
    printf("ERROR: ");
    printf("\nLocation: %s:%d\n", file, line);

    // Print custom message.
    va_list vl;
    va_start(vl, msg);
    vprintf(msg, vl);
    printf("\n");
    va_end(vl);

    // Cleanup and bail.
    cleanup();
    exit(error);
    }
}
#endif