#include "examples.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "tools.h"

extern Nl::StopWatch sw;

namespace Nl {
namespace Examples {

//conversion helper functions --- schould be in a header in future
unsigned int getByteIndex(unsigned int frameIndex, unsigned int channel, unsigned int byte, const SampleSpecs &sampleSpecs)
{
    static unsigned int currByte;

             // Index of current Frame
    currByte = (frameIndex*sampleSpecs.channels*sampleSpecs.bytesPerSample) +
            // Index of current channel in Frame (= Sample)
            (channel*sampleSpecs.bytesPerSample) +
            // Index of current byte in Sample
            byte;
    return currByte;
    /*return	// Index of current Frame
            (frameIndex*sampleSpecs.channels*sampleSpecs.bytesPerSample) +
            // Index of current channel in Frame (= Sample)
            (channel*sampleSpecs.bytesPerSample) +
            // Index of current byte in Sample
            byte;*/
}

float getSample(u_int8_t* in, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs)
{
    // Protect against segfault
    if (frameIndex > sampleSpecs.buffersizeInFramesPerPeriode)
        return 0.f;
    if (channelIndex > sampleSpecs.channels)
        return 0.f;

    if (sampleSpecs.isSigned) {

#if 0 // 2016-01-21 original PALSA code
        //Big-Endian
        signed int currentSample = 0; // use int32_t
        signed int currentMask = 0;   // use int32_t

        for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
            currentSample |= in[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] << ((sampleSpecs.bytesPerSample-byte-1)*8);
            currentMask |= (0xFF << (byte*8));
        }

        // If MSB is 1 we deal with negative numbers.
        if (currentSample & (1 << (sampleSpecs.bytesPerSample*8-1)))
            currentSample |= ~currentMask;

        return static_cast<float>(currentSample) / static_cast<float>(currentMask);
#endif

#if 1 // AS 2016-01-25
        //Little-Endian
        signed int currentSample = 0; // use int32_t
        signed int currentMask = 0;   // use int32_t

        for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
            currentSample |= in[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)]  << (byte*8);
            currentMask |= (0xFF << (byte*8));
        }

        // If MSB is 1 we deal with negative numbers.
        if (currentSample & (1 << (sampleSpecs.bytesPerSample*8-1)))
            currentSample |= ~currentMask;

        return static_cast<float>(currentSample) / static_cast<float>(currentMask);


#endif
    } else { // UNSIGNED
        //TODO: implement me
        return 0.f;
    }
}

void setSample(u_int8_t* out, float sample, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs)
{
#if 0 // 2016-01-21 original PALSA code
    // Protect against segfault
    if (frameIndex > sampleSpecs.buffersizeInFramesPerPeriode)
        return;
    if (channelIndex > sampleSpecs.channels)
        return;

    if (sampleSpecs.isSigned) {

        int32_t currentMask = 0;

        for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++)
            currentMask |= (0xFF << (byte*8));

        for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
            // TODO: if isLittleEndian...
            out[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] =
                    (static_cast<int32_t>(sample * currentMask)) >> ((sampleSpecs.bytesPerSample-byte-1)*8);
        }

    } else { // UNSIGNED
        //TODO: implement me
    }
#endif

