#include "cross.h"
#ifndef CROSS_CPU
  #ifdef CROSS_HALF
    #include <cuda_fp16.h>
  #endif

    template <class T> struct real_to_bytes { };
    template <> struct real_to_bytes<double> {
      typedef unsigned long long int bytes;
      typedef double real;
      static CudaDeviceFunction bytes tobytes(real val) { return __double_as_longlong(val); }
      static CudaDeviceFunction real toreal(bytes val) { return __longlong_as_double(val); }
    };
    template <> struct real_to_bytes<float> {
      typedef unsigned int bytes;
      typedef float real;
      static CudaDeviceFunction bytes tobytes(real val) { return __float_as_int(val); }
      static CudaDeviceFunction real toreal(bytes val) { return __int_as_float(val); }
    };
    #ifdef CROSS_HALF
      template <> struct real_to_bytes<half> {
        typedef unsigned short int bytes;
        typedef half real;
        static CudaDeviceFunction bytes tobytes(real val) { return __half_as_short(val); }
        static CudaDeviceFunction real toreal(bytes val) { return __short_as_half(val); }
      };
    #endif

  template <class T>
      CudaDeviceFunction inline void atomicAddP(T* address, T val)
      {
        typedef real_to_bytes<T> R2B;
          if (val != 0.0) {
            typename R2B::bytes* address_as_ull = reinterpret_cast<typename R2B::bytes*>(address);
            typename R2B::bytes  old = *address_as_ull;
            typename R2B::bytes  assumed, nw;
            do {
                assumed = old;
                nw = R2B::tobytes(val + R2B::toreal(assumed));
                old = atomicCAS(address_as_ull, assumed, nw);
            } while (assumed != old);
          }
      }


  template <class T>
      CudaDeviceFunction inline void atomicMaxP(T* address, T val)
      {
        typedef real_to_bytes<T> R2B;
          if (val != 0.0) {
            typename R2B::bytes* address_as_ull = reinterpret_cast<typename R2B::bytes*>(address);
            typename R2B::bytes  old = *address_as_ull;
            typename R2B::bytes  assumed, nw;
            do {
                assumed = old;
                nw = R2B::tobytes(max(val,R2B::toreal(assumed)));
                old = atomicCAS(address_as_ull, assumed, nw);
            } while (assumed != old);
          }
      }

  #if __CUDA_ARCH__ >= 200
    template <> CudaDeviceFunction inline void atomicAddP(float* address, float val) { atomicAdd(address, val); }
    #if __CUDA_ARCH__ >= 600
      template <> CudaDeviceFunction inline void atomicAddP(double* address, double val) { atomicAdd(address, val); }
    #endif
  #endif


    #ifndef MAX_THREADS
      #error FUCK!
    #else
      #if MAX_THREADS < 500
        #error Double Fuck!
      #endif
    #endif

    template <class T>
      CudaDeviceFunction inline T blockSum(T val) {
              int i = blockDim.x*blockDim.y;
              int k = blockDim.x*blockDim.y;
              int j = blockDim.x*threadIdx.y + threadIdx.x;
              __shared__ T  sumtab[MAX_THREADS];
              __syncthreads();
              sumtab[j] = val;
              __syncthreads();
              while (i> 1) {
                      k = i >> 1;
                      i = i - k;
                      if (j<k) sumtab[j] += sumtab[j+i];
                      __syncthreads();
              }
              return sumtab[0];
      }

      template <class T>
      CudaDeviceFunction inline void atomicSum(T * sum, T val)
      {
              int j = blockDim.x*threadIdx.y + threadIdx.x;
              val = blockSum<T>(val);
              if (j==0) {
                if (val != 0.0) {
                  atomicAddP(sum, val);
                }
              }
      }

#if CUDART_VERSION >= 9000
template <class T>
CudaDeviceFunction inline void atomicSumWarp(T * sum, T val)
{
	#define FULL_MASK 0xffffffff
	if (__any_sync(FULL_MASK, val != 0)) {
		for (int offset = 16; offset > 0; offset /= 2)
		    val += __shfl_down_sync(FULL_MASK, val, offset);
		if (threadIdx.x == 0) atomicAddP(sum,val);
	}
}

