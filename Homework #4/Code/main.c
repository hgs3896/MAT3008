#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#define NRANSI
#include "nr.h"
#define NUM_INTERVALS 100

float uniform_dist(float a, float b, long* seed){
    return a + (b-a) * ran1(seed);
}

float gaussian_dist(float m, float s, long* seed){
    return m + s * gasdev(seed);
}

static const int n_samples[4] = {100, 1000, 10000, 100000};
long hist[NUM_INTERVALS];

int main(int argc, char const *argv[])
{
    // Get the seed from the system clock
    const long seed = time(NULL);

    /* 1. Uniform Distribution in [-3, 2] */
    const float a = -3, b = 2;
    float r;
    long seed1 = seed;
    for(int i = 0;i < sizeof(n_samples)/sizeof(n_samples[0]); ++i){
        memset(hist, 0, sizeof(hist));
        for(int rep = 0;rep < n_samples[i];++rep){
            r = uniform_dist(a, b, &seed1);
            hist[(int)((r - a)/(b - a) * NUM_INTERVALS)] += 1;
        }
        printf("Histogram of Uniform Distribution in [-3, 2] with %d samples\n", n_samples[i]);
        for(int j = 0;j < NUM_INTERVALS;++j){
            if(j%10 == 0) printf("%+3.2f ~ %+3.2f\t", a+(b-a)*j/NUM_INTERVALS, a+(b-a)*(j+9)/NUM_INTERVALS);
            printf("%4ld ", hist[j]);
            if(j%10 == 9) printf("\n");
        }
        printf("\n");
    }

    /* 2. Gaussian Distrubution with m = 0.5, standard s = 1.5 */
    const float m = 0.5, s = 1.5;
    long seed2 = seed;
    for(int i = 0;i < sizeof(n_samples)/sizeof(n_samples[0]); ++i){
        memset(hist, 0, sizeof(hist));
        for(int rep = 0;rep < n_samples[i];++rep){
            r = gaussian_dist(m, s, &seed2);
            int bucket = ((r - m)/(6*s) + 0.5) * NUM_INTERVALS;
            bucket = bucket < 0 ? 0 : bucket >= NUM_INTERVALS ? NUM_INTERVALS - 1 : bucket;
            hist[bucket] += 1;
        }
        printf("Histogram of Gaussian Distribution with m = 0.5, standard s = 1.5 with %d samples\n", n_samples[i]);
        for(int j = 0;j < NUM_INTERVALS;++j){
            if(j%10 == 0) printf("%+3.2f ~ %+3.2f\t", m + (6*s) * ((float)j/NUM_INTERVALS - 0.5), m + (6*s) * ((float)(j+9)/NUM_INTERVALS - 0.5));
            printf("%4ld ", hist[j]);
            if(j%10 == 9) printf("\n");
        }
        printf("\n");
    }


    return 0;
}

#undef NRANSI