#if 1 // 2016-01-22 AS
    /* Ich weiß bis jetzt nicht wo der genaue Fehler liegt, aber ich vermute in der folgenden Zeile:
     * (static_cast<int32_t>(sample * currentMask)) >> ((sampleSpecs.bytesPerSample-byte-1)*8);
     * In der Version von Pascal soll das "nicht-maskierte" um 16-Bit nach rechts verschoben werden.
     * Dadurch entsehen, wenn ich das alles richtig verstanden habe, Umrechnungsfehler ...
     * Wir weisen dem out-Buffer byteweise Werte zu, und zwar nach Little-Endian Prinzip.
     * Also können wir ja gleich dem ersten Byte, den größten Wert zuweisen (siehe unten).*/

    if (frameIndex > sampleSpecs.buffersizeInFramesPerPeriode)
        return;
    if (channelIndex > sampleSpecs.channels)
        return;

    if (sampleSpecs.isSigned) {

        int32_t currentMask = 0;

        for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++)
        {
            /*Little-Endian - Jahr.Monat.Tag || Big-Endian - Tag.Monat.Jahr ... richtig?
             * wir arbeiten in diesem Fall mit Little-Endian (siehe sampleSpecs)
             * sprich wenn ich mit der Maske arbeite, dann kann ich doch hier die 255
             * erst um 16Bit (dann um 8Bit, dann um 0Bit) verschieben und brauche
             * später gar nicht zu schiften ... oder?*/
            currentMask = (0xFF << ((sampleSpecs.bytesPerSample-1-byte)*8));
#if 1
            //Zur Kontrolle ...
            int32_t testVal32 = static_cast<int32_t>(sample * currentMask);
            uint8_t testValu8 = static_cast<uint8_t>((static_cast<int32_t>(sample * currentMask)));
#endif
            /*da ich die Maske oben anders definiere, fällt die Bitschubserei weg.
             * zusätzlich wird noch in uint8_t gecastet. Zum einen, weil der übergebene out-Buffer
             * auch diesen Typen besitzt, zum anderen, weil das auch in dem midiSine Beispiel
             * geschieht und da scheint es zu funktionieren
             * EDIT: okay ... klappt auch ohne in uint8_t zu casten ...
             * Ist es weil das automatisch geschieht bei der Wertzuweisung?*/
            out[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] =
                    (static_cast<int32_t>(sample * currentMask));
                    //static_cast<uint8_t>((static_cast<int32_t>(sample * currentMask)));
        }

    } else { // UNSIGNED
        //TODO: implement me
    }
#endif

}

// In to out example
void inToOutCallback(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
{
	static int counter = 0;
	StopBlockTime sft(&sw, "val" + std::to_string(counter++));

	memcpy(out, in, size);
}

ExamplesHandle inputToOutput(const AlsaCardIdentifier &inCard, const AlsaCardIdentifier &outCard, unsigned int buffersize, unsigned int samplerate)
{
	// In this example, we just copy data from input to output
	// Samplerate and buffersize can be set. A handle to stop the
	// working threads is returned
	// To terminate this example, call:
	// Nl::terminateWorkingThread(hamdle)
	ExamplesHandle ret;

	ret.inBuffer = createBuffer("InputBuffer");

	ret.audioInput = createInputDevice(inCard, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);

	ret.outBuffer = createBuffer("OutputBuffer");
	ret.audioOutput = createOutputDevice(outCard, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

	// DANGER!!!!
	// TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
	//		 input/output->start(), we seem to have a deadlock!
	// TODO: Consider implementing something like autostart for the threads in the
	//		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
	ret.audioInput->start();
	ret.audioOutput->start();

	ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallback);

	return ret;
}



void midiSineCallback(u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs)
{
	static int counter = 0;
	StopBlockTime sft(&sw, "val" + std::to_string(counter++));

	static uint8_t velocity = 0;
	static double frequency = 0;
	static int32_t notesOn = 0;

	unsigned char midiByteBuffer[3];
	bool reset = false;

	// We can get a buffer by its name, to access its data:
	auto midiBuffer = getBufferForName("MidiBuffer");

	if(midiBuffer) {
		while(midiBuffer->availableToRead() >= 3) {
			midiBuffer->get(midiByteBuffer, 3);
			if(midiByteBuffer[0] == 0x90)
			{
				velocity = midiByteBuffer[2];
				if(velocity) {
					notesOn++;
					reset = true;   // nur vorläufig
					frequency = pow(2.f, static_cast<double>((midiByteBuffer[1]-69)/12.f)) * 440.f;
				} else {
					notesOn--;
				}
			} else if(midiByteBuffer[0] == 0x80) {
				notesOn--;
			}
		}
	}

	if(notesOn > 0) {
		int32_t samples[sampleSpecs.buffersizeInFramesPerPeriode];
		sinewave<int32_t>(samples, frequency, reset, sampleSpecs);

		for (unsigned int byte=0; byte<sampleSpecs.buffersizeInBytesPerPeriode; byte++) {
            unsigned int currentSample = (byte / (sampleSpecs.channels * sampleSpecs.bytesPerSample));
			unsigned int byteIndex = (byte % sampleSpecs.bytesPerSample);

			if (sampleSpecs.isLittleEndian) {
				*out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((byteIndex)*8)) & 0xFF);
			} else {
				*out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((sampleSpecs.bytesPerSample-byteIndex-1)*8)) & 0xFF);
			}
		}
	}
	else {
		memset(out, 0, size);
	}
}

