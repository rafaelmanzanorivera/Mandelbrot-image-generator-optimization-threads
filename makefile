OBJS	= ListaEnlazada.o main.o mandelbrotCalculators.o writingFunctions.o allInOnePNG.o
SOURCE	= ListaEnlazada.cpp main.cpp mandelbrotCalculators.cpp writingFunctions.cpp allInOnePNG.cpp
HEADER	= ListaEnlazada.h PNGParallel.h mandelbrotCalculators.h pngpriv.h writingFunctions.h
OUT	= out
CC	 = g++
FLAGS	 = -g -c -Wall  -std=c++11
#LFLAGS	 = -pthread -std=c++11 -lpng16 -L/usr/local/Cellar/imagemagick/7.0.8-32/lib -llibMagick++-7.Q16HDRI -llibMagickCore-7.Q16HDRI.a -llibMagickWand-7.Q16HDRI.a -L/usr/local/Cellar/zlib/1.2.11/lib/ -llibz.a -L/usr/local/Cellar/imagemagick/7.0.8-32/lib/ -llibMagickWand-7.Q16HDRI.a -L/usr/local/Cellar/boost/1.68.0_1/lib/ -llibboost_program_options.a -L/usr/local/Cellar/libpng/1.6.35/lib -llibpng16.a -L/usr/local/Cellar/llvm/7.0.0/lib/ -llibomp.dylib

#LFLAGS	 = -L./lib -pthread -std=c++11 -lpng16

LFLAGS = -L/usr/local/lib -L/usr/local/Cellar/libpng/1.6.35/lib -L/usr/local/Cellar/llvm/7.0.0/lib -L/usr/local/include -L/usr/local/lib -L/usr/local/Cellar/libmagic/5.36/lib -L/usr/local/Cellar/boost/1.68.0_1/lib -L/usr/local/Cellar/imagemagick/7.0.8-32/lib -L/usr/local/Cellar/zlib/1.2.11/lib -stdlib=libc++ -lMagickCore-7.Q16HDRI -lMagick++-7.Q16HDRI -lz -lMagickWand-7.Q16HDRI -lboost_program_options -lomp -lpng16 -std=c++11

INCLUDES= -I/usr/local/include/

# -g option enables debugging mode 
# -c flag generates object code for separate file

#for i in $(ls *.cpp); 
#do g++ -I/usr/local/Cellar/libpng/1.6.35/include/ -I/usr/local/Cellar/gcc/8.2.0/lib/gcc/8/gcc/x86_64-apple-darwin18.0.0/8.2.0/include/ -c $i -o $i.o -fopenmp ; done ; g++ *.o -lpthread -o main -lpng16 -fopenmp


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) 


# create/compile the individual files >>separately<<
ListaEnlazada.o: ListaEnlazada.cpp
	$(CC) $(FLAGS) $(INCLUDES) ListaEnlazada.cpp 

main.o: main.cpp
	$(CC) $(FLAGS) $(INCLUDES) main.cpp 

mainPNG.o: mainPNG.cpp
	$(CC) $(FLAGS) $(INCLUDES) mainPNG.cpp 

mandelbrotCalculators.o: mandelbrotCalculators.cpp
	$(CC) $(FLAGS) $(INCLUDES) mandelbrotCalculators.cpp 

writingFunctions.o: writingFunctions.cpp
	$(CC) $(FLAGS) $(INCLUDES) writingFunctions.cpp 


# clean house
clean:
	rm -f $(OBJS) $(OUT)



