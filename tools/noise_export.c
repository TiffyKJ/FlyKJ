#include <stdio.h>
#include <stdint.h>

#include "noise.h"

#define OUTPUT_FILE "output/noise.csv"

int main()
{

    FILE *fp =
        fopen(
            OUTPUT_FILE,
            "w");


    if(!fp)
    {
        perror("open output");
        return 1;
    }


    NoiseEstimator estimator;

    noise_init(
            &estimator,
            100);

    fprintf(
        fp,
        "index,power,noise\n");

    uint32_t index = 0;



    /*
     * Stage 1:
     *
     * background noise
     */
    for(int i=0;i<2000;i++)
    {

        uint32_t power = 100;


        noise_update(
                &estimator,
                power);

        fprintf(
            fp,
            "%u,%u,%u\n",
            index++,
            power,
            noise_get(&estimator));
    }




    /*
     * Stage 2:
     *
     * simulated ADS-B burst
     *
     * not a real waveform,
     * just stress test
     */
    for(int i=0;i<300;i++)
    {

        uint32_t power = 20000;


        noise_update(
                &estimator,
                power);

        fprintf(
            fp,
            "%u,%u,%u\n",
            index++,
            power,
            noise_get(&estimator));
    }



    /*
     * Stage 3:
     *
     * return background
     */
    for(int i=0;i<2000;i++)
    {

        uint32_t power = 100;


        noise_update(
                &estimator,
                power);

        fprintf(
            fp,
            "%u,%u,%u\n",
            index++,
            power,
            noise_get(&estimator));
    }



    /*
     * Stage 4:
     *
     * environmental noise increase
     */
    for(int i=0;i<3000;i++)
    {

        uint32_t power = 500;

        noise_update(
                &estimator,
                power);


        fprintf(
            fp,
            "%u,%u,%u\n",
            index++,
            power,
            noise_get(&estimator));
    }

    fclose(fp);


    printf(
        "Noise export finished: %s\n",
        OUTPUT_FILE);

    return 0;
}