// Midi Sine example
ExamplesHandle midiSine(const AlsaCardIdentifier &audioOutCard,
						const AlsaCardIdentifier &midiInCard,
						unsigned int buffersize,
						unsigned int samplerate)
{
	ExamplesHandle ret;

	// Not needed, since we only playback here
	ret.inBuffer = nullptr;
	ret.audioInput = nullptr;

	// Lets create a buffer, which we have to pass to the output soundcard
	ret.outBuffer = createBuffer("AudioOutput");
	// Open soundcard, using above buffer
	ret.audioOutput = createOutputDevice(audioOutCard, ret.outBuffer, buffersize);

	// Configure Audio (if needed, or use default)
	//ret.audioOutput->setSampleFormat(...);
    ret.audioOutput->setSamplerate(samplerate);
	ret.audioOutput->setChannelCount(2);
	//ret.audioOutput->setSampleFormat("S16_LE");

	// We want midi as well
	ret.inMidiBuffer = createBuffer("MidiBuffer");
    ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

	// Start Audio and Midi Thread
	ret.audioOutput->start();
    ret.midiInput->start();

    std::cout << "MidiBufferSize: " << ret.midiInput->getAlsaMidiBufferSize() << std::endl;

	// Register a Callback
	ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, midiSineCallback);

	return ret;
}

// Silence Example
void silenceCallback(u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
{
	memset(out, 0, size);
}

ExamplesHandle silence(const AlsaCardIdentifier &audioOutCard,
					   unsigned int buffersize,
					   unsigned int samplerate)
{
	ExamplesHandle ret;

	// Not nedded, since we only playback here w/o midi
	ret.inBuffer = nullptr;
	ret.audioInput = nullptr;

	// Create an output buffer and an output device
	ret.outBuffer = createBuffer("AudioOutput");
	ret.audioOutput = createOutputDevice(audioOutCard, ret.outBuffer, buffersize);

	// Configure audio device
	ret.audioOutput->setSamplerate(samplerate);

	// Start audio Thread
	ret.audioOutput->start();

	// Register a Callback
	ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, silenceCallback);

	return ret;
}

// Midi Influence Example
void inToOutCallbackWithMidi(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs){

    // Audio Stuff
#if 0
    unsigned int fs = sampleSpecs.samplerate;
    static float currentSample = -0.5;

    //für einen 1Khz Sägezahn
    unsigned int f = 500;
    float resolution = 1./static_cast<float>(fs/f);

    //für 24Bit Auflösung
    //float resolution = 1./static_cast<float>(pow(2,24));
#endif

    //Midi Stuff
    static float curVolumeFactor = 0.f;
    auto midiBuffer = getBufferForName("MidiBuffer");

    if (midiBuffer) {
        unsigned char midiByteBuffer[3];
        while (midiBuffer->availableToRead() >= 3) {
            midiBuffer->get(midiByteBuffer, 3);
            printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
            //std::cout<<std::dec<<static_cast<float>(std::numeric_limits<float>::max())<<std::endl;
            if (midiByteBuffer[1] == 0x02) {
                //curVolumeFactor = static_cast<float>(midiByteBuffer[2]);
                //printf("currVol-> %f\n", curVolumeFactor);
                curVolumeFactor = static_cast<float>(midiByteBuffer[2]) / static_cast<float>(std::numeric_limits<unsigned char>::max() / 2);
                //curVolumeFactor = static_cast<float>(midiByteBuffer[2]) / 127.f;
                printf("currVol-> %f\n", curVolumeFactor);
            }
        }
    }


    for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex) {
#if 0
        //calculate samplewise
        currentSample += resolution;
        if(currentSample > 0.5)
            currentSample = -0.5;
#endif
        for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
#if 1
            // Get float sample
            float currentSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
            // Write back float sample
            //setSample(out, currentSample, frameIndex, channelIndex, sampleSpecs);
#endif
            // Do something with float sample
            // currentSample *= curVolumeFactor;
            // printf("currVol-> %f\n", currentSample);
            setSample(out, currentSample * curVolumeFactor, frameIndex, channelIndex, sampleSpecs);
        }

        // printf("%.5f\n",currentSample);
    }
}

