#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> 
#include <assert.h>


/* WAV file header format struct*/

typedef struct wavfile
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

} __attribute__((__packed__)) wavfile;


/* Make sure the WAV is valid on current ENV
    Return TRUE if it's big endian*/
int is_big_endian(void);


/* Return the number of samples of given WAV file */
int get_number_of_samples(char *path2file);


/* Read the samples from WAV 
    Return a pointer to the sample array */
int *get_array_of_samples(char *path2file);

/* Given a pointer to array of samples, 
    the number of the part to divide, 
    the n-th (1~num_of_part) part to be tuned,
    and the bias to apply on the selected block of samples */
void tune_the_wav(int num_of_samples, int *samples, int num_of_part, int n_th_part, int bias);


/* Export the tuned result to $path2file 
    $path2file must be identical as the source, it will overwrite the data part */
void export_new_wav(int *samples, int num_of_samples, char *src, char *tar);


// Tutorial for little Tsung
int main()
{
    char file[] = "storage/test.wav";
    // 先透過 get_number_of_samples() 得到 WAV 的總 sample 數
    int numSamples = get_number_of_samples(file);
    // 透過 get_array_of_samples() 得到 sample 的 int 數值，回傳 pointer to int
    int *samples = get_array_of_samples(file);
    tune_the_wav(numSamples, samples, 2, 1, -100000);
    export_new_wav(samples, numSamples, file, "storage/converted.wav");
    // 記得 free 掉
    free(samples);
    return 0;
}


int is_big_endian(void) 
{

    union 
    {
        uint32_t i;
        char c[4];
    } bint = {0x01000000};

    return bint.c[0] == 1;
}



int get_number_of_samples(char *path2file)
{
     FILE *wav = fopen(path2file,"rb");

    if(wav == NULL) 
    {
        fprintf(stderr,"Can't open file %s\n", path2file);
        exit(1);
    }

    struct wavfile header;

    // read header
    if (fread(&header, sizeof(header), 1, wav) < 1) 
    {
        fprintf(stderr,"Can't read file header %s\n", path2file);
        exit(1);
    }

    // if wav file isn't the same endianness than the current environment, then quit
    if (is_big_endian()) 
    {
        if (memcmp(header.id,"RIFX", 4) != 0 ) 
        {
            fprintf(stderr,"ERROR: %s is not a big endian wav file\n", path2file); 
            exit(1);
        }
    } 
    else 
    {
        if (memcmp( header.id,"RIFF", 4) != 0) 
        {
            fprintf(stderr,"ERROR: %s is not a little endian wav file\n", path2file); 
            exit(1);
        }
    }
    fclose(wav);
    return header.bytes_in_data / (header.format / 8);

}




int *get_array_of_samples(char *path2file)
{
    FILE *wav = fopen(path2file,"rb");

    if(wav == NULL) 
    {
        fprintf(stderr,"Can't open file %s\n", path2file);
        exit(1);
    }

    struct wavfile header;

    // read header
    if (fread(&header, sizeof(header), 1, wav) < 1) 
    {
        fprintf(stderr,"Can't read file header %s\n", path2file);
        exit(1);
    }

    // if wav file isn't the same endianness than the current environment, then quit
    if (is_big_endian()) 
    {
        if (memcmp(header.id,"RIFX", 4) != 0 ) 
        {
            fprintf(stderr,"ERROR: %s is not a big endian wav file\n", path2file); 
            exit(1);
        }
    } 
    else 
    {
        if (memcmp( header.id,"RIFF", 4) != 0) 
        {
            fprintf(stderr,"ERROR: %s is not a little endian wav file\n", path2file); 
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

    if (memcmp(header.data, "data", 4) != 0)
    { 
        fprintf(stderr,"ERROR: WTF?\n"); 
        exit(1); 
    }

    fprintf(stderr,"WAV format\n");
    fprintf(stderr,"---\n\n");
    // read data
    
    int16_t temp;
    int num_of_samples = header.bytes_in_data / (header.format / 8), _i = 0;
    int *ret_sample_array = (int *) malloc(sizeof(int) * num_of_samples);
    assert(ret_sample_array != NULL);

    while(fread(&temp, sizeof(temp), 1, wav)) 
    {
        ret_sample_array[_i++] = (int) temp;
        // printf("%d, ", ret_sample_array[_i-1]);
    }
    fclose(wav);
    return ret_sample_array;
}



void tune_the_wav(int num_of_samples, int *samples, int num_of_part, int n_th_part, int bias)
{
    int base = num_of_samples / num_of_part;
    int start_idx = (n_th_part - 1) * base, _i, end_idx = start_idx + base;
    for(_i = start_idx; _i < end_idx; _i++)
    {
        samples[_i] += bias;
    }
    fprintf(stderr, "Tuning is done\n");
    return;
}



void export_new_wav(int *samples, int num_of_samples, char *src, char *tar)
{
    FILE *fpread = fopen(src, "rb");
    assert(fpread != NULL);
    wavfile tempHeader;
    int ret = fread(&tempHeader, sizeof(wavfile), 1, fpread), i;
    fclose(fpread);
    assert(ret >= 1);
    FILE *fpwrite = fopen(tar, "wb+");
    assert(fpwrite != NULL);
    fwrite(&tempHeader, sizeof(wavfile), 1, fpwrite);
    int16_t temp;
    for(i = 0; i < num_of_samples; i++)
    {
        temp = (int16_t) samples[i];
        fwrite(&temp, sizeof(int16_t), 1, fpwrite);
    }
    fclose(fpwrite);
    return;
}