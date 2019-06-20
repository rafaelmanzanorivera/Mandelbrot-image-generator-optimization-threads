//
//  mandelbrotCalculators.cpp
//  mendelbrot
//
//  Created by Rafael Manzano on 05/03/2019.
//  Copyright © 2019 Rafael Manzano. All rights reserved.
//

#include "mandelbrotCalculators.h"
//#include "ListaEnlazada.h"


#include <string>
#include <sstream>
#include <iostream>
#include <vector>

//Estructura de bloques de trabajo para division dinamica

std::vector<work_batch> ready;
std::vector<work_batch> done;
std::mutex mutex_ready;
std::mutex mutex_done;

//struct IObatches batches={NULL, NULL};


using namespace std;

float *createMandelbrotImageSerial(int width, int height, float xS, float yS, float rad, int maxIteration)
{
    
//
    
//    chrono::steady_clock::time_point totalTime, mendelbrotTimeS, mendelbrotTimeE, writeTimeS, writeTimeE, timeIni, timeEnd;
//    //duration<double> totalTimeD, mendelbrotTimeD, writeTimeD;
//
//
//
//    timeIni = chrono::steady_clock::now();
//
//    double totalTimeD = chrono::duration_cast<chrono::milliseconds>(timeEnd - timeIni).count();
//    //cout << "Total time " << totalTimeD << endl;
//    double mendelbrotTimeD = chrono::duration_cast<chrono::milliseconds>(mendelbrotTimeE - mendelbrotTimeS).count();
//    //cout << "Load time " << mendelbrotTimeD << endl;
//    double writeTimeD = chrono::duration_cast<chrono::milliseconds>(writeTimeE - writeTimeS).count();
//    //cout << "Filter time " << writeTimeD << endl;
//
//    cout << "\"" << "mandelbrot_"<< width << "x" << height  << "\",\"" << totalTimeD/1000 << "\",\"" << mendelbrotTimeD/1000 << "\",\"" << writeTimeD/1000  << "\"" << "\",\"" << nthreads  << "\",\"" ;
//
   // 1/1-f + (f/g)
    
   // timeIni = chrono::steady_clock::now();
    
    float *buffer = (float *) malloc(width * height * sizeof(float));
    if (buffer == NULL) {
        fprintf(stderr, "Could not create image buffer\n");
        return NULL;
    }
    
    // Create Mandelbrot set image
    
    int xPos, yPos;
    float minMu = maxIteration;
    float maxMu = 0;
    
    for (yPos=0 ; yPos<height ; yPos++)
    {
        float yP = (yS-rad) + (2.0f*rad/height)*yPos;
        
        for (xPos=0 ; xPos<width ; xPos++)
        {
            float xP = (xS-rad) + (2.0f*rad/width)*xPos;
            
            int iteration = 0;
            float x = 0;
            float y = 0;
            
            while (x*x + y*y <= 4 && iteration < maxIteration)
            {
                float tmp = x*x - y*y + xP;
                y = 2*x*y + yP;
                x = tmp;
                iteration++;
            }
            
            if (iteration < maxIteration) {
                float modZ = sqrt(x*x + y*y);
                float mu = iteration - (log(log(modZ))) / log(2);
                if (mu > maxMu) maxMu = mu;
                if (mu < minMu) minMu = mu;
                buffer[yPos * width + xPos] = mu;
            }
            else {
                buffer[yPos * width + xPos] = 0;
            }
        }
    }
    
    // Scale buffer values between 0 and 1
    int count = width * height;
    while (count) {
        count --;
        buffer[count] = (buffer[count] - minMu) / (maxMu - minMu);
    }
    
    return buffer;
}




