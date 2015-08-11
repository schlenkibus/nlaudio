#if 0

#include "sampleformatconverter.h"

#include <cmath>
#include <fstream>

void SampleFormatConverter::convert(float sampleIn, int32_t *sampleOut, bool isSigned)
{
	int32_t maxVal = (uint32_t)(~0);
	*sampleOut = sampleIn * maxVal;

}

void SampleFormatConverter::convert(float sampleIn, uint32_t *sampleOut, bool isSigned)
{
	uint32_t maxVal = ((uint32_t)(~0)) >> 1;
	*sampleOut = sampleIn * maxVal;
}


void SampleFormatConverter::test()
{

	ofstream uint32Stream;
	uint32Stream.open("./uint32Stream.txt");

	ofstream int32Stream;
	int32Stream.open("./int32Stream.txt");

	ofstream floatStream;
	floatStream.open("./floatStream.txt");


	uint32_t v1 = 111;
	float v2 = 5.5;


	for (int i=0; i<360; i++) {
		float fVal = sin(float(i)/360.f*2*M_PI);
		floatStream << i << ", " << fVal << std::endl;

		uint32_t uint32Val;
		convert(fVal, &uint32Val, false);
		uint32Stream << i << ", " << uint32Val << std::endl;

		int32_t int32Val;
		convert(fVal, &int32Val, true);
		int32Stream << i << ", " << int32Val << std::endl;

	}





}


#endif
