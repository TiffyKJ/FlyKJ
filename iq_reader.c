size_t iq_reader_read(
        IQReader *reader,
        IQSample *buffer,
        size_t max_samples)
{
    if (!reader || !reader->file || !buffer)
        return 0;

    size_t count = 0;

    while(count < max_samples)
    {
        uint8_t raw[2];


        if(fread(raw,1,2,reader->file)!=2)
            break;

        buffer[count].i = raw[0];
        buffer[count].q = raw[1];

        count++;
    }

    reader->samples_read += count;

    return count;
}
