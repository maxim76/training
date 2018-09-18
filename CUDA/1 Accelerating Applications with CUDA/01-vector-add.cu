#include <stdio.h>
#include <assert.h>

inline cudaError_t checkCuda(cudaError_t result)
{
  if (result != cudaSuccess) {
    fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
    assert(result == cudaSuccess);
  }
  return result;
}


void initWith(float num, float *a, int N)
{
  for(int i = 0; i < N; ++i)
  {
    a[i] = num;
  }
}

__global__ void addVectorsInto(float *result, float *a, float *b, int N)
{
  /*
  for(int i = 0; i < N; ++i)
  {
    result[i] = a[i] + b[i];
  }
  */
  int gridStride = gridDim.x * blockDim.x;
  for( int i = blockIdx.x * blockDim.x + threadIdx.x; i < N; i += gridStride)
  {
    result[i] = a[i] + b[i];
  }
}

void checkElementsAre(float target, float *array, int N)
{
  for(int i = 0; i < N; i++)
  {
    if(array[i] != target)
    {
      printf("FAIL: array[%d] - %0.0f does not equal %0.0f\n", i, array[i], target);
      exit(1);
    }
  }
  printf("SUCCESS! All values added correctly.\n");
}

int main()
{
  const int N = 2<<20;
  size_t size = N * sizeof(float);

  float *a;
  float *b;
  float *c;

  //a = (float *)malloc(size);
  //b = (float *)malloc(size);
  //c = (float *)malloc(size);
  cudaMallocManaged( &a, size );
  cudaMallocManaged( &b, size );
  cudaMallocManaged( &c, size );


  initWith(3, a, N);
  initWith(4, b, N);
  initWith(0, c, N);

  int blockNum = 32;
  int tNum = 1024;
  addVectorsInto<<<blockNum, tNum>>>(c, a, b, N);
  checkCuda( cudaDeviceSynchronize() );  // Check sync errors
  checkCuda( cudaGetLastError() );       // Check async errors

  checkElementsAre(7, c, N);

  //free(a);
  //free(b);
  //free(c);
  cudaFree( a );
  cudaFree( b );
  cudaFree( c );
}
