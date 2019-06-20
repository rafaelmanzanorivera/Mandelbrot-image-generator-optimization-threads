// LibPNG example
// A.Greensted
// http://www.labbookpages.co.uk

// Version 2.0
// With some minor corrections to Mandlebrot code (thanks to Jan-Oliver)

// Version 1.0 - Initial release

#include <stdio.h>
#include <omp.h>
#include <chrono>

#include <sstream>
#include <cstring>
#include <iostream>
#include <math.h>
#include <cstring>

#include "mandelbrotCalculators.h"
#include "writingFunctions.h"

#define DYNAMIC_DIV_BATCH_SIZE 5

using namespace std;

float *globalBuffer = nullptr;


double runWith(int imageTotalPixels, int nthreads, int parallel, int batch_size);


int main(int argc, char *argv[])
{
    int result = 0;
    int counter = 1;
    int imageSize = 10000000;
    int increment = 100000000;

    double serialTime;
    double parallelTime;
    double speedUp;

    cout << "image,totalTime,mendelbrotTime,writeTime,nThreads,speedUp" << endl;
//
//
    while(counter <= 30)
    {

        serialTime = runWith(imageSize,1,0,-1);
        cout << "1\"" <<endl;

        for(int i = 2; i <= 10; i+=2)
        {
            parallelTime = runWith(imageSize,i,PARALLEL_STATIC,-1);
            speedUp = serialTime/parallelTime;
            cout << speedUp << "\"" <<endl;

            parallelTime = runWith(imageSize,i,PARALLEL_DYNAMIC, DYNAMIC_DIV_BATCH_SIZE);
            speedUp = serialTime/parallelTime;
            cout << speedUp << "\"" <<endl;

//            for(int batch_size = 1; batch_size < 15; batch_size += 1)
//            {
//                parallelTime = runWith(imageSize,i,PARALLEL_DYNAMIC, batch_size);
//                speedUp = serialTime/parallelTime;
//                cout << speedUp << "\", -> " << "BATCH_SIZE: " << batch_size << endl;
//
//            }
        }

        imageSize += increment;
        counter++;
    }
//
//////
//

//    runWith(10000000, 8, PARALLEL_DYNAMIC, DYNAMIC_DIV_BATCH_SIZE);cout<<endl;
//    runWith(100000000, 8, PARALLEL_DYNAMIC, DYNAMIC_DIV_BATCH_SIZE);cout<<endl;

    return result;
}

double runWith(int imageTotalPixels, int nthreads, int parallel, int batch_size)
{


    chrono::steady_clock::time_point totalTime, mendelbrotTimeS, mendelbrotTimeE, writeTimeS, writeTimeE, timeIni, timeEnd;
    //duration<double> totalTimeD, mendelbrotTimeD, writeTimeD;

    // Specify an output image size
    int width = sqrt(imageTotalPixels);
    int height = sqrt(imageTotalPixels);

    float *globalBuffer = nullptr;

    stringstream imageName;
    imageName << "mandelbrot_"<< width << "x" << height << ".png" << endl;


    timeIni = chrono::steady_clock::now();

    //int xS = -0.802;
    float xS = -0.802;;//-0.802;
    float yS = -0.177;//-0.177;


    switch (parallel)
    {
        case SERIAL:
            mendelbrotTimeS = chrono::steady_clock::now();
            globalBuffer = createMandelbrotImageSerial(width, height, xS, yS, 0.011, 110);
            mendelbrotTimeE = chrono::steady_clock::now();
            if (globalBuffer == NULL) return 1;
            break;

        case PARALLEL_STATIC:
            mendelbrotTimeS = chrono::steady_clock::now();
            globalBuffer = createMandelbrotImageParallelStatic(width, height, xS, yS, 0.011, 110, nthreads);
            mendelbrotTimeE = chrono::steady_clock::now();
            if (globalBuffer == NULL) return 1;
            break;

        case PARALLEL_DYNAMIC:
            mendelbrotTimeS = chrono::steady_clock::now();
            globalBuffer = createMandelbrotImageParallelDynamic(width, height, xS, yS, 0.011, 110, nthreads, DYNAMIC_DIV_BATCH_SIZE);
            mendelbrotTimeE = chrono::steady_clock::now();
            if (globalBuffer == NULL) return 1;
            break;

        default:
            break;
    }
    
    

    imageName.str(std::string());
    imageName << "mandelbrot_"<< width << "x" << height << "_" << parallel << ".png" << endl;
    
    const std::string tmp = imageName.str();
    char *pline = new char[imageName.str().size() + 1];
    pline[imageName.str().size()] = '\0';
    strcpy ( pline, imageName.str().c_str() );
    
    //cout << "Saving\n";
    writeTimeS = chrono::steady_clock::now();
    int result = writeImageSerial(pline, width, height, globalBuffer, NULL);
    writeTimeE = chrono::steady_clock::now();

    timeEnd = chrono::steady_clock::now();

//
    double totalTimeD = chrono::duration_cast<chrono::milliseconds>(timeEnd - timeIni).count();
    //cout << "Total time " << totalTimeD << endl;
    double mendelbrotTimeD = chrono::duration_cast<chrono::milliseconds>(mendelbrotTimeE - mendelbrotTimeS).count();
    //cout << "Load time " << mendelbrotTimeD << endl;
    double writeTimeD = chrono::duration_cast<chrono::milliseconds>(writeTimeE - writeTimeS).count();
    //cout << "Filter time " << writeTimeD << endl;

    cout << "\"" << "mandelbrot_"<< width << "x" << height  << "\",\"" << totalTimeD/1000 << "\",\"" << mendelbrotTimeD/1000 << "\",\"" << writeTimeD/1000  << "\"" << "\",\"" << nthreads  << "\",\"" ;


    switch (parallel)
    {
        case SERIAL:
            cout << "SERIAL\",\"" ;
            break;

        case PARALLEL_STATIC:
            cout << "STATIC\",\"" ;
            break;

        case PARALLEL_DYNAMIC:
            cout << "DYNAMIC\",\"" ;
            break;

        default:
            break;
    }

    free(globalBuffer);
    return totalTimeD;
}