#if 0
//Midi Influence Example
void inToOutCallbackWithMidi(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs)
{
    float samplingRate = static_cast<float>(sampleSpecs.samplerate);
    float frequency = 800.;
    unsigned int bufferSize = sampleSpecs.buffersizeInSamplesPerPeriode;

#if 0
    // Sine Generation
    bool reset = true;
    float sineSamples[sampleSpecs.buffersizeInFramesPerPeriode];
    sinewave<float>(sineSamples, frequency, reset, sampleSpecs);
#endif
#if 0
    float sineSamples[sampleSpecs.buffersizeInFramesPerPeriode];
    sinewave<float>(sineSamples, sampleSpecs.buffersizeInFramesPerPeriode);
#endif

#if 0
    //Own Sine Generation
    //static float phase = 0.;
    //float time_length = 1.;
    float frequency = 800.;
    //static float inc = frequency / samplingRate;
    //unsigned int endtime = sampleSpecs.buffersizeInFramesPerPeriode;

    float sineSamples[sampleSpecs.buffersizeInFramesPerPeriode];

    for(unsigned int time = 0; time < samplingRate; ++time){
        //phase += inc;

        //if (phase > 0.5)
        //   phase -= 1.0;

        sineSamples[time] = sin(2.f * M_PI* frequency * time);
    }
#endif

#if 0
    //Square Generation
    float squareSamples[sampleSpecs.buffersizeInFramesPerPeriode];

    for(unsigned int cnt = 0; cnt < sampleSpecs.buffersizeInFramesPerPeriode; ++cnt){
        if(cnt<sampleSpecs.buffersizeInFramesPerPeriode/4)
           squareSamples[cnt] = 1.f;
        else if(cnt>sampleSpecs.buffersizeInFramesPerPeriode/4 && cnt<sampleSpecs.buffersizeInFramesPerPeriode/2)
            squareSamples[cnt] = -1.f;
        else if(cnt>sampleSpecs.buffersizeInFramesPerPeriode/2 && cnt<(sampleSpecs.buffersizeInFramesPerPeriode/4)*3)
            squareSamples[cnt] = 1.f;
        else
            squareSamples[cnt] = -1.f;
        //unsigned int byteIndex = (cnt % sampleSpecs.bytesPerSample);
        //squareSamples[cnt] = -1.f;

    }
#endif

#if 0
    //Triangle Wave and Saw Wave Generator
    //float triangleSamples[sampleSpecs.buffersizeInFramesPerPeriode];
    float sawSamples[sampleSpecs.buffersizeInSamplesPerPeriode];
    float amplitude = 2.;
    float temp = -1.;

    for(unsigned int cnt = 0; cnt < bufferSize; ++cnt){

        sawSamples[cnt] = temp;
        temp += amplitude/static_cast<float>(bufferSize);
        //printf("%f\n", temp);
        if (temp > 1.)
            temp = -1. * amplitude;

        //triangleSample[cnt] = 0;
        //float pos = fmod(frequency*cnt/(samplingRate/2.f),1.f);
        //triangleSamples[cnt] = (1-fabs(pos-0.5f)*4.f)*0.5f;
        //float factor = floor(cnt/(frameSize)+0.5f);
        //triangleSamples[cnt] = 1.f - (2.f/(frameSize))*(cnt-frameSize*factor*pow(-1.f,factor));
        //sawSamples[cnt] = (pos*2.f-1.f)*0.5f;
        //sawSamples[cnt] = 2.f*fabs(2.f*(cnt/frameSize-factor))-1.f;

    }

#endif

    // Midi Stuff
    static float curVolumeFactor = 0.f;
    auto midiBuffer = getBufferForName("MidiBuffer");

    if (midiBuffer) {
        unsigned char midiByteBuffer[3];
        while (midiBuffer->availableToRead() >= 3) {
            midiBuffer->get(midiByteBuffer, 3);
            printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
            //std::cout<<std::dec<<static_cast<float>(std::numeric_limits<float>::max())<<std::endl;
            if (midiByteBuffer[1] == 0x02) {
                //curVolumeFactor = static_cast<float>(midiByteBuffer[2]);
                //printf("currVol-> %f\n", curVolumeFactor);
                curVolumeFactor = static_cast<float>(midiByteBuffer[2]) / static_cast<float>(std::numeric_limits<unsigned char>::max() / 2);
                //curVolumeFactor = static_cast<float>(midiByteBuffer[2]) / 127.f;
                printf("currVol-> %f\n", curVolumeFactor);
            }
        }
    }

    // Audio Stuff
    //unsigned int frameSampleIndex = 0;
    static float currentSample = -1.;
    //für 24Bit Auflösung
    float resolution = 1./static_cast<float>(pow(2,24));

    for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex) {

#if 0
        //get Samples from buffer
        currentSample = sawSamples[frameIndex*2+frameSampleIndex];
        ++frameSampleIndex;
        if(frameSampleIndex > 1){
            frameSampleIndex = 0;
        }
#endif

        //calculate samplewise
        currentSample += resolution;
        if(currentSample > 1.)
            currentSample = -1.;

        for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
#if 0
            // Get float sample
            float currentSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
            // Do something with float sample
            currentSample *= curVolumeFactor*127.f;
            // Write back float sample
            setSample(out, currentSample, frameIndex, channelIndex, sampleSpecs);
#endif

#if 1
            //float currentSample = sawSamples[frameIndex];
            //adjust Volume
            //currentSample *= 20.f;
            setSample(out, currentSample, frameIndex, channelIndex, sampleSpecs);
#endif
        }
    }
}
#endif

