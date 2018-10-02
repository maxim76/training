#include <stdio.h>

__global__ void printNumber(int number)
{
  printf("%d\n", number);
}

int main()
{
  cudaStream_t streams[5];
  for (int i = 0; i < 5; ++i) {
      cudaStreamCreate(&(streams[i]));
  }
  
  for (int i = 0; i < 5; ++i)
  {
    printNumber<<<1, 1, 0, streams[i]>>>(i);
  }
  cudaDeviceSynchronize();
  
  for (int i = 0; i < 5; ++i) {
      cudaStreamDestroy(streams[i]);
  }
  
}
