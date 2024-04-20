//
// Created by User on 21.04.2024.
//

#ifndef RICE_AUDIO_ENCODERS_DECODERS_H
#define RICE_AUDIO_ENCODERS_DECODERS_H

#endif //RICE_AUDIO_ENCODERS_DECODERS_H

#include <vector>
#include <string>
#include "writers-readers-converters.h"
#include "stats.h"


template <typename T>
std::vector<T> encodeDifferential(std::vector<T> samples){
    int size = samples.size();
    std::vector<T> encoded;
    encoded.reserve(size);;


    encoded.push_back(samples.front());
    for(int i = 1; i<size;i++){
        encoded.push_back(samples[i]-samples[i-1]);
    }


    return encoded;
}

template <typename T>
std::vector<T> decodeDifferential(std::vector<T> encodedSamples){
    int size = encodedSamples.size();
    std::vector<T> decoded;
    decoded.reserve(size);

    decoded.push_back(encodedSamples.front());
    for(int i = 1; i<size; i++){
        decoded.push_back(encodedSamples[i]+decoded[i-1]);
    }

    return decoded;
}


template <typename T>
unsigned long long encodeRice(std::vector<T> samples, const std::string& name){
    std::ofstream outFile(".//datFiles//"+name+".dat", std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 0;
    }

    unsigned long long bitsLength = 0;
    auto k = kValueRice(findDistributionRice(samples));
    unsigned long long originalSize = samples.size();

    if(k>15) k = 15;
    outFile.write(reinterpret_cast<const char *> (&originalSize),sizeof(originalSize));
    outFile.put(k);

    auto twoPowk = pow(2,k);
    unsigned int u = 0;
    unsigned int v = 0;

    for(auto sample:samples){
        u = floor(sample/twoPowk);
        writeUnary(outFile,u);
        v = sample - u*twoPowk;
        writeBinary(outFile, v,k);
        bitsLength+=u+k+1;

    }

    char bitsNeeded =(char) (bitsLength%8);
    for(char i = 0; i<bitsNeeded; i++){
        writeBit(outFile,false);
    }
    outFile.close();

    return bitsLength;

}


std::vector<unsigned short > decodeRice(const std::string & fileName){
    std::ifstream inFile(".//datFiles//"+fileName+".dat", std::ios::binary);

    if (!inFile.is_open()) {
        throw std::ios_base::failure("Error opening file for reading.");
    }

    unsigned long long allocationSize;

    inFile.read(reinterpret_cast<char *>(&allocationSize),sizeof(allocationSize));
    char k;
    inFile.read(reinterpret_cast<char *>(&k),sizeof(k));

    std::vector<unsigned short> decoded;
    decoded.reserve(allocationSize);

    unsigned int u = 0;
    unsigned int v = 0;
    char bit;
    char vBitPos;
    unsigned short n;

    while(true) {
        u = 0;
        bit = readBit(inFile);
        while(bit == 0){
            u++;

            bit = readBit(inFile);
        }
        if(bit == 2){
            break;
        }
        vBitPos = k-1;
        v = 0;
        for(char i = 0; i<k;i++){
            bit = readBit(inFile);
            v |= (bit<<vBitPos);
            vBitPos--;
        }
        n = u * pow(2, k) + v;
        decoded.push_back(n);
    }

    return decoded;
}
