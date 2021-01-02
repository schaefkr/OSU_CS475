#include <xmmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;
using std::ofstream; 

// globals
#define SSE_WIDTH	4

#ifndef NUMTRIES
#define NUMTRIES	10	
#endif


// function prototypes
float SimdMulSum (float *, float *, int);

// main
int main( int argc, char *argv[ ] )
{
	// Read txt file and fill arrays
	FILE *fp = fopen( "signal.txt", "r" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open file 'signal.txt'\n" );
		exit( 1 );
	}
	int Size;
	fscanf( fp, "%d", &Size );
	float *A =     new float[ 2*Size ];
	float *Sums  = new float[ 1*Size ];
	for( int i = 0; i < Size; i++ )
	{
		fscanf( fp, "%f", &A[i] );
		A[i+Size] = A[i];		// duplicate the array
	}
	fclose( fp );
	

	// SIMD
	float maxPerformance = 0.0;
	double time0 = omp_get_wtime( );
	for (int i = 0; i < NUMTRIES; i++)
	{ 
		for( int shift = 0; shift < Size; shift++ )
		{
			Sums[shift] = SimdMulSum( &A[0], &A[0+shift], Size );	
		}
		
		double time1 = omp_get_wtime( );
		double megaMultSumsPerSec = (double)Size*Size / (time1-time0) / 1000000.0;
		if (maxPerformance < megaMultSumsPerSec)
		{	
			maxPerformance = megaMultSumsPerSec;
		}	
	}
	cout << "Performance: " << maxPerformance << endl;

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

