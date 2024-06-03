

#include <stdio.h>
#include <omp.h>
#include <time.h>

#define SAMPLE    10
#define CACHE_MIN (1024)
#define CACHE_MAX (16*1024*1024)
#define RTIME     0.1
struct timespec start,finish;
int x[CACHE_MAX];
double sample_ns;
#define TIME_DIF_TO_NS(s,f) \
    ((f.tv_sec-s.tv_sec)*1000000000.0 + (f.tv_nsec-s.tv_nsec))
int main(int argc, char** argv)
{



    /* Print CSV file header */
   // fprintf(fp, "size,stride,ns\n");

    /* Run the timing experiments */
    for (long csize = CACHE_MIN; csize <= CACHE_MAX; csize *= 2){
        for (int stride = 1; stride <= csize/2; stride *= 2){
            double sec0 = 0;
            double sec1 = 0;
            int limit = csize-stride+1;

            /* Time the loop with strided access + loop overhead */
            int steps = 0;
            clock_gettime(CLOCK_REALTIME,&start);
            do {
                for (int i = SAMPLE*stride; i != 0; i--)
                    for (int index = 0; index < limit; index += stride)
                        x[index]++;
                steps++;
                clock_gettime(CLOCK_REALTIME,&finish);
                sample_ns = TIME_DIF_TO_NS(start,finish);
            } while (sec0 < RTIME);

            /* Try to time just the overheads */
            int tsteps=0;
            int temp=0;
            clock_gettime(CLOCK_REALTIME,&start);
            do {
                for (int i = SAMPLE*stride; i != 0; i--)
                    for (int index = 0; index < limit; index += stride)
                        temp += index;
                tsteps++;
                clock_gettime(CLOCK_REALTIME,&finish);
                sample_ns = TIME_DIF_TO_NS(start,finish);
            } while (tsteps < steps);

            /* Report on the average time per read/write */
            double sec            = sec0 - sec1;
            double ns_per_step    = (sec*1.0e9)/steps;
            double reads_per_step = SAMPLE*stride*((limit-1.0)/stride+1.0);

        }
    }


    return 0;
}
