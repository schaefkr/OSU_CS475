#include <xmmintrin.h>
#include <omp.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;

// setting the array size
#ifndef ARRAYSIZE 
#define ARRAYSIZE		1024 
#endif

// setting the number of times loop trials 
#ifndef NUMTRIES 
#define NUMTRIES		20 
#endif

// globals
#define SSE_WIDTH		4
float ARRAY_1[ARRAYSIZE];
float ARRAY_2[ARRAYSIZE];

// function prototypes
float SimdMulSum (float *, float *, int);
float NonSimdMulSum (float *, float *, int);

int main( int argc, char *argv[ ] )
{
	float simdSum;
	float nonsimdSum;
	double simdMaxPerformance = 0.0;
	double nonsimdMaxPerformance = 0.0;

	// fill arrays with values
	std::fill_n(ARRAY_1, ARRAYSIZE, 1.0);	
	std::fill_n(ARRAY_2, ARRAYSIZE, 1.0);	

	for (int i = 0; i < NUMTRIES; i++)
	{
		// simd array multiplication sum
		double simdTime0 = omp_get_wtime();
		simdSum = SimdMulSum (ARRAY_1, ARRAY_2, ARRAYSIZE);
		double simdTime1 = omp_get_wtime();	
		double simdPerformance = (double)ARRAYSIZE/(simdTime1-simdTime0)/1000000.0;
		if (simdPerformance > simdMaxPerformance)
			simdMaxPerformance = simdPerformance;

		// non-simd array multiplication sum
		double nonsimdTime0 = omp_get_wtime();
		nonsimdSum = NonSimdMulSum (ARRAY_1, ARRAY_2, ARRAYSIZE);
		double nonsimdTime1 = omp_get_wtime();
		double nonsimdPerformance = (double)ARRAYSIZE/(nonsimdTime1-nonsimdTime0)/1000000.0;
		if (nonsimdPerformance > nonsimdMaxPerformance)
			nonsimdMaxPerformance = nonsimdPerformance;
	}

	cout << "SIMD Performance: " << simdMaxPerformance << " MegaMults/Sec" << endl;
	cout << "Non-SIMD Performance: " << nonsimdMaxPerformance << " MegaMults/Sec" << endl;
	cout << endl;

	return 0;
}


float SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;

	__m128 ss = _mm_loadu_ps( &sum[0] );
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		ss = _mm_add_ps( ss, _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
	}
	_mm_storeu_ps( &sum[0], ss );

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}

float NonSimdMulSum( float *a, float *b, int len )
{
	float sum = 0.0;

	for (int i = 0; i < len; i++)
	{
		sum += a[i] * b[i]; 
	}	  

	return sum;
}
