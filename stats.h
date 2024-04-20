//
// Created by User on 21.04.2024.
//

#include <vector>
#include <iostream>
#include <map>
#include <cmath>

#ifndef RICE_AUDIO_STATS_H
#define RICE_AUDIO_STATS_H

#endif //RICE_AUDIO_STATS_H




template <typename T>
bool testDecode(std::vector<T> original, std::vector<T> decoded){
    int size = original.size();
    if(size != decoded.size()){
        std::cout<<"Failed: decoded size not equals original size"<<std::endl;
        return false;
    }

    for(int i = 0; i<size; i++){
        if(original[i] !=decoded[i]){
            std::cout<<"Failed: Sample nr "<<i<<" in decoded data "<<decoded[i]<<" not equal to original "<<original[i]<<std::endl;
            return false;
        }
    }

    std::cout<<"Success: Decoded data equals original "<<std::endl;
    return true;
}


template <typename T>
std::map<T,double> findDistribution(std::vector<T> samples){
    int size = samples.size();

    std::map<T,double> distribution;

    for(auto sample : samples){
        distribution[sample]++;
    }

    for(auto &[key, value] : distribution){
        value = value/size;
    }

    return distribution;
}


template <typename T>
double zeroOrderEntropy(std::vector<T> samples){
    auto distribution = findDistribution(samples);

    double H = 0;

    for(auto &[key,value]:distribution){
        if(value !=0){
            H += value* log2(value);
        }
    }
    H = -H;
    distribution.clear();

    return H;

}


template <typename T>
double findDistributionRice(std::vector<T> samples){
    long long sum = 0;
    for(auto sample:samples){
        sum+=sample;
    }
    double s = sum/samples.size();
    return (s-1)/s;
}

char kValueRice(double p){
    return ceil(log2((log2((sqrt(5)-1)/2))/log2(p)));
}