#include <utility>

#include "AudioFile.h"
#include "encoders-decoders.h"
#include <filesystem>


struct Information {
    std::string name;
    double entropy{};
    double entropyLeft{};
    double entropyRight{};
    double averageBitLength{};
    double efficiency{};
    unsigned int frameSizePowerOfTwo{};
    double averageBitLengthFullFile{};
} ;

#define numOfFiles 16

void printInfo(const std::vector<Information> & information){
    printf("Name\t\t\tAvg Bits length\t \tAvg Bits Length with frames(q=9)\t \tEfficiency\n");
    for(int i = 0; i<108; i++){
        printf("-");
    }

    Information summary;
    summary.name = "Average";
    printf("\n");
    for(auto info:information){

        summary.entropy +=info.entropy;
        summary.entropyLeft+=info.entropyLeft;
        summary.entropyRight +=info.entropyRight;
        summary.averageBitLength += info.averageBitLength;
        summary.averageBitLengthFullFile += info.averageBitLengthFullFile;
        summary.efficiency +=info.efficiency;
        summary.frameSizePowerOfTwo+=info.frameSizePowerOfTwo;


        printf("%s",info.name.c_str());
        for(char i =0; i < (24 -info.name.size()); i++ ){
            printf(" ");
        }
        printf("\t %f\t    \t %f\t\t\t\t %f \n",info.averageBitLengthFullFile,info.averageBitLength,info.efficiency);
    }

    summary.entropy /= information.size();
    summary.entropyLeft /= information.size();
    summary.entropyRight /= information.size();
    summary.averageBitLength/=information.size();
    summary.averageBitLengthFullFile/=information.size();
    summary.efficiency /=information.size();
    summary.frameSizePowerOfTwo /=information.size();

    printf("\n");
    for(int i = 0; i<108; i++){
        printf("-");
    }
    printf("\n");

    printf("%s",summary.name.c_str());
    for(char i =0; i < (24 -summary.name.size()); i++ ){
        printf(" ");
    }
    printf("\t %f\t     \t%f\t\t\t \t%f \n",summary.averageBitLengthFullFile,summary.averageBitLength,summary.efficiency);


}

Information testRice(AudioFile<double> audioFile , const std::string & audioFileName){
    Information information;
    information.name = audioFileName;

    auto converted = convertTo16bit(audioFile.samples[0]);

    auto encodedDifferential = encodeDifferential(converted);
    information.entropyLeft = zeroOrderEntropy(encodedDifferential);


    auto unsignedDifferentialSamples = convertToUnsigned(encodedDifferential);

    auto encodedRiceLeft = encodeRice(unsignedDifferentialSamples,audioFileName);

    auto decodedRice = decodeRiceMono(audioFileName);
    auto decodeRiceDifferential = decodeDifferential(convertToSigned(decodedRice));
    printf("File %s left channel Rice`a decode test: ",audioFileName.c_str());
    testDecode(converted,decodeRiceDifferential);


    converted = convertTo16bit(audioFile.samples[1]);

    encodedDifferential = encodeDifferential(converted);
    information.entropyRight = zeroOrderEntropy(encodedDifferential);

    unsignedDifferentialSamples = convertToUnsigned(encodedDifferential);

     auto encodedRiceRight = encodeRice(unsignedDifferentialSamples,audioFileName+"R");
    decodedRice = decodeRiceMono(audioFileName+"R");
    decodeRiceDifferential = decodeDifferential(convertToSigned(decodedRice));
    printf("File %s right channel Rice`a decode test: ",audioFileName.c_str());

     testDecode(converted,decodeRiceDifferential);

    information.averageBitLength = (double)(encodedRiceLeft+encodedRiceRight)/(2*audioFile.getNumSamplesPerChannel());

    information.entropy =( information.entropyRight + information.entropyLeft)/2;

    information.efficiency = (information.entropy/information.averageBitLength)*100;

    return information;



}


Information testRiceStereo(const AudioFile<double>  & audioFile , const std::string & audioFileName){
    Information information;
    information.name = audioFileName;

    auto converted = convertTo16bit(audioFile.samples);
    auto convertedMono = convertTo16bit(audioFile.samples[0]);

    auto encodedDifferential = encodeDifferential(converted);
    information.entropyLeft = zeroOrderEntropy(encodedDifferential[0]);


    auto unsignedDifferentialSamples = convertToUnsigned(encodedDifferential);

    auto encodedRice = encodeRice(unsignedDifferentialSamples,audioFileName);

    auto decodedRice = decodeRiceStereo(audioFileName);
    auto decodeRiceDifferential = decodeDifferential(convertToSigned(decodedRice));
    printf("File %s  Rice`a decode test: ",audioFileName.c_str());
    testDecode(converted,decodeRiceDifferential);


    information.entropyRight = zeroOrderEntropy(encodedDifferential[1]);




    information.averageBitLength = (double)(encodedRice)/(2*audioFile.getNumSamplesPerChannel());

    information.entropy =( information.entropyRight + information.entropyLeft)/2;

    information.efficiency = (information.entropy/information.averageBitLength)*100;

    return information;



}


Information testGolombStereo(const AudioFile<double>  & audioFile , const std::string & audioFileName, std::map<char,double> & qAverage){
    Information information;
    information.name = audioFileName;

    auto converted = convertTo16bit(audioFile.samples);


    auto encodedDifferential = encodeDifferential(converted);
    information.entropyLeft = zeroOrderEntropy(encodedDifferential[0]);


    auto unsignedDifferentialSamples = convertToUnsigned(encodedDifferential);
    //plot(unsignedDifferentialSamples[0], audioFileName);
    unsigned long long encodedGolomb = -1;
    unsigned long long temp;
    char q = 9;

//    for(char i = 5; i<15;i++){
        encodedGolomb = encodeGolomb(unsignedDifferentialSamples,q,audioFileName);
//        qAverage[i]+=(double)(temp)/(2*audioFile.getNumSamplesPerChannel())/numOfFiles;
//        if(encodedGolomb>temp){
//            q = i;
//            encodedGolomb = temp;
//        }
//    }
    information.averageBitLengthFullFile = (double)(encodeGolomb(unsignedDifferentialSamples,audioFileName))/(2*audioFile.getNumSamplesPerChannel());


    information.frameSizePowerOfTwo = q;
//    auto decodedGolomb = decodeGolombStereoFrames(audioFileName);
//    auto decodeGolombDifferential = decodeDifferential(convertToSigned(decodedGolomb));
//
//    printf("File %s  Golomb decode test: ",audioFileName.c_str());
//
//    testDecode(converted,decodeGolombDifferential);


    information.entropyRight = zeroOrderEntropy(encodedDifferential[1]);



    information.averageBitLength = (double)(encodedGolomb)/(2*audioFile.getNumSamplesPerChannel());

    information.entropy =( information.entropyRight + information.entropyLeft)/2;

    information.efficiency = (information.entropy/information.averageBitLength)*100;

    return information;



}



int main() {

    AudioFile<double> audioFile;

    std::vector<Information> information;

    std::map<char, double> qAverage;
    for(char i = 5; i<15;++i){
        qAverage[i] = 0;
    }


    audioFile.load(".//audio//ATrain.wav");

//    information.push_back(testGolombStereo(audioFile,"ATrain.waw"));


    std::filesystem::path directoryPath = ".//audio";

    if(std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)){
        for(const auto & entry : std::filesystem::directory_iterator(directoryPath)){
            audioFile.load(entry.path().string());
//            information.push_back(testRice(audioFile,entry.path().filename().string()));
//            information.push_back(testRiceStereo(audioFile,entry.path().filename().string()));
            information.push_back(testGolombStereo(audioFile,entry.path().filename().string(),qAverage));
        }
    } else{
        std::cerr << "Directory does not exist or is not a directory!" <<std::endl;
    }

//    plot(qAverage,"qAverage");



    printf("\n");




    printInfo(information);




    return 0;
}
