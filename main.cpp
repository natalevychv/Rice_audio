#include "AudioFile.h"
#include "encoders-decoders.h"






int main() {

    AudioFile<double> audioFile;

    audioFile.load(".//audio//ATrain.wav");
    audioFile.printSummary();


    auto converted = convertTo16bit(audioFile.samples[0]);

    auto encoded = encodeDifferential(converted);
    auto decoded = decodeDifferential(encoded);

    auto test = testDecode(converted,decoded);



    auto unsignedDifferentialSamples = convertToUnsigned(encoded);

    auto encodedRice = encodeRice(unsignedDifferentialSamples,"ATrain");


    auto decodedRice = decodeRice("ATrain");



    testDecode(unsignedDifferentialSamples,decodedRice);



    return 0;
}
