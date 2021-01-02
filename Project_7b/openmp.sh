#!/bin/bash

# number of threads 
for t in 1 16 
do    
	echo NUMT = $t
	g++ -DNUMT=$t openmp.cpp -o openmp -lm -fopenmp
	./openmp
done