template <class T, class P>
CudaDeviceFunction inline void atomicSumWarpArr(T * sum, P * val, unsigned char len)
{
	#define FULL_MASK 0xffffffff
	bool pred = false;
	for (unsigned char i=0; i<len; i++) pred = pred || (val[i] != 0.0);
	if (__any_sync(FULL_MASK, pred)) {
		for (int offset = 16; offset > 0; offset /= 2) {
			for (unsigned char i=0; i<len; i++) val[i] += __shfl_xor_sync(FULL_MASK, val[i], offset);
		}
		if (threadIdx.x < len) {
			atomicAddP(sum+threadIdx.x,static_cast<T>(val[threadIdx.x]));
		}
	}
}

#elif CUDART_VERSION >= 7000

template <class T>
CudaDeviceFunction inline void atomicSumWarp(T * sum, T val)
{
	#define FULL_MASK 0xffffffff
	if (__any(val != 0)) {
		for (int offset = 16; offset > 0; offset /= 2)
		    val += __shfl_down(val, offset);
		if (threadIdx.x == 0) atomicAddP(sum,val);
	}
}

template <class T>
CudaDeviceFunction inline void atomicSumWarpArr(T * sum, T * val, unsigned char len)
{
	#define FULL_MASK 0xffffffff
	bool pred = false;
	for (unsigned char i=0; i<len; i++) pred = pred || (val[i] != 0.0);
	if (__any(pred)) {
		for (int offset = 16; offset > 0; offset /= 2) {
			for (unsigned char i=0; i<len; i++) val[i] += __shfl_xor(val[i], offset);
		}
		if (threadIdx.x < len) {
			atomicAddP(sum+threadIdx.x,val[threadIdx.x]);
		}
	}
}
#else
  #warning "no atomicSumWarp for this CUDA version"
#endif

/*      CudaDeviceFunction inline void atomicSum(real_t * sum, real_t val) {
        typedef cub::BlockReduce<real_t, 32, cub::BLOCK_REDUCE_WARP_REDUCTIONS, 20> BlockReduce;
        __shared__ typename BlockReduce::TempStorage temp_storage;
        real_t ret = BlockReduce(temp_storage).Sum(val);
        if ((blockDim.x*threadIdx.y + threadIdx.x) == 0) atomicAddP(sum, ret);
      }
*/

      template <class T>
      CudaDeviceFunction inline void atomicMax(T * sum, T val)
      {
              int i = blockDim.x*blockDim.y;
              int k = blockDim.x*blockDim.y;
              int j = blockDim.x*threadIdx.y + threadIdx.x;
              __shared__ T  sumtab[MAX_THREADS];
              __syncthreads();
              sumtab[j] = val;
              __syncthreads();
              while (i> 1) {
                      k = i >> 1;
                      i = i - k;
                      if (j<k) sumtab[j] = max(sumtab[j],sumtab[j+i]);
                      __syncthreads();
              }
              if (j==0) atomicMaxP(sum,sumtab[0]);
      }

      template <class T>
      CudaDeviceFunction inline void atomicSumDiff(T * sum, T val, bool yes)
      {
                if (!yes) val = 0;
                atomicSum(sum,val);
      }
#else

    template <class T, class P> inline T data_cast(const P& x) { static_assert(sizeof(T)==sizeof(P),"Wrong sizes in data_cast"); T ret; memcpy(&ret, &x, sizeof(T)); return ret; }

    template <class T> struct real_to_bytes { };
    template <> struct real_to_bytes<double> {
      typedef unsigned long long int bytes;
      typedef double real;
      static CudaDeviceFunction bytes tobytes(real val) { return data_cast< bytes, real >(val); }
      static CudaDeviceFunction real toreal(bytes val) { return data_cast< real, bytes >(val); }
    };
    template <> struct real_to_bytes<float> {
      typedef unsigned int bytes;
      typedef float real;
      static CudaDeviceFunction bytes tobytes(real val) { return data_cast< bytes, real >(val); }
      static CudaDeviceFunction real toreal(bytes val) { return data_cast< real, bytes >(val); }
    };
    #ifdef CROSS_HALF
      template <> struct real_to_bytes<half> {
        typedef unsigned short int bytes;
        typedef half real;
        static CudaDeviceFunction bytes tobytes(real val) { return data_cast< bytes, real >(val); }
        static CudaDeviceFunction real toreal(bytes val) { return data_cast< real, bytes >(val); }
      };
    #endif
#endif
