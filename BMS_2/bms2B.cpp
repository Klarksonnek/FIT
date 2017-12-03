/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>

#include "sndfile.hh"


using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    SndfileHandle inputFile;
    int sampleRate;
    int *buffer;
    
    inputFile = SndfileHandle("sine.waw");
    
    sampleRate = inputFile.samplerate();
    
    buffer = new int[sampleRate];

    inputFile.read(buffer, sampleRate);
    
    
    delete [] buffer;
    return EXIT_SUCCESS;
}

