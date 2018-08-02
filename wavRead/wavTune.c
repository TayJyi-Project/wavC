/* ref to https://stackoverflow.com/questions/25634377/get-frames-and-samples-of-a-wav-file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdint.h>
#include <assert.h>

struct wavfile
{
    char    id[4];          // should always contain "RIFF"
    int32_t totallength;    // total file length minus 8
    char    wavefmt[8];     // should be "WAVEfmt "
    int32_t format;         // 16 for PCM format
    int16_t pcm;            // 1 for PCM format
    int16_t channels;       // channels
    int32_t frequency;      // sampling frequency
    int32_t bytes_per_second;
    int16_t bytes_by_capture;
    int16_t bits_per_sample;
    char    data[4];        // should always contain "data"
    int32_t bytes_in_data;

} __attribute__((__packed__));



int is_big_endian(void) {
    union 
    {
        uint32_t i;
        char c[4];
    } bint = {0x01000000};
    return bint.c[0]==1;
}

int main(int argc, char **argv) 
{
    char *filename = argv[1];
    FILE *wav = fopen(filename,"rb");

    if (wav == NULL) 
    {
        fprintf(stderr,"Can't open input file %s\n", filename);
        exit(1);
    }

    struct wavfile header;

    // read header
    if (fread(&header, sizeof(header), 1, wav) < 1) 
    {
        fprintf(stderr,"Can't read input file header %s\n", filename);
        exit(1);
    }

    // if wav file isn't the same endianness than the current environment
    // we quit
    if (is_big_endian()) 
    {
        if (memcmp( header.id,"RIFX", 4) != 0 ) 
        {
            fprintf(stderr,"ERROR: %s is not a big endian wav file\n", filename); 
            exit(1);
        }
    } 
    else 
    {
        if (memcmp( header.id,"RIFF", 4) != 0) 
        {
            fprintf(stderr,"ERROR: %s is not a little endian wav file\n", filename); 
            exit(1);
        }
    }

    if (memcmp(header.wavefmt, "WAVEfmt ", 8) != 0 || memcmp( header.data, "data", 4) != 0) 
    {
        fprintf(stderr,"ERROR: Not wav format\n"); 
        exit(1); 
    }

    if (header.format != 16)
    {
        fprintf(stderr,"\nERROR: not 16 bit wav format.");
        exit(1);
    }

    fprintf(stderr,"format: %d bits", header.format);
    
    if (header.format == 16) 
    {
        fprintf(stderr,", PCM");
    } 
    else 
    {
        fprintf(stderr,", not PCM (%d)", header.format);
    }
    if (header.pcm == 1) 
    {
        fprintf(stderr, " uncompressed" );
    } 
    else 
    {
        fprintf(stderr, " compressed" );
    }
    fprintf(stderr,", channel %d", header.pcm);
    fprintf(stderr,", freq %d", header.frequency );
    fprintf(stderr,", %d bytes per sec", header.bytes_per_second );
    fprintf(stderr,", %d bytes by capture", header.bytes_by_capture );
    fprintf(stderr,", %d bits per sample", header.bytes_by_capture );
    fprintf(stderr,"\n" );

    if (memcmp( header.data, "data", 4) != 0)
    { 
        fprintf(stderr,"ERROR: Prrroblem?\n"); 
        exit(1); 
    }

    fprintf(stderr,"wav format\n");

    // read data
    int cnt = 0, i = 0;
    int16_t value;
    fprintf(stderr,"---\n");
    while( fread(&value,sizeof(value),1,wav) ) 
    {
        cnt++;
        printf("%d, ", value);
        value = value > 0 ? value : -value;

    }

    printf("Total bytes in data:\t%d\n", header.bytes_in_data);
    printf("Total Samples in wav:\t%d\n", cnt);
    return 0;
}