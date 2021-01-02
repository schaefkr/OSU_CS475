#!/bin/bash

for t in 32 64
do
	echo LOCAL_SIZE = $t
	g++ -o opencl7b opencl7b.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp -DLOCAL_SIZE=$t
	./opencl7b
done
