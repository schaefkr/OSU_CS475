#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;
using std::ofstream; 

// setting the number of threads:
#ifndef NUMT
#define	NUMT		1
#endif

// setting the number of loop tries 
#ifndef NUMTRIES	
#define NUMTRIES	10	
#endif


// main program
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


	// OpenMP
	float maxPerformance = 0.0;		// must be declared outside the NUMTRIES loop
	omp_set_num_threads( NUMT );		// set the number of threads to use in the for-loop

	for (int t = 0; t < NUMTRIES; t++)	
	{
		float sum = 0.0;
		double time0 = omp_get_wtime( );

		#pragma omp parallel for default(none), shared(A,Sums,Size), reduction(+:sum)
		for( int shift = 0; shift < Size; shift++ )
		{	
			sum = 0.0;
			for( int i = 0; i < Size; i++ )
			{
				sum += A[i] * A[i + shift];
			}
			Sums[shift] = sum;	// note the "fix #2" from false sharing if you are using OpenMP
		}
		double time1 = omp_get_wtime( );	
		double megaMultiplyAddsPerSec = (double)Size*Size / (time1-time0) / 1000000.0;	
		if (maxPerformance < megaMultiplyAddsPerSec)
		{ 
			maxPerformance = megaMultiplyAddsPerSec;	
		}
	}

	cout << "Best Performance: " << maxPerformance << endl;
	
	// Write Sums[1] to Sums[512] to csv file
	ofstream openmpOutput;
	openmpOutput.open("openmp.csv");
	openmpOutput << "Sums, Shift" << endl; 

	for (int i = 1; i <= 512; i++)
	{
		openmpOutput << Sums[i] << "," << i << endl; 
	} 
	openmpOutput.close();
	

	return 0;
}
