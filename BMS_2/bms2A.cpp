/* 
 * File:   bms1A.cpp
 */

#include <cstdlib>
#include <math.h>

#include "sndfile.cpp"

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

/*
 * 
 */
int main(int argc, char** argv) {

    SndfileHandle outputFile;
    int *buffer = new int[SAMPLE_RATE];

    
    for (int i = 0; i < SAMPLE_RATE; i++)
        buffer [i] = AMPLITUDE * sin(FREQ * 2 * i * M_PI);

    
    outputFile = SndfileHandle("sine.waw", SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    
    outputFile.write(buffer, SAMPLE_RATE);

    delete [] buffer;
    return EXIT_SUCCESS;
}

