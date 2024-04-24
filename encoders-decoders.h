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
std::vector<std::vector<T>> encodeDifferential(std::vector<std::vector<T>> samples){
    int size = samples[0].size();
    std::vector<std::vector<T>> encoded{};
    encoded[0] = std::vector<T>();
    encoded[1] = std::vector<T>();


    for(int channel = 0; channel < samples.size(); channel++){
        encoded[channel].reserve(size);
        encoded[channel].push_back(samples[channel].front());
        for(int sample = 1; sample < size; sample++){
            encoded[channel].push_back(samples[channel][sample] - samples[channel][sample - 1]);
        }
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
std::vector<std::vector<T>> decodeDifferential(std::vector<std::vector<T>> encodedSamples){
    int size = encodedSamples[0].size();
    std::vector<std::vector<T>>  decoded;
    decoded.reserve(encodedSamples.size());
    decoded[0].reserve(size);
    decoded[1].reserve(size);

    for(int channel = 0; channel < 2; channel++){
        decoded[channel].push_back(encodedSamples[channel].front());
        for(int sample = 1; sample < size; sample++){
            decoded[channel].push_back(encodedSamples[channel][sample] + decoded[channel][sample - 1]);
        }
    }

    return decoded;
}


template <typename T>
unsigned long long encodeRice(std::vector<T> samples,const std::string & name){
    std::ofstream outFile(".//datFiles//"+name+".dat", std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 0;
    }


    auto k = kValueRice(findDistributionRice(samples));
    unsigned long long originalSize = samples.size();

    if(k>15) k = 15;
    outFile.write(reinterpret_cast<const char *> (&originalSize),sizeof(originalSize));
    outFile.put(k);
    unsigned long long bitsLength =0;

    auto twoPowk = pow(2,k);
    unsigned int u = 0;
    unsigned int v = 0;

    for(auto sample:samples){
        u = floor(sample/twoPowk);
        writeUnary(outFile,u);
        if (k != 0){
            v = sample - u*twoPowk;
            writeBinary(outFile, v,k);
        }
        bitsLength+=u+k+1;

    }
    writeBit(outFile, false,true);
    outFile.close();

    return bitsLength;

}


template <typename T>
unsigned long long encodeRice(std::vector<std::vector<T>> samples,const std::string & name){
    std::ofstream outFile(".//datFiles//"+name+".dat", std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 0;
    }


    auto kLeft = kValueRice(findDistributionRice(samples[0]));
    auto kRight = kValueRice(findDistributionRice(samples[1]));
    unsigned long long originalSize = samples[0].size();

    if(kLeft > 15) kLeft = 15;
    if(kRight > 15) kRight = 15;
    unsigned char k = ((kLeft&0xF)<<4) + (kRight&0xF);

    outFile.write(reinterpret_cast<const char *> (&originalSize),sizeof(originalSize));
    outFile.put(k);
    unsigned long long bitsLength =0;

    auto twoPowKLeft = pow(2, kLeft);
    auto twoPowKRight = pow(2, kRight);
    unsigned int u = 0;
    unsigned int v = 0;


    for( unsigned int sample = 0; sample < originalSize; sample++)
    for( unsigned char channel = 0; channel < 2; channel++){

        if( channel == 0)
            u = floor(samples[channel][sample]/twoPowKLeft);
        else
            u = floor(samples[channel][sample]/twoPowKRight);

        writeUnary(outFile,u);

        bitsLength+= u + 1;

        if (kLeft != 0){
            if(channel == 0){
                v = sample - u*twoPowKLeft;
                bitsLength += kLeft;
            }
            else {
                v = sample - u * twoPowKRight;
                bitsLength+=kRight;
            }

            writeBinary(outFile, v, kLeft);
        }


    }

    writeBit(outFile, false,true);
    outFile.close();

    return bitsLength;

}






std::vector<unsigned short > decodeRiceMono( const std::string & fileName){
    std::ifstream inFile(".//datFiles//"+fileName+".dat", std::ios::binary);

    if (!inFile.is_open()) {
        throw std::ios_base::failure("Error opening file for reading.");
    }

    unsigned long long allocationSize;

    inFile.read(reinterpret_cast<char *>(&allocationSize),sizeof(allocationSize));
    char k =(char) inFile.get();

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
        n = u *( pow(2, k)) + v;
        decoded.push_back(n);
    }
    inFile.close();

    return decoded;
}



std::vector<std::vector<unsigned short >> decodeRiceStereo( const std::string & fileName){
    std::ifstream inFile(".//datFiles//"+fileName+".dat", std::ios::binary);

    if (!inFile.is_open()) {
        throw std::ios_base::failure("Error opening file for reading.");
    }

    unsigned long long allocationSize;

    inFile.read(reinterpret_cast<char *>(&allocationSize),sizeof(allocationSize));
    auto k =(unsigned char) inFile.get();

    unsigned  char kLeft = (k>>4) & 0xF;
    unsigned char kRight = k& 0xF;


    std::vector<std::vector<unsigned short>> decoded{};
    decoded[0].reserve(allocationSize);
    decoded[1].reserve(allocationSize);

    unsigned int u = 0;
    unsigned int v = 0;
    char bit;
    char vBitPos;
    unsigned short n;
    bool isLeft = true;

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
        n = u *( pow(2, k)) + v;
        if(isLeft)
            decoded[0].push_back(n);
        else
            decoded[1].push_back(n);

        isLeft = !isLeft;
    }
    inFile.close();

    return decoded;
}