/*void inToOutCallbackWithMidi(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
{

    auto midiBuffer = getBufferForName("MidiBuffer");
    unsigned char midiInfoBuffer[3];
    static bool notesOn = false;
    static float currentVolume;

    // for the sine generator
    bool reset = true;
    static double frequency = 1600.f;
    int32_t samples[sampleSpecs.buffersizeInFramesPerPeriode];
    sinewave<int32_t>(samples, frequency, reset, sampleSpecs);

    for (unsigned int byte=0; byte<sampleSpecs.buffersizeInBytesPerPeriode; byte++) {
        unsigned int currentSample = (byte / (sampleSpecs.channels * sampleSpecs.bytesPerSample));
        unsigned int byteIndex = (byte % sampleSpecs.bytesPerSample);

        if (sampleSpecs.isLittleEndian) {
            *out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((byteIndex)*8)) & 0xFF);
        } else {
            *out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((sampleSpecs.bytesPerSample-byteIndex-1)*8)) & 0xFF);
        }
    }

    if (midiBuffer) {

        while(midiBuffer->availableToRead() >= 3) {
            //unsigned char buffer[3];
            midiBuffer->get(midiInfoBuffer, 3);
            printf("%02X %02X %02X\n", midiInfoBuffer[0], midiInfoBuffer[1], midiInfoBuffer[2]);

            //Not on, Note Off Abhängigkeit
            if(midiInfoBuffer[2] == 0x7F){
                notesOn = true;
            }else if(midiInfoBuffer[2] == 0x00){
                notesOn = false;
            }

            //Note to Volume
            //Midi Val to dB
            currentVolume = 40.f*log(static_cast<float>(midiInfoBuffer[2])/127.f);
            //std::cout << currentVolume << "\n";
            printf("%4.2fdB -> ", currentVolume);
            //fromDb conversion
            currentVolume = pow(10.f,currentVolume/20.f);
            //std::cout << currentVolume << "\n";
            printf("%04.4f\n", currentVolume);

            //Pan Influence
            //currentPan = panDir.get //from -1 to 1
            //currentAmpRight = currentAmp + (currentAmp / 100.f * currentPan);
            //currentAmpLeft = currentAmp - (currentAmp / 100.f * currentPan);

            //get number of channels
            //std::cout<<sampleSpecs.channels;
            //printf("Number of channels: %u\n",sampleSpecs.channels)
        }
    }
    if(notesOn){
        memcpy(out,in,size);
    }else{
        memset(out,0,size);
    }
}*/

