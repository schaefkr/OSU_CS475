#include <math.h>
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <iomanip>
using std:: cout;
using std:: endl;
using std:: ofstream;
using std:: setw; 
using std:: left; 

// global variables
int	NowYear;		// 2020 - 2025
int	NowMonth;		// 0 - 11
float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population	
int	NowNumHuntingLicenses;	// number of hunting licenses	

// "interesting parameters that you need"
const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

const int MIN_LICENSES = 			1;
const int MAX_LICENSES = 			3; 

// function prototypes
void Watcher();
void GrainDeer();
void Grain();
void HuntingSeason();
float SQR( float );
float Ranf( float , float );
int Ranf( int , int );
void TimeOfDaySeed( );
float InchesToCentimeters( float );
float FahrenheitToCelsius( float );

int main( int argc, char *argv[] )
{
	// starting values 
	NowMonth = 0;
	NowYear = 2020;
	NowNumDeer = 1;
	NowHeight = 2.0;	
	NowPrecip = 7.0;
	NowTemp = 38.0;
	TimeOfDaySeed();		// seed variable
	
	omp_set_num_threads( 4 );	// same as # of sections
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			GrainDeer( );
		}

		#pragma omp section
		{
			Grain( );
		}

		#pragma omp section
		{
			Watcher( );
		}

		#pragma omp section
		{
			HuntingSeason( );
		}
	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here
		
	return 0;
}


void Watcher()
{
	// print & csv file 
	cout 	<< setw(10) << left << "Month" 
		<< setw(10) << left << "Year"
		<< setw(15) << left << "Temperature" 
		<< setw(20) << left << "Precipitation" 
		<< setw(10) << left << "Deer" 
		<< setw(10) << left << "Height" 
		<< setw(10) << left << "Hunting Licenses" << endl;	
	ofstream project3output;
	project3output.open("project3output.csv");
	project3output << "Month,Year,Temperature,Precipitation,Deer,Height,HuntingLicenses" << endl;

	while( NowYear < 2026 )
	{
		// DoneComputing barrier:
		#pragma omp barrier

		// DoneAssigning barrier:
		#pragma omp barrier

		// print & csv file
		cout 	<< setw(10) << left << NowMonth + 1 
			<< setw(10) << left << NowYear 
			<< setw(15) << left << FahrenheitToCelsius(NowTemp) 
			<< setw(20) << left << InchesToCentimeters(NowPrecip) 
			<< setw(10) << left << NowNumDeer 
			<< setw(10) << left << InchesToCentimeters(NowHeight) 
			<< setw(10) << left << NowNumHuntingLicenses << endl;	
		project3output 	<< NowMonth + 1 << "," 
				<< NowYear << "," 
				<< FahrenheitToCelsius(NowTemp) << "," 
				<< InchesToCentimeters(NowPrecip) << "," 
				<< NowNumDeer << "," 
				<< InchesToCentimeters(NowHeight) << "," 
				<< NowNumHuntingLicenses << endl;
	
		// udpate time	
		if (NowMonth == 11) 
		{
			NowMonth = 0;
			NowYear++;
		} 	
		else
		{	
			NowMonth++;
		}

		// calculate tempurature and precipitation
		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
		
		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		NowTemp = temp + Ranf( -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		NowPrecip = precip + Ranf( -RANDOM_PRECIP, RANDOM_PRECIP );
		if( NowPrecip < 0. )
			NowPrecip = 0.;

		// DonePrinting barrier:
		#pragma omp barrier
	}
	project3output.close();
}

void GrainDeer()
{
	while( NowYear < 2026 )
	{
		// compute a temporary next-value for this quantity
 		// based on the current state of the simulation:
		float tempHeight = NowHeight;
		int tempDeer = NowNumDeer;

		if (tempDeer < tempHeight)
		{
			tempDeer++;
		}
		else
		{
			tempDeer--;
		}
		if (NowNumHuntingLicenses >= 2)
		{
			tempDeer--;
		}		
 
		if (tempDeer <= 0)
		{
			tempDeer = 1;
		}

		// DoneComputing barrier:
		#pragma omp barrier
		NowNumDeer = tempDeer;

		// DoneAssigning barrier:
		#pragma omp barrier
 	
		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Grain()
{
	while( NowYear < 2026 )
	{
		// compute a temporary next-value for this quantity
 		// based on the current state of the simulation:
		float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
		float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

		// DoneComputing barrier:
		#pragma omp barrier
		NowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
 		NowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
	
		if (NowHeight < 0.0)
		{
			NowHeight = 0.2;
		}

		// DoneAssigning barrier:
		#pragma omp barrier
	
		// DonePrinting barrier:
		#pragma omp barrier
	}	
}

void HuntingSeason()
{
	while( NowYear < 2026 )
	{
		// compute a temporary next-value for this quantity
 		// based on the current state of the simulation:
		int tempNumHuntingLicenses = NowNumHuntingLicenses; 	

		if (((NowMonth >= 5) && (NowMonth <= 9)) && (NowNumDeer >= 4)) 
		{
			tempNumHuntingLicenses += Ranf( MIN_LICENSES, MAX_LICENSES );
		}
		else
		{
			tempNumHuntingLicenses --; 	
		}
		if (tempNumHuntingLicenses < 0)
		{
			tempNumHuntingLicenses = 0;
		}	

		// DoneComputing barrier:
		#pragma omp barrier
		NowNumHuntingLicenses = tempNumHuntingLicenses;

		// DoneAssigning barrier:
		#pragma omp barrier
 	
	
		// DonePrinting barrier:
		#pragma omp barrier
	
	}
}
 
float SQR( float x )
{
	return x*x;
}

float Ranf( float low, float high)
{
	float r = (float) rand( );              // 0 - RAND_MAX
	float t = r  /  (float) RAND_MAX;       // 0. - 1.
	return   low  +  t * ( high - low );
}

int Ranf( int ilow, int ihigh )
{
	float low = (float)ilow;
	float high = ceil( (float)ihigh );
	return (int) Ranf(low,high);
}

void TimeOfDaySeed( )
{
	struct tm y2k = { 0 };
	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t  timer;
	time( &timer );
	double seconds = difftime( timer, mktime(&y2k) );
	unsigned int seed = (unsigned int)( 1000.*seconds );    // milliseconds
	srand( seed );
}
	
float InchesToCentimeters( float inches )
{
	return inches * 2.54;
}

float FahrenheitToCelsius( float fahrenheit )
{
	float fahr = (5.0/9.0) * (fahrenheit - 32.0); 
	return fahr;
} 