float *createMandelbrotImageParallelStatic(int width, int height, float xS, float yS, float rad, int maxIteration, int nthreads)
{
    
     chrono::steady_clock::time_point totalTime, mendelbrotTimeS, mendelbrotTimeE, writeTimeS, writeTimeE, timeIni, timeEnd;
    
    float *buffer = (float *) malloc(width * height * sizeof(float));
    if (buffer == NULL)
    {
        fprintf(stderr, "Could not create image buffer\n");
        return NULL;
    }
    mendelbrotTimeS = chrono::steady_clock::now();
    
    //TODO locks para estas variables
    float globalMinMu = maxIteration;
    float globalMaxMu = 0;
    
    
    //crear paquetes de trabajo
    int workBatchSize = (height) / nthreads;
    int offset = (height) % nthreads;
    int nBatches = nthreads;
    
    std::thread* th1[nthreads];
    
    
    //TODO reservar batches e inicializar (minMu = maxIter) Esto se podria encapsular
    //work_batch batches[nBatches];//TODO no en stack
    
    work_batch *batchesList = (work_batch*)malloc(sizeof(work_batch)*nBatches);
    
    //    cout << "Image height: " << height << endl;
    //    cout << "Work per thread: " << workBatchSize << endl;
    //    cout << "Offset: " << offset << endl;
    
    int start = 0;
    int fin = start + workBatchSize;
    
    //Inicializamos los paquetes de trabajo
    for(int i = 0; i<nBatches; i++)
    {
        batchesList[i].ini = start;
        batchesList[i].fin = fin - 1;
        batchesList[i].minMu = maxIteration;
        batchesList[i].maxMu = 0;
        
        start+=workBatchSize;
        fin+=workBatchSize;
        
        if(i == nBatches-1)//ultimo batch
            batchesList[i].fin += offset;
    }
    
    mendelbrotTimeE = chrono::steady_clock::now();
    
    double totalTimeE = chrono::duration_cast<chrono::milliseconds>(mendelbrotTimeE - mendelbrotTimeS).count();
    //cout << "Total time paquetes " << totalTimeE/1000 << endl;
    
    
     mendelbrotTimeS = chrono::steady_clock::now();
    
    //Lanzas los threads, cada uno con la referencia a su correspondiente paquete de trabajo
    for (int t = 0; t < nthreads; t++)
    {
        th1[t] = new std::thread(mandelBrotThreadWorkStatic, width,  height,  xS,  yS,  rad,  maxIteration, buffer, &batchesList[t]);
    }
    
    for (int i = 0; i < nthreads; i++)
    {
        th1[i]->join();
    }
    mendelbrotTimeE = chrono::steady_clock::now();
    
    totalTimeE = chrono::duration_cast<chrono::milliseconds>(mendelbrotTimeE - mendelbrotTimeS).count();
    //cout << "Total time threads " << totalTimeE/1000 << endl;

      timeIni = chrono::steady_clock::now();
    
    //Calculamos minimos y maximos globales
    for(int i = 0; i < nBatches; i++)
    {
        if (batchesList[i].maxMu > globalMaxMu) globalMaxMu = batchesList[i].maxMu;
        if (batchesList[i].minMu < globalMinMu) globalMinMu = batchesList[i].minMu;
    }
    
    
    //Escalamos los valores del buffer entre 0 y 1 usando los maximos y minimos
    int count = width * height;
    while (count) {//CACHE
        count --;
        buffer[count] = (buffer[count] - globalMinMu) / (globalMaxMu - globalMinMu);
    }
    
    
    timeEnd = chrono::steady_clock::now();
    //
    double totalTimeD = chrono::duration_cast<chrono::milliseconds>(timeEnd - timeIni).count();
    //cout << "Total time bucle " << totalTimeD/1000 << endl;
    
    
    return buffer;
}


void mandelBrotThreadWorkStatic(int width, int height, float xS, float yS, float rad, int maxIteration, float* buffer, work_batch *work)
{
    
    int xPos, yPos;
    
    for (yPos=work->ini ; yPos<=work->fin ; yPos++)
    {
        float yP = (yS-rad) + (2.0f*rad/height)*yPos;
        
        for (xPos=0 ; xPos<width ; xPos++)
        {
            float xP = (xS-rad) + (2.0f*rad/width)*xPos;
            
            int iteration = 0;
            float x = 0;
            float y = 0;
            
            while (x*x + y*y <= 4 && iteration < maxIteration)
            {
                float tmp = x*x - y*y + xP;
                y = 2*x*y + yP;
                x = tmp;
                iteration++;
            }
            
            if (iteration < maxIteration) {
                float modZ = sqrt(x*x + y*y);
                float mu = iteration - (log(log(modZ))) / log(2);
                if (mu > work->maxMu) work->maxMu = mu;
                if (mu < work->minMu) work->minMu = mu;
                buffer[yPos * width + xPos] = mu;
            }
            else {
                buffer[yPos * width + xPos] = 0;
            }
        }
    }

}