ExamplesHandle inputToOutputWithMidi(const AlsaCardIdentifier &inCard, const AlsaCardIdentifier &outCard, const AlsaCardIdentifier &midiIn, unsigned int buffersize, unsigned int samplerate)
{
    // In this example, we just copy data from input to output
    // Samplerate and buffersize can be set. A handle to stop the
    // working threads is returned
    // To terminate this example, call:
    // Nl::terminateWorkingThread(hamdle)
    ExamplesHandle ret;

    // For Audio Input
    ret.inBuffer = createBuffer("InputBuffer");
    ret.audioInput = createInputDevice(inCard, ret.inBuffer, buffersize);
    ret.audioInput->setSamplerate(samplerate);

    ret.outBuffer = createBuffer("OutputBuffer");
    ret.audioOutput = createOutputDevice(outCard, ret.outBuffer, buffersize);
    ret.audioOutput->setSamplerate(samplerate);

    // We want midi as well
    ret.inMidiBuffer = createBuffer("MidiBuffer");
    ret.midiInput = createRawMidiDevice(midiIn, ret.inMidiBuffer);

    // DANGER!!!!
    // TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
    //		 input/output->start(), we seem to have a deadlock!
    // TODO: Consider implementing something like autostart for the threads in the
    //		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
    ret.audioInput->start();
    ret.audioOutput->start();
    ret.midiInput->start();

    std::cout << "MidiBufferSize: " << ret.midiInput->getAlsaMidiBufferSize() << std::endl;

    ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallbackWithMidi);

    return ret;
}

