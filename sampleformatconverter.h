#ifndef SAMPLEFORMATCONVERTER_H
#define SAMPLEFORMATCONVERTER_H

#if 0

#include <cstdint>


using namespace std;

namespace SampleFormatConverter {

uint32_t uint32Samples[360];
int32_t int32Samples[360];
float floatSamples[360];


void test();
//void convert(int32_t *sampleIn, float *sampleOut);
//void convert(uint32_t *sampleIn, float *sampleOut);

void convert(float sampleIn, int32_t *sampleOut, bool isSigned);
void convert(float sampleIn, uint32_t *sampleOut, bool isSigned);


}

#endif

#endif // SAMPLEFORMATCONVERTER_H
