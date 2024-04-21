#include <utility>

#include "AudioFile.h"
#include "encoders-decoders.h"


struct Information {
    std::string name;
    double entropy{};
    double entropyLeft{};
    double entropyRight{};
    double averageBitLength{};
    double efficiency{};

} ;

void printInfo(const std::vector<Information> & information){
    printf("\tName\t\tEntropy Left\t Entropy Right\t  Entropy\t Average Bit Length\t Efficiency\n\t");
    for(int i = 0; i<100; i++){
        printf("-");
    }
    printf("\n");
    for(auto info:information){
        printf("\t%s",info.name.c_str());
        if(info.name.size() <7){
            printf("\t");
        }
        printf("\t%f\t %f  \t  %f\t     %f\t \t %f %\n",info.entropyLeft,info.entropyRight,info.entropy,info.averageBitLength,info.efficiency);
    }
}

Information testRice(AudioFile<double> audioFile , const std::string & audioFileName){
    Information information;
    information.name = audioFileName;

    auto converted = convertTo16bit(audioFile.samples[0]);

    auto encodedDifferential = encodeDifferential(converted);
    information.entropyLeft = zeroOrderEntropy(encodedDifferential);


    auto unsignedDifferentialSamples = convertToUnsigned(encodedDifferential);

    auto encodedRiceLeft = encodeRice(unsignedDifferentialSamples,audioFileName);

    auto decodedRice = decodeRice(audioFileName);
    auto decodeRiceDifferential = decodeDifferential(convertToSigned(decodedRice));
    printf("File %s.wav left channel Rice`a decode test: ",audioFileName.c_str());
    testDecode(converted,decodeRiceDifferential);


   auto convertedR = convertTo16bit(audioFile.samples[1]);

   auto encodedDifferentialR = encodeDifferential(convertedR);
    information.entropyRight = zeroOrderEntropy(encodedDifferentialR);

    auto unsignedDifferentialSamplesR = convertToUnsigned(encodedDifferentialR);

    auto encodedRiceRight = encodeRice(unsignedDifferentialSamplesR,audioFileName+"R");
     auto decodedRiceR = decodeRice(audioFileName+"R");
     auto convertedToSignedR = convertToSigned(decodedRiceR);
     auto decodeRiceDifferentialR = decodeDifferential(convertedToSignedR);
    printf("File %s.wav right channel Rice`a decode test: ",audioFileName.c_str());

     testDecode(convertedR,decodeRiceDifferentialR);

    information.averageBitLength = (double)(encodedRiceLeft+encodedRiceRight)/(2*audioFile.getNumSamplesPerChannel());

    information.entropy =( information.entropyRight + information.entropyLeft)/2;

    information.efficiency = (information.entropy/information.averageBitLength)*100;

    return information;



}





int main() {

    AudioFile<double> audioFile;

    std::vector<Information> information;

    audioFile.load(".//audio//Layla.wav");



    information.push_back(testRice(audioFile,"Layla"));

    audioFile.load(".//audio//velvet.wav");
    information.push_back(testRice(audioFile,"velvet"));

    audioFile.load(".//audio//ATrain.wav");
    information.push_back(testRice(audioFile,"ATrain"));

    audioFile.load(".//audio//female_speech.wav");
    information.push_back(testRice(audioFile,"female_speech"));

    audioFile.load(".//audio//male_speech.wav");
    information.push_back(testRice(audioFile,"male_speech"));

    audioFile.load(".//audio//TomsDiner.wav");
    information.push_back(testRice(audioFile,"TomsDiner"));


    printInfo(information);



//    auto converted = convertTo16bit(audioFile.samples[0]);
//
//    auto encoded = encodeDifferential(converted);
//
//
//
//    auto unsignedDifferentialSamples = convertToUnsigned(encoded);
//
//    auto encodedRice = encodeRice(unsignedDifferentialSamples,"ATrain");
//
//
//    auto decodedRice = decodeRice("ATrain");
//
//
//
//    testDecode(unsignedDifferentialSamples,decodedRice);



    return 0;
}
