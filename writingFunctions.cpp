//
//  writingFunctions.cpp
//  mendelbrot
//
//  Created by Rafael Manzano on 05/03/2019.
//  Copyright © 2019 Rafael Manzano. All rights reserved.
//

#include "writingFunctions.h"


inline void setRGB(png_byte *ptr, float val)
{
    int v = (int)(val * 767);
    if (v < 0) v = 0;
    if (v > 767) v = 767;
    int offset = v % 256;
    
    // int r = 0;
    // int g = 1;
    // int b = 2;
    
    int r = 2;
    int g = 1;
    int b = 0;
    
    
    if (v<256) {
        ptr[r] = 0; ptr[g] = 0; ptr[b] = offset;
    }
    else if (v<512) {
        ptr[r] = 0; ptr[g] = offset; ptr[b] = 255-offset;
    }
    else {
        ptr[r] = offset; ptr[g] = 255-offset; ptr[b] = 0;
    }
}



int writeImageSerial(char* filename, int width, int height, float *buffer, char* title)
{
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    
    filename[strlen(filename)-1] = '\0';
    
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }
    
    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }
    
    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }
    
    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }
    
    png_init_io(png_ptr, fp);
    
    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    // Set title
    if (title != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }
    
    png_write_info(png_ptr, info_ptr);
    
    // Allocate memory for one row (3 bytes per pixel - RGB)
    row = (png_bytep) malloc(3 * width * sizeof(png_byte));
    
    // Write image data
    int x, y;
    for (y=0 ; y<height ; y++) {
        for (x=0 ; x<width ; x++) {
            setRGB(&(row[x*3]), buffer[y*width + x]);
        }
        png_write_row(png_ptr, row);
    }
    
    // End write
    png_write_end(png_ptr, NULL);
    
finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);
    
    return code;
}


int writeImageParallel(string filename, int width, int height, float *buffer, char* title, int nthreads)
{
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    
    char *pline = new char[filename.size()];
    strcpy ( pline, filename.c_str() );
    
    // Open file for writing (binary mode)
    fp = fopen(pline, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename.c_str());
        code = 1;
        goto finalise;
    }
    
    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }
    
    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }
    
    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }
    
    png_init_io(png_ptr, fp);
    
    
    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    // Set title
    if (title != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }
    
    png_write_info(png_ptr, info_ptr);
    
    //    //    png_structp png_ptr = NULL;
    //    //    png_infop info_ptr = NULL;
    //    //    png_bytep row = NULL;
    //
    //
    //    // Allocate memory for one row (3 bytes per pixel - RGB)
    //    row = (png_bytep) malloc(3 * width * sizeof(png_byte));
    //    //Se podria crear tantas filas auxiliares como threads haya y que cada uno utilice la suya
    //
    //    // Write image data
    //    int x, y;
    //
    //    #pragma omp parallel for private(x,row)
    //    for (y=0 ; y<height ; y++)
    //    {
    //        for (x=0 ; x<width ; x++) {
    //            setRGB(&(row[x*3]), buffer[y*width + x]);
    //        }
    //        png_write_row(png_ptr, row);
    //    }
    
    // End write
    png_write_end(png_ptr, NULL);
    
finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);
    
    return code;
}

void writeThreadWork(int width, int height, float *buffer, png_structp png_ptr)
{
    //    png_structp png_ptr = NULL;
    //    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    
    //png_struct png_ptr_local = *png_ptr; ??// ES necesario hacer una copia de este puntero? Funciona como un puntero de archivo?
    
    
    // Allocate memory for one row (3 bytes per pixel - RGB)
    row = (png_bytep) malloc(3 * width * sizeof(png_byte));
    //Se podria crear tantas filas auxiliares como threads haya y que cada uno utilice la suya
    
    // Write image data
    int x, y;
    
    
    for (y=0 ; y<height ; y++)
    {
        for (x=0 ; x<width ; x++) {
            setRGB(&(row[x*3]), buffer[y*width + x]);
        }
        png_write_row(png_ptr, row);
    }
}

