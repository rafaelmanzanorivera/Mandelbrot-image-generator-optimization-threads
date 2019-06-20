# Mandelbrot-image-generator-optimization-threads
Took existing Mandelbrot image generator from https://www-fourier.ujf-grenoble.fr/mobile/~parisse/info/makepng.cc and parallelize it using C++11 threads.

Two parallel modes implemented, static and dynamic.

Sample output (in two processors machine, so speedup won´t raise much adding more than 2 threads)

image,totalTime,mendelbrotTime,writeTime,nThreads,speedUp
"mandelbrot_3162x3162","5.093","3.856","1.236"","1","SERIAL","1"
"mandelbrot_3162x3162","3.444","2.24","1.204"","2","STATIC","1.4788"
"mandelbrot_3162x3162","3.214","1.992","1.222"","2","DYNAMIC","1.58463"
"mandelbrot_3162x3162","3.03","1.838","1.192"","4","STATIC","1.68086"
"mandelbrot_3162x3162","2.543","1.346","1.197"","4","DYNAMIC","2.00275"
"mandelbrot_3162x3162","2.777","1.583","1.194"","6","STATIC","1.83399"
"mandelbrot_3162x3162","2.568","1.365","1.203"","6","DYNAMIC","1.98326"
"mandelbrot_3162x3162","2.778","1.584","1.193"","8","STATIC","1.83333"
"mandelbrot_3162x3162","2.633","1.431","1.202"","8","DYNAMIC","1.9343"
"mandelbrot_3162x3162","2.752","1.555","1.197"","10","STATIC","1.85065"
"mandelbrot_3162x3162","2.598","1.376","1.221"","10","DYNAMIC","1.96035"
"mandelbrot_10488x10488","53.092","42.589","10.502"","1","SERIAL","1"
"mandelbrot_10488x10488","35.736","25.082","10.654"","2","STATIC","1.48567"
"mandelbrot_10488x10488","34.242","22.722","11.519"","2","DYNAMIC","1.55049"
"mandelbrot_10488x10488","31.003","20.509","10.493"","4","STATIC","1.71248"
"mandelbrot_10488x10488","26.695","16.205","10.489"","4","DYNAMIC","1.98884"
"mandelbrot_10488x10488","30.739","18.67","12.068"","6","STATIC","1.72719"
"mandelbrot_10488x10488","27.775","17.315","10.46"","6","DYNAMIC","1.9115"

As each chunk of work has different work load to the CPU even if it has same size (because of the nature of mandelbrot calculation), dinamic division performs best. This is because we divide all work in small chunks (instead of as chunks as number of threads) and each thread performs the calculation of several chunks, so in the end the overall work load ends balanced much better this way. 
