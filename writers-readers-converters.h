//
// Created by User on 21.04.2024.
//

#include <fstream>
#include <vector>
#include <cmath>

#ifndef RICE_AUDIO_WRITERS_READERS_CONVERTERS_H
#define RICE_AUDIO_WRITERS_READERS_CONVERTERS_H

#endif //RICE_AUDIO_WRITERS_READERS_CONVERTERS_H


template <typename T>
std::vector<short> convertTo16bit(std::vector<T> samples){
    int size = samples.size();
    std::vector<short>  converted;
    converted.reserve(size);
    for(auto sample:samples){
        converted.push_back(round(sample * pow(2,15)));
    }
    return converted;
}


template <typename T>
std::vector<unsigned short> convertToUnsigned(std::vector<T> samples){
    int size = samples.size();
    std::vector<unsigned short> unsignedSamples;
    unsignedSamples.reserve(size);

    for(auto sample: samples){
        unsignedSamples.push_back(sample >=0 ? sample*2 : sample *(-2) -1);
    }
    return unsignedSamples;
}


std::vector<short> convertToSigned(std::vector<unsigned short>  unsignedSamples){
    unsigned int size = unsignedSamples.size();
    std::vector<short> signedSamples;
    signedSamples.reserve(size);

    for(auto sample:unsignedSamples){
        signedSamples.push_back((sample & 1) ? -(sample+1)/2 : sample/2);
    }

    return signedSamples;
}



void writeBit(std::ofstream& file, bool bit){
    static unsigned char currentByte = 0;
    static char bitPos = 0;

    if(bit){
        currentByte |= (1 << bitPos);
    }

    bitPos++;
    if(bitPos == 8){
        file.put(currentByte);
        currentByte = 0;
        bitPos = 0;
    }
}


void writeUnary(std::ofstream& stream, unsigned int u){
    for(unsigned int i = 0; i<u;i++){
        writeBit(stream,false);
    }
    writeBit(stream,true);
}


void writeBinary(std::ofstream& stream, unsigned int v, unsigned int bits){
    unsigned int bitPos = (1<<(bits-1));
    bool bit;
    for(int i = 0; i < bits; i++){
        bit = (v&bitPos);
        writeBit(stream, bit);
        bitPos = bitPos >> 1;
    }
}


//return: 2 - eof; 0 - zero binary; 1 - one binary
char readBit(std::istream& stream){
    static unsigned char currentByte = 0;
    static char bitPos = 0;

    if(bitPos==0){
        currentByte = stream.get();
        if(stream.eof()){
            return 2;
        }
    }
    char bit = (currentByte>>bitPos)&1;

    bitPos++;
    if(bitPos>=8) bitPos = 0;

    return bit;

}