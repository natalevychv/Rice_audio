//
// Created by User on 21.04.2024.
//

#include <fstream>
#include <utility>
#include <vector>
#include <cmath>
#include <map>

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
std::vector<std::vector<short>> convertTo16bit(std::vector<std::vector<T>> samples){
    int size = samples[0].size();
    std::vector<std::vector<short>> converted(2,std::vector<short>(0,0));
    converted[0].reserve(size);
    converted[1].reserve(size);
    bool isLeft = true;


    for (const auto & channel: samples){
        for(const auto & sample:channel){
            if(isLeft)
                converted[0].push_back(round(sample * pow(2,15)));
            else
                converted[1].push_back(round(sample * pow(2,15)));

        }
        isLeft = !isLeft;
    }


    return converted;
}


template <typename T>
std::vector<unsigned short> convertToUnsigned(std::vector<T> samples){
    int size = samples.size();
    std::vector<unsigned short> unsignedSamples;
    unsignedSamples.reserve(size);

    for(auto sample: samples){
        unsignedSamples.push_back(sample >=0 ? (sample<<1) : -(sample<<1) -1);
    }
    return unsignedSamples;
}



template <typename T>
std::vector<std::vector<unsigned short>>  convertToUnsigned(std::vector<std::vector<T>> samples){
    int size = samples[0].size();
    std::vector<std::vector<unsigned short>> unsignedSamples(2, std::vector<unsigned short>(0,0));

    bool isLeft = true;
    unsignedSamples[0].reserve(size);
    unsignedSamples[1].reserve(size);


    for(const auto channel: samples){

        for(auto sample: channel){
            if(isLeft)
                unsignedSamples[0].push_back(sample >=0 ? (sample<<1) : -(sample<<1) -1);
            else
                unsignedSamples[1].push_back(sample >=0 ? (sample<<1) : -(sample<<1) -1);

        }
        isLeft = !isLeft;
    }
    return unsignedSamples;
}


std::vector<short> convertToSigned(std::vector<unsigned short>  unsignedSamples){
    unsigned int size = unsignedSamples.size();
    std::vector<short> signedSamples;
    signedSamples.reserve(size);

    for(auto sample:unsignedSamples){
        signedSamples.push_back((sample & 1) ? -(sample+1)>>1 : sample>>1);
    }

    return signedSamples;
}

std::vector<std::vector<short>> convertToSigned(std::vector<std::vector<unsigned short>>  unsignedSamples){
    unsigned int size = unsignedSamples[0].size();
    std::vector<std::vector<short>> signedSamples(2, std::vector<short>(0,0));

    signedSamples[0].reserve(size);
    signedSamples[1].reserve(size);

    bool isLeft = true;


    for(const auto  & channel: unsignedSamples) {
        for (const auto & sample: channel) {
            if(isLeft)
                signedSamples[0].push_back((sample & 1) ? -(sample + 1) >> 1 : sample >> 1);
            else
                signedSamples[1].push_back((sample & 1) ? -(sample + 1) >> 1 : sample >> 1);
        }
        isLeft = !isLeft;

    }

    return signedSamples;
}




void writeBit(std::ofstream& file, bool bit, bool eof){
    static unsigned char currentByte = 0;
    static char bitPos = 0;
    if(eof){
        file.put(currentByte);
        currentByte = 0;
        bitPos = 0;
        return;
    }

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
        writeBit(stream,false,false);
    }
    writeBit(stream,true,false);
}


void writeBinary(std::ofstream& stream, unsigned int v, unsigned int bits){
    unsigned int bitPos = (1<<(bits-1));
    bool bit;
    for(int i = 0; i < bits; i++){
        bit = (v&bitPos);
        writeBit(stream, bit,false);
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

unsigned int readMGolomb(std::istream & stream){
    unsigned int m = 0;
    for(char bitPos = 31; bitPos>=0; bitPos--){
        m|=(readBit(stream)<<bitPos);
    }
    return m;
}


void writeToFile(std::vector<unsigned short> samples, std::string fileName){
    std::ofstream filex("pointsX.txt");
    int stop = 0;
    if(filex.is_open()){
        std::string temp;
        for(const auto & sample: samples ){
            stop++;
            if(stop > 1000) break;
            temp = std::to_string(sample) + ", " ;
            filex<<temp;
        }
    }
    filex.close();

    std::ofstream filen("fileN.txt");
    if(filen.is_open()){
        filen<<fileName;
    }
    filen.close();
}


void writeToFile(std::map<char, double> map, std::string fileName){
    std::ofstream filex("pointsX.txt");
    if(filex.is_open()){
        std::string temp;
        for(const auto & sample: map ){

            temp = std::to_string(sample.first) + ", " ;
            filex<<temp;
        }
    }
    filex.close();

    std::ofstream filey("pointsY.txt");

    if(filey.is_open()){
        std::string temp;
        for(const auto & sample: map ){

            temp = std::to_string(sample.second) + ", " ;
            filey<<temp;
        }
    }
    filey.close();

    std::ofstream filen("fileN.txt");
    if(filen.is_open()){
        filen<<fileName;
    }
    filen.close();
}


void plot(std::vector<unsigned short> x, std::string fileName){
    writeToFile(std::move(x),std::move(fileName));
    system("plotPoints.py");
}

void plot(std::map<char, double > x, std::string fileName){
    writeToFile(std::move(x),std::move(fileName));
    system("plotPoints.py");
}
