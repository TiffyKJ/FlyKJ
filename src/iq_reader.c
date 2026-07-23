#include "iq_reader.h"

bool iq_reader_open(
        IQReader *reader,
        const char *filepath)
{
    if(!reader || !filepath)
        return false;

    reader->file = NULL;
    reader->samples_read = 0;

    reader->file = fopen(filepath,"rb");

    return reader->file != NULL;
}


size_t iq_reader_read(
        IQReader *reader,
        IQSample *buffer,
        size_t max_samples)
{
    if(!reader ||
       !reader->file ||
       !buffer)
    {
        return 0;
    }

    size_t count = 0;

    uint8_t raw[2];

    while(count < max_samples)
    {

        /*
         * IQ file layout:

              I Q I Q I Q

         */
            
        if(fread(raw,1,2,reader->file)!=2)
        {
            break;
        }

        buffer[count].i = raw[0];
        buffer[count].q = raw[1];


        count++;
    }

    reader->samples_read += count;


    return count;
}

void iq_reader_close(
        IQReader *reader)
{
    if(reader && reader->file)
    {
        fclose(reader->file);

        reader->file = NULL;
    }
}
