#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>
using std:: cout;
using std:: endl;

// setting the number of threads:
#ifndef NUMT
#define NUMT		4	
#endif

// setting the number of subdivisions:
#ifndef NUMNODES
#define NUMNODES	1000		
#endif

// setting the exponent N
#ifndef N
#define N		4				
#endif

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

// function prototypes
float Height( int, int );


int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	// additional setup including setup variables
	omp_set_num_threads( NUMT );	// set the number of threads to use in the for-loop
	float volSuperquad = 0.0;						
	int col = 0;
	int row = 0;
	
	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for-loop and a reduction
	double time0 = omp_get_wtime( );
	#pragma omp parallel for default(none) shared(col,row)  reduction(+:volSuperquad) 
	for( int i = 0; i < NUMNODES*NUMNODES; i++ )
	{
		// calculate row and column
		if ((i >= NUMNODES) && ((i % NUMNODES) == 0)) 
		{	 
				row += 1;
				col = 0;
		}

		// the area of a single full-sized tile:
		float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
					( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );

		// if edge case 1 fullTileArea *= 0.25 
		// if edge case 2 fullTileArea *= 0.50 
		if (row == 0)
		{
			if ((col == 0) || (col == (NUMNODES-1)))
				fullTileArea *= 0.25;
			else
				fullTileArea *= 0.50; 
		}
		if (row == (NUMNODES-1))
		{
 			if ((col == 0) || (col == (NUMNODES-1)))
				fullTileArea *= 0.25;
			else
				fullTileArea *= 0.50; 
		}	

		int iu = i % NUMNODES;
		int iv = i / NUMNODES;
		float z = Height( iu, iv );
		float fullTileVolume = z * 2.0 * fullTileArea;

		// add rest of code here - calculate sum
		volSuperquad += fullTileVolume;

		col += 1;
	}

	// calculate performance and volume average
	double time1 = omp_get_wtime( );
	double megaHeightsPerSecond = (double)NUMNODES*NUMNODES / ( time1 - time0 ) / 1000000.0;
	
	// Print Results (1)num threads (2)numnodes (3)performance (4)volume 
	cout 	<< "Threads: " << NUMT 
		<< "   NUMNODES: " << NUMNODES 
		<< "   Performance: " << megaHeightsPerSecond 
		<< "   Volume: " << volSuperquad 
		<< endl;
	cout << endl; 
	
	return 0;
}


float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r < 0. )
	        return 0.;
	float height = pow( 1. - xn - yn, 1./(float)N );
	return height;
}
