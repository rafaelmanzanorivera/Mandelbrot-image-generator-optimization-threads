//
//  writingFunctions.hpp
//  mendelbrot
//
//  Created by Rafael Manzano on 05/03/2019.
//  Copyright © 2019 Rafael Manzano. All rights reserved.
//

#ifndef writingFunctions_h
#define writingFunctions_h

#include <stdio.h>
#include <png.h>
#include <string>
#include <cstring>

using namespace std;

// This takes the float value 'val', converts it to red, green & blue values, then
// sets those values into the image memory buffer location pointed to by 'ptr'
inline void setRGB(png_byte *ptr, float val);

// This function actually writes out the PNG image file.
int writeImageSerial(char* filename, int width, int height, float *buffer, char* title);
int writeImageParallel(string filename, int width, int height, float *buffer, char* title, int nthreads);

#endif /* writingFunctions_hpp */
