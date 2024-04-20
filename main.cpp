#include <iostream>
#include <cmath>
#include "AudioFile.h"
#include <map>

using namespace std;

template <typename T>
class bitMask{
public:
    T mask;
    int size = sizeof(T);
    bitMask(){
        mask = 1;
    }

    void setLastBit(){
        mask = 1;
    }

    void setFirstBit(){
        mask = 1 << (size * 8 - 1);
    }
    void left(){
        mask = mask << 1;
        if(mask == 0){
            mask = 1;
        }
    }

    void right(){
        mask = mask >> 1;
        if(mask == 0){
            mask =  1 << (size * 8 - 1);
        }
    }


};

template <typename T>
vector<short> convertTo16bit(vector<T> samples){
    int size = samples.size();
    vector<short>  converted;
    converted.reserve(size);
    for(auto sample:samples){
        converted.push_back(round(sample * pow(2,15)));
    }
    return converted;
}

template <typename T>
vector<T> encodeDifferential(vector<T> samples){
    int size = samples.size();
    vector<T> encoded;
    encoded.reserve(size);;


    encoded.push_back(samples.front());
    for(int i = 1; i<size;i++){
        encoded.push_back(samples[i]-samples[i-1]);
    }


    return encoded;
}

template <typename T>
vector<T> decodeDifferential(vector<T> encodedSamples){
    int size = encodedSamples.size();
    vector<T> decoded;
    decoded.reserve(size);

    decoded.push_back(encodedSamples.front());
    for(int i = 1; i<size; i++){
        decoded.push_back(encodedSamples[i]+decoded[i-1]);
    }

    return decoded;
}

template <typename T>
bool testDecode(vector<T> original, vector<T> decoded){
    int size = original.size();
    if(size != decoded.size()){
        cout<<"Failed: decoded size not equals original size"<<endl;
        return false;
    }

    for(int i = 0; i<size; i++){
        if(original[i] !=decoded[i]){
            cout<<"Failed: Sample nr "<<i<<" in decoded data not equal to original"<<endl;
            return false;
        }
    }

    cout<<"Success: Decoded data equals original "<<endl;
    return true;
}


template <typename T>
map<T,double> findDistribution(vector<T> samples){
    int size = samples.size();

    map<T,double> distribution;

    for(auto sample : samples){
        distribution[sample]++;
    }

    for(auto &[key, value] : distribution){
        value = value/size;
    }

    return distribution;
}


template <typename T>
double zeroOrderEntropy(vector<T> samples){
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
vector<unsigned short> convertToUnsigned(vector<T> samples){
    int size = samples.size();
    vector<unsigned short> unsignedSamples;
    unsignedSamples.reserve(size);

    for(auto sample: samples){
        unsignedSamples.push_back(sample >=0 ? sample*2 : sample *(-2) -1);
    }
    return unsignedSamples;
}


vector<short> convertToSigned(vector<unsigned short>  unsignedSamples){
    unsigned int size = unsignedSamples.size();
    vector<short> signedSamples;
    signedSamples.reserve(size);

    for(auto sample:unsignedSamples){
        signedSamples.push_back((sample & 1) ? -(sample+1)/2 : sample/2);
    }

    return signedSamples;
}

template <typename T>
double findDistributionRice(vector<T> samples){
    long long sum = 0;
    for(auto sample:samples){
        sum+=sample;
    }
    double s = sum/samples.size();
    return (s-1)/s;
}


template <typename T>
void encodeRice(vector<T> samples){

}




int main() {

    AudioFile<double> audioFile;

    audioFile.load(".//audio//ATrain.wav");
    audioFile.printSummary();


    auto converted = convertTo16bit(audioFile.samples[0]);

    auto encoded = encodeDifferential(converted);
    auto decoded = decodeDifferential(encoded);

    auto test = testDecode(converted,decoded);

    cout<<endl<<zeroOrderEntropy(converted)<<endl;

    auto bit = * new bitMask<unsigned short>();


    return 0;
}
