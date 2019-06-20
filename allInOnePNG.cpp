////
////  allInOnePNG.cpp
////  mendelbrot
////
////  Created by Rafael Manzano on 15/03/2019.
////  Copyright © 2019 Rafael Manzano. All rights reserved.
////
//#include <iostream>
//#include <iterator>
//#include <fstream>
//#include <cstring>
//#include <Magick++/Image.h>
//#include <Magick++/Pixels.h>
//#include <png.h>
//#include "pngpriv.h"
//#include <time.h>
//#include <omp.h>
//#include <zlib.h>
//#include <math.h>
//#include <boost/program_options.hpp>
//
//#define COLOR_FORMAT_BPP 4
//
//#ifndef COLOR_FORMAT_BPP
//#if QuantumDepth==8
//#define COLOR_FORMAT_BPP 4
//#elif QuantumDepth==16
//#define COLOR_FORMAT_BPP 8
//#endif
//#endif
//
//
//using namespace std;
//using namespace Magick;
//namespace po = boost::program_options;
///**
// * Function used internally in libpng to write to an output file
// *
// * @param    pngPtr
// * @param    data
// * @param    length
// */
//
//void pngWrite(png_structp pngPtr, png_bytep data, png_size_t length) {
//    ofstream* file = reinterpret_cast<ofstream*>(png_get_io_ptr(pngPtr));
//    file->write(reinterpret_cast<char*>(data), length);
//    if (file->bad()) {
//        cout << reinterpret_cast<char*>(data) << endl << endl << length << endl << file->badbit << endl;
//        png_error(pngPtr, "Write error");
//    }
//}
//
//
///**
// * Function used internally in libpng to flush the output buffer to a file
// *
// * @param    pngPtr
// */
//
//void pngFlush(png_structp pngPtr) {
//    ofstream* file = reinterpret_cast<ofstream*>(png_get_io_ptr(pngPtr));
//    file->flush();
//}
//
//
//
//
//
//class PNGParallel {
//public:
//
//    PNGParallel(Image &inputFile):InputFile(&inputFile) {
//        
//        //default settings
//        CompressionLevel = 9;
//        NumThreads = 2;
//        
//    }
//
//    void setNumThreads(int numThreads) {
//        NumThreads = numThreads;
//        omp_set_num_threads(numThreads);
//    }
//    
//    void setCompressionLevel(int compressionLevel) {
//        CompressionLevel = compressionLevel;
//    }
//
//    void compress(ofstream &outputFile)
//    {
//        
//        //Init PNG write struct
//        png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//        if (!pngPtr) {
//            cout << "PNG write struct could not be initialized" << endl;
//            return;
//        }
//        
//        //Init PNG info struct
//        png_infop infoPtr = png_create_info_struct(pngPtr);
//        if (!infoPtr) {
//            png_destroy_write_struct(&pngPtr, (png_infopp) NULL);
//            cout << "PNG info struct could not be initialized" << endl;
//            return;
//        }
//        
//        //Tell the pnglib where to save the image
//        png_set_write_fn(pngPtr, &outputFile, pngWrite, pngFlush);
//        
//        //Change from RGB to BGR as Magick++ uses this format
//        png_set_bgr(pngPtr);
//        
//        //For the sake of simplicity we do not apply any filters to a scanline
//        png_set_filter(pngPtr, 0, PNG_FILTER_NONE);
//        
//        //Write IHDR chunk
//        Geometry ig = this->InputFile->size();
//        int height = ig.height();
//        int width = ig.width();
//        png_set_IHDR(pngPtr, infoPtr, width, height, 16, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
//                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_BASE);
//        
//        //Write the file header information.
//        png_write_info(pngPtr, infoPtr);
//        
//        //Init vars used for compression
//        int totalDeflateOutputSize = 0;
//        uint32_t adler32Combined = 0L;
//        z_stream zStreams[NumThreads];
//        size_t deflateOutputSize[NumThreads];
//        char *deflateOutput[NumThreads];
//        size_t startRow[NumThreads];
//        size_t endRow[NumThreads];
//        uint32_t adlerSums[NumThreads];
//        MagickCore::PixelPacket *filteredData[NumThreads];
//        
//#pragma omp parallel for default(shared)
//        for (int threadNum = 0; threadNum < NumThreads; threadNum++) //Sust paquetes trabajo
//        {
//            int row, stopAtRow;
//            
//            //Calculate which lines have to be handled by this thread
//            row = threadNum * static_cast<int>(ceil(static_cast<double>(height) / static_cast<double>(NumThreads)));
//            stopAtRow = static_cast<int>(ceil(static_cast<double>(height) / static_cast<double>(NumThreads))) * (threadNum + 1);
//            stopAtRow = stopAtRow > height ? height : stopAtRow;
//            startRow[threadNum] = row;
//            endRow[threadNum] = stopAtRow;
//            
//            //Load all pixel data
//            MagickCore::PixelPacket* pixels = (MagickCore::PixelPacket*)InputFile->getPixels(0, row, width, stopAtRow - row);
//            
//            //Add the filter byte and reorder the RGB values
//            MagickCore::PixelPacket *filteredRows = this->filterRows(pixels, stopAtRow - row, width);
//            filteredData[threadNum] = filteredRows;
//        }
//        
//#pragma omp parallel for default(shared)
//        for (int threadNum = 0; threadNum < NumThreads; threadNum++)
//        {
//            
//            int ret, flush, row, stopAtRow;
//            unsigned int have;
//            const int chunkSize = 16384;
//            unsigned char output_buffer[chunkSize];
//            
//            row = startRow[threadNum];
//            stopAtRow = endRow[threadNum];
//            
//            FILE *deflate_stream = open_memstream(&deflateOutput[threadNum], &deflateOutputSize[threadNum]);
//            
//            //Allocate deflate state
//            zStreams[threadNum].zalloc = Z_NULL;
//            zStreams[threadNum].zfree = Z_NULL;
//            zStreams[threadNum].opaque = Z_NULL;
//            if (deflateInit(&zStreams[threadNum], 9) != Z_OK)
//            {
//                cout << "Not enough memory for compression" << endl;
//            }
//            
//            // Initialize dictionary with the last 32kb of the previous chunk
//            if (threadNum > 0) //dependencia con el bloque anterior si no soy el primer thread
//            {
//                int lastThread = threadNum - 1;
//                size_t maxDict = 32768;
//                size_t lastChunkSize = (endRow[lastThread] - startRow[lastThread]) * (COLOR_FORMAT_BPP * width + 1);
//                Bytef* lastData = reinterpret_cast<Bytef*>(filteredData[lastThread]);
//                size_t dictSize = (lastChunkSize > maxDict) ? maxDict : lastChunkSize;
//                deflateSetDictionary(&zStreams[threadNum],
//                                     lastData + lastChunkSize - dictSize,
//                                     dictSize);
//            }
//            
//            //Let's compress line by line so the input buffer is the number of bytes of one pixel row plus the filter byte
//            zStreams[threadNum].avail_in = (COLOR_FORMAT_BPP * width + 1) * (stopAtRow - row);
//            //zStreams[threadNum].avail_in += stopAtRow == height ? 0 : 1;
//            
//            //Finish the stream if it's the last pixel row
//            flush = stopAtRow == height ? Z_FINISH : Z_SYNC_FLUSH;
//            zStreams[threadNum].next_in = reinterpret_cast<Bytef*>(filteredData[threadNum]);
//            zStreams[threadNum].adler = adler32(0L, NULL, 0);
//            
//            //Compress the image data with deflate
//            do {
//                zStreams[threadNum].avail_out = chunkSize;
//                zStreams[threadNum].next_out = output_buffer;
//                ret = deflate(&zStreams[threadNum], flush);
//                have = chunkSize - zStreams[threadNum].avail_out;
//                fwrite(&output_buffer, 1, have, deflate_stream);
//            } while (zStreams[threadNum].avail_out == 0);
//            
//            fclose(deflate_stream);
//            
//            adlerSums[threadNum] = zStreams[threadNum].adler;
//            
//            //Finish deflate process
//            (void) deflateEnd(&zStreams[threadNum]);
//        }
//        
//        // These must run in order for safety
//        adler32Combined = adler32(0L, NULL, 0);
//        for (int threadNum = 0; threadNum < NumThreads; threadNum++) {
//            totalDeflateOutputSize += deflateOutputSize[threadNum];
//            
//            //Calculate the combined adler32 checksum
//            int input_length = (endRow[threadNum] - startRow[threadNum]) * (COLOR_FORMAT_BPP * width + 1);
//            adler32Combined = adler32_combine(adler32Combined, adlerSums[threadNum], input_length);
//        }
//        
//        //Concatenate the zStreams
//        png_byte *idatData = new png_byte[totalDeflateOutputSize];
//        for (int i = 0; i < NumThreads; i++) {
//            if(i == 0) {
//                memcpy(idatData, deflateOutput[i], deflateOutputSize[i]);
//                idatData += deflateOutputSize[i];
//            } else {
//                //strip the zlib stream header
//                // 2 bytes regular
//                // 4 more for the "dictionary"
//                const size_t hdr = 2 + 4;
//                memcpy(idatData, deflateOutput[i] + hdr, deflateOutputSize[i] - hdr);
//                idatData += (deflateOutputSize[i] - hdr);
//                totalDeflateOutputSize -= hdr;
//            }
//        }
//        
//        //Add the combined adler32 checksum
//        //idatData -= sizeof(adler32Combined);
//        //memcpy(idatData, &adler32Combined, sizeof(adler32Combined));
//        //idatData -= (totalDeflateOutputSize - sizeof(adler32Combined));
//        idatData -= 4;
//        *idatData++ = (adler32Combined >> 24) & 0xff;
//        *idatData++ = (adler32Combined >> 16) & 0xff;
//        *idatData++ = (adler32Combined >> 8) & 0xff;
//        *idatData++ = (adler32Combined >> 0) & 0xff;
//        idatData -= (totalDeflateOutputSize);
//        
//        //We have to tell libpng that an IDAT was written to the file
//        pngPtr->mode |= PNG_HAVE_IDAT;
//        
//        //Create an IDAT chunk
//        png_unknown_chunk idatChunks[1];
//        strcpy((png_charp) idatChunks[0].name, "IDAT");
//        idatChunks[0].data = idatData;
//        idatChunks[0].size = totalDeflateOutputSize;
//        idatChunks[0].location = PNG_AFTER_IDAT;
//        //pngPtr->flags |= 0x10000L; //PNG_FLAG_KEEP_UNSAFE_CHUNKS
//        png_set_keep_unknown_chunks(pngPtr, 3, idatChunks[0].name, 1);
//        png_set_unknown_chunks(pngPtr, infoPtr, idatChunks, 1);
//        png_set_unknown_chunk_location(pngPtr, infoPtr, 0, PNG_AFTER_IDAT);
//        
//        //Write the rest of the file
//        png_write_end(pngPtr, infoPtr);
//        
//        //Cleanup
//        png_destroy_write_struct(&pngPtr, &infoPtr);
//        delete(idatData);
//        
//        
//    }
//    ~PNGParallel() {
//        
//    }
//    
//private:
//
//    MagickCore::PixelPacket* filterRow(MagickCore::PixelPacket* pixels, int rowLength)
//    {
//        
//        unsigned short colorTmp;
//        char filterByte = 0;
//        
//        //Transformations
//        for (int j = 0; j < rowLength; j++) {
//            //swap red and blue in BGRA mode
//            colorTmp = pixels->red;
//            pixels->red = pixels->blue;
//            pixels->blue = colorTmp;
//            
//            //change opacity
//            //pixels->opacity = TransparentOpacity;
//            pixels += 1;
//        }
//        pixels -= rowLength;
//        
//        //Add filter byte 0 to disable row filtering
//        MagickCore::PixelPacket* filteredRow = reinterpret_cast<MagickCore::PixelPacket*>(malloc(rowLength * COLOR_FORMAT_BPP + 1));
//        memcpy(&(reinterpret_cast<char*>(filteredRow)[0]), &filterByte, 1);
//        memcpy(&(reinterpret_cast<char*>(filteredRow)[1]), pixels, COLOR_FORMAT_BPP * rowLength);
//        
//        return filteredRow;
//        
//    }
//    
//    
//    /**
//     * Filter all pixel rows to optimize the deflate compression
//     * Note: Filtering is disabled in this implementation.
//     *
//     * @param    pixels        pointer to the pixels in memory
//     * @param    rowLength    amount of pixels per row
//     * @return    filtered rows
//     */
//    
//    MagickCore::PixelPacket* filterRows(MagickCore::PixelPacket* pixels, int rows, int rowLength)
//    {
//        
//        int bytesPerRow = (COLOR_FORMAT_BPP * rowLength + 1);
//        MagickCore::PixelPacket* filteredRows = reinterpret_cast<MagickCore::PixelPacket*>(malloc(rows * bytesPerRow));
//        MagickCore::PixelPacket* filteredRow;
//        
//        for(int row = 0; row < rows; row++) {
//            filteredRow = this->filterRow(pixels, rowLength);
//            memcpy(reinterpret_cast<char*>(filteredRows), filteredRow, bytesPerRow);
//            pixels += rowLength;
//            filteredRows = reinterpret_cast<MagickCore::PixelPacket*>(reinterpret_cast<char*>(filteredRows) + bytesPerRow);
//        }
//        
//        filteredRows = reinterpret_cast<MagickCore::PixelPacket*>(reinterpret_cast<char*>(filteredRows) - (bytesPerRow * rows));
//        
//        return filteredRows;
//        
//    }
//    
//    int CompressionLevel;
//    Image *InputFile;
//    int NumThreads;
//};
//
//
//int main(int argc, char *argv[]) {
//    
//    Image inputFile;
//    ofstream outputFile;
//    int numThreads = 8;
//    
//    try {
//        
//        //Command line argument handling
//        po::options_description desc("Allowed options");
//        desc.add_options()
//        ("help", "produce help message")
//        ("input,i", po::value<string>(), "path to input file")
//        ("output,o", po::value<string>(), "path to output file")
//        ("num_threads,nt", po::value<int>()->default_value(numThreads), "number of threads used for the compression");
//        
//        po::variables_map vm;
//        po::store(po::parse_command_line(argc, argv, desc), vm);
//        po::notify(vm);
//        
//        if (vm.count("help")) {
//            cout << desc << "\n";
//            return 1;
//        }
//        
//        if (vm.count("input")) {
//            inputFile.read(vm["input"].as<string>());
//        } else {
//            cout << "Input file was not set.\n";
//            return 1;
//        }
//        
//        if (vm.count("output")) {
//            outputFile.open(vm["output"].as<string>().c_str());
//            if (!outputFile.good()) {
//                cout << "Output file could not be opened" << endl;
//                return 1;
//            }
//        } else {
//            cout << "Output file was not set.\n";
//            return 1;
//        }
//        
//        if (vm.count("num_threads")) {
//            numThreads = vm["num_threads"].as<int>();
//        }
//        
//        //Encode the file if everything is okay
//        if (inputFile.isValid() && outputFile.good()) {
//            
//            PNGParallel pngParallel(inputFile);
//            pngParallel.setCompressionLevel(9);
//            pngParallel.setNumThreads(numThreads);
//            pngParallel.compress(outputFile);
//            
//        }
//        outputFile.close();
//        
//    } catch (exception& e) {
//        cerr << "error: " << e.what() << "\n";
//        return 1;
//    } catch (...) {
//        cerr << "Exception of unknown type!\n";
//    }
//    
//    return 0;
//    
//}