void midiSineWithMidiCallback(u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs)
{
    //for conversion scale factor of int32_t
    static int32_t scale = std::numeric_limits<int32_t>::is_signed ?
                ((1 << (sampleSpecs.bytesPerSample * 8)) / 2) :
                (1 << (sampleSpecs.bytesPerSample * 8));

    static int counter = 0;
    StopBlockTime sft(&sw, "val" + std::to_string(counter++));


    //static uint8_t velocity = 0;
    static float frequency = 880.0f;
    static float curVolumeFactor = 0.f;
    //static int32_t notesOn = 0;

    //unsigned char midiByteBuffer[3];
    bool reset = false;

    // We can get a buffer by its name, to access its data:
    auto midiBuffer = getBufferForName("MidiBuffer");

    // for volume effect
    if (midiBuffer) {
        unsigned char midiByteBuffer[3];
        while (midiBuffer->availableToRead() >= 3) {
            midiBuffer->get(midiByteBuffer, 3);
            printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
            if (midiByteBuffer[1] == 0x02) {
                curVolumeFactor = static_cast<float>(midiByteBuffer[2]) / static_cast<float>(std::numeric_limits<unsigned char>::max() / 2);
                printf("currVol-> %f\n", curVolumeFactor);
            }
            if (midiByteBuffer[2] == 0x7F && midiByteBuffer[1] != 0x02){
                frequency = pow(2.f, static_cast<double>((midiByteBuffer[1]-69)/12.f)) * 440.f;
                reset = true;
            }
        }
    }

    // note on - note off effect
    /*if(midiBuffer) {
        while(midiBuffer->availableToRead() >= 3) {
            midiBuffer->get(midiByteBuffer, 3);
            if(midiByteBuffer[0] == 0x90)
            {
                velocity = midiByteBuffer[2];
                if(velocity) {
                    notesOn++;
                    reset = true;   // nur vorläufig
                    frequency = pow(2.f, static_cast<double>((midiByteBuffer[1]-69)/12.f)) * 440.f;
                } else {
                    notesOn--;
                }
            } else if(midiByteBuffer[0] == 0x80) {
                notesOn--;
            }
        }
    }*/

    //if(notesOn > 0) {
        //int32_t samples[sampleSpecs.buffersizeInFramesPerPeriode];
        //sinewave<int32_t>(samples, frequency, reset, sampleSpecs);
        float samples[sampleSpecs.buffersizeInFramesPerPeriode];
        sinewave<float>(samples, frequency, reset, sampleSpecs);

        // Audio Stuff
        /*for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex) {
            for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
                //printf("%f\n", samples[frameIndex]);

                // Get float sample
                //float currentSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
                // Do something with float sample
                //currentSample *= curVolumeFactor;
                // Write back float sample
                    setSample(out, samples[frameIndex], frameIndex, channelIndex, sampleSpecs);
                //}
            }
        }*/

        for (unsigned int byte=0; byte<sampleSpecs.buffersizeInBytesPerPeriode; byte++) {
            //Is this not a frame with 2 Samples and 6 Bytes??
            unsigned int currentSample = (byte / (sampleSpecs.channels * sampleSpecs.bytesPerSample));
            unsigned int byteIndex = (byte % sampleSpecs.bytesPerSample);
            unsigned int byteIndexFr = (byte % sampleSpecs.bytesPerFrame);
            //unsigned int currentChannel = (byteIndexFr/sampleSpecs.bytesPerSample);

            if (sampleSpecs.isLittleEndian) {
                *out++ = static_cast<uint8_t>(uint32_t(static_cast<int32_t>(samples[currentSample]*curVolumeFactor*scale) >> ((byteIndex)*8)) & 0xFF);
            } else {
                *out++ = static_cast<uint8_t>(uint32_t(static_cast<int32_t>(samples[currentSample]*curVolumeFactor*scale) >> ((sampleSpecs.bytesPerSample-byteIndex-1)*8)) & 0xFF);
            }
        }
    //}
    //else {
    //  memset(out, 0, size);
    //}
}

ExamplesHandle midiSineWithMidi(const AlsaCardIdentifier &audioOutCard,
                        const AlsaCardIdentifier &midiInCard,
                        unsigned int buffersize,
                        unsigned int samplerate)
{
    ExamplesHandle ret;

    // Not needed, since we only playback here
    ret.inBuffer = nullptr;
    ret.audioInput = nullptr;

    // Lets create a buffer, which we have to pass to the output soundcard
    ret.outBuffer = createBuffer("AudioOutput");
    // Open soundcard, using above buffer
    ret.audioOutput = createOutputDevice(audioOutCard, ret.outBuffer, buffersize);

    // Configure Audio (if needed, or use default)
    //ret.audioOutput->setSampleFormat(...);
    ret.audioOutput->setSamplerate(samplerate);
    ret.audioOutput->setChannelCount(2);
    //ret.audioOutput->setSampleFormat("S16_LE");

    // We want midi as well
    ret.inMidiBuffer = createBuffer("MidiBuffer");
    ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

    // Start Audio and Midi Thread
    ret.audioOutput->start();
    ret.midiInput->start();

    std::cout << "MidiBufferSize: " << ret.midiInput->getAlsaMidiBufferSize() << std::endl;

    // Register a Callback
    ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, midiSineWithMidiCallback);

    return ret;
}



} // namespace Nl
} // namespace Examples
