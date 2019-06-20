//
//  mandelbrotCalculators.hpp
//  mendelbrot
//
//  Created by Rafael Manzano on 05/03/2019.
//  Copyright © 2019 Rafael Manzano. All rights reserved.
//

#ifndef mandelbrotCalculators_h
#define mandelbrotCalculators_h


#define SERIAL 0
#define PARALLEL_STATIC 1
#define PARALLEL_DYNAMIC 2

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>




typedef struct work_batch
{
    int ini;
    int fin;
    float minMu;
    float maxMu;
}work_batch;





//typedef struct mutexes;

float *createMandelbrotImageParallelDynamic(int width, int height, float xS, float yS, float rad, int maxIteration, int nthreads, int batchSize);

// Creates a test image for saving. Creates a Mandelbrot Set fractal of size width x height

//Bounds struct version
float *createMandelbrotImageParallelStatic(int width, int height, float xS, float yS, float rad, int maxIteration, int nthreads);

float *createMandelbrotImageSerial(int width, int height, float xS, float yS, float rad, int maxIteration);

void mandelBrotThreadWorkStatic(int width, int height, float xS, float yS, float rad, int maxIteration, float* buffer, work_batch *work);

void mandelBrotThreadWorkDynamic(int width, int height, float xS, float yS, float rad, int maxIteration, float* buffer);

#endif /* mandelbrotCalculators_hpp */