float *createMandelbrotImageParallelDynamic(int width, int height, float xS, float yS, float rad, int maxIteration, int nthreads, int batchSize)
{
    
    float *buffer = (float *) malloc(width * height * sizeof(float));
    if (buffer == NULL)
    {
        fprintf(stderr, "Could not create image buffer\n");
        return NULL;
    }
    
    if(batchSize > height)
    {
        printf("Batch size is too big\n");
        return NULL;
    }
    
    
//    batches.ready = new ListaEnlazada();
//    batches.done = new ListaEnlazada();
    
    
    
    float globalMinMu = maxIteration;
    float globalMaxMu = 0;

    //crear paquetes de trabajo
    int workBatchSize = batchSize;
    int offset = (height) % batchSize;
    int nBatches = (height) / batchSize;
    
    std::thread* th1[nthreads];
    
    
//    cout << "Image height: " << height << endl;
//    cout << "Work per thread: " << workBatchSize << endl;
//    cout << "nBatches: " << nBatches << endl;
//    cout << "Offset: " << offset << endl;
    
    
    int start = 0;
    int fin = start + workBatchSize;
    
    work_batch batch;
    
    //Inicializamos los paquetes de trabajo
    for(int i = 0; i<nBatches; i++)
    {
        //Inicializamos paquete
        batch.ini = start;
        batch.fin = fin - 1;
        batch.minMu = maxIteration;
        batch.maxMu = 0;
        
        if(i == nBatches-1)//ultimo batch
            batch.fin += offset;
        
        //Insertamos en la lista
        //batches.ready->insert(batches.ready->getN(), batch);
        ready.push_back(batch);
        
        start+=workBatchSize;
        fin+=workBatchSize;
        
    }
    
//    for(int i = 0; i<ready.size(); i++)
//    {
//        cout << "Batch #:  " << i << endl;
//        cout << "   Ini: " << ready.at(i).ini << endl;
//        cout << "   End: " << ready.at(i).fin << endl;
//    }
//
//
    //Lanzas los threads, cada uno con la referencia a su correspondiente paquete de trabajo
    for (int t = 0; t < nthreads; t++)
    {
       th1[t] = new std::thread(mandelBrotThreadWorkDynamic, width,  height,  xS,  yS,  rad,  maxIteration, buffer);
    }
    
    for (int i = 0; i < nthreads; i++)
    {
        th1[i]->join();
    }
    
    

    for(int i = 0; i<done.size(); i++)
    {
        if (done.at(i).maxMu > globalMaxMu) globalMaxMu = done.at(i).maxMu;
        if (done.at(i).minMu < globalMinMu) globalMinMu = done.at(i).minMu;
    }


    //Escalamos los valores del buffer entre 0 y 1 usando los maximos y minimos
    int count = width * height;
    while (count) {
        count --;
        buffer[count] = (buffer[count] - globalMinMu) / (globalMaxMu - globalMinMu);
    }
    
//    delete batches.ready;
//    delete batches.done;
    
    return buffer;
}


void mandelBrotThreadWorkDynamic(int width, int height, float xS, float yS, float rad, int maxIteration, float* buffer)
{
    
    work_batch work;
    
    bool doneWork = false;

    //Tomamos el primer batch de la lista de trabajo y lo eliminamos de la misma
    mutex_ready.lock();
    work = ready.back();
    ready.pop_back();
    mutex_ready.unlock();

    int lastBatchEndsAt = 0;

    while(!doneWork)
    {
        //Hacemos el trabajo correspondiente
        int xPos, yPos;

        for (yPos=work.ini ; yPos<=work.fin ; yPos++)
        {
            float yP = (yS-rad) + (2.0f*rad/height)*yPos;

            for (xPos=0 ; xPos<width ; xPos++)
            {
                float xP = (xS-rad) + (2.0f*rad/width)*xPos;

                int iteration = 0;
                float x = 0;
                float y = 0;

                while (x*x + y*y <= 4 && iteration < maxIteration)
                {
                    float tmp = x*x - y*y + xP;
                    y = 2*x*y + yP;
                    x = tmp;
                    iteration++;
                }

                if (iteration < maxIteration) {
                    float modZ = sqrt(x*x + y*y);
                    float mu = iteration - (log(log(modZ))) / log(2);
                    if (mu > work.maxMu) work.maxMu = mu;
                    if (mu < work.minMu) work.minMu = mu;
                    buffer[yPos * width + xPos] = mu;
                }
                else {
                    buffer[yPos * width + xPos] = 0;
                }
            }
        }



        //Guardamos en la lista de trabajo terminado
        mutex_done.lock();
        done.push_back(work);
        mutex_done.unlock();

        //Sacas el siguiente elemento
        mutex_ready.lock();
        if(!ready.empty())
        {
            work = ready.back();
            ready.pop_back();
        }
        else
            doneWork = true;
        mutex_ready.unlock();

    }


}
