#include <stdio.h>
#include <stdint.h>

#include "iq_reader.h"
#include "power.h"

#define BUFFER_SIZE 4096

/*IQ file
*
*   ↓
*
*   Power
*
*     ↓
*
*Binary uint32_t file
*/

int main(int argc,char *argv[])
{

    const char *input =
        argc > 1 ?
        argv[1]:
        "sample/test.iq";


    const char *output =
        argc > 2 ?
        argv[2]:
        "power.bin";




    IQReader reader;


    if(!iq_reader_open(&reader,input))
    {
        fprintf(stderr,
                "Cannot open IQ file\n");

        return 1;
    }





    FILE *fp =
        fopen(output,"wb");


    if(!fp)
    {
        fprintf(stderr,
                "Cannot create output\n");


        iq_reader_close(&reader);

        return 1;
    }



    /*
     * Large output buffer
     */
    char write_buffer[65536];

    setvbuf(
        fp,
        write_buffer,
        _IOFBF,
        sizeof(write_buffer)
    );




    IQSample samples[BUFFER_SIZE];

    uint32_t power[BUFFER_SIZE];

    uint64_t total = 0;

    while(1)
    {

        size_t count =
            iq_reader_read(
                &reader,
                samples,
                BUFFER_SIZE
            );



        if(count==0)
            break;

        power_calc_buffer(
                samples,
                power,
                count
        );

        fwrite(
            power,
            sizeof(uint32_t),
            count,
            fp
        );

        total += count;
    }

    fclose(fp);

    iq_reader_close(&reader);

    printf(
        "Exported %llu samples\n",
        (unsigned long long)total
    );


    return 0;
}
