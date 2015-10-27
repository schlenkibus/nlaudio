#include <iostream>
#include <ostream>

#include "audioalsainput.h"
#include "audioalsaoutput.h"

#include "midi.h"
#include "rawmididevice.h"

#include <stdio.h>
#include <sched.h>

#include <cmath>
#include <sys/resource.h> // Nicelevel

#include "blockingcircularbuffer.h"
#include "audiofactory.h"
#include "examples.h"

#include "tools.h"

using namespace std;

void callback(uint8_t *out, size_t size, const Nl::SampleSpecs_t &sampleSpecs)
{
    unsigned char midiByteBuffer[3];
    static uint8_t velocity = 0;
    static double frequency = 0;
    bool reset = false;

    size_t debugCounter = 0;

    auto midiBuffer = Nl::getBufferForName("MidiBuffer");
    if (midiBuffer)
        while (midiBuffer->availableToRead() >= 3) {
            midiBuffer->get(midiByteBuffer, 3);
            if (midiByteBuffer[0] == 0x90) {
                velocity = midiByteBuffer[2];
                double newFrequency = pow(2.f, static_cast<double>((midiByteBuffer[1]-69)/12.f)) * 440.f;
                if (newFrequency != frequency) {
                    frequency = newFrequency;
                    reset = true;
                }
            } else {
                velocity = 0;
            }
            printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
            printf("V=%i f=%f\n", velocity, frequency);
        }

    if (velocity) {
        int32_t samples[sampleSpecs.buffersizeInSamples];
        Nl::sinewave<int32_t>(samples, sampleSpecs.buffersizeInSamples, frequency, 48000, reset);

        for (unsigned int sample=0; sample<sampleSpecs.buffersizeInSamples; sample++) {
            for (unsigned int channel=0; channel<sampleSpecs.channels; channel++) {
                for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
                    if (sampleSpecs.isLittleEndian) {
                        *out = static_cast<uint8_t>((samples[sample] >> ((byte+1)*8)) & 0xFF);
                    } else {
                        *out = static_cast<uint8_t>((samples[sample] >> ((sampleSpecs.bytesPerSample-byte)*8)) & 0xFF);
                    }
                    out++;
                    debugCounter++;
                }
            }
        }
    } else {
        memset(out, 0, size);
    }


//    printf("size=%d debugCounter=%d\n", size, debugCounter);

}

int main()
{
    try {
        // Lets just open the default device:
        auto audioOutputBuffer = Nl::createBuffer("AudioOutput");

        //auto audioOutput = Nl::createDefaultOutputDevice(audioOutputBuffer);
        //auto audioOutput = Nl::createOutputDevice("hw:2,0", audioOutputBuffer, 512); // Audiobox USB
        //auto audioOutput = Nl::createOutputDevice("hw:2,0,1", audioOutputBuffer, 512); // Traktor Audio 4 DJ
        auto audioOutput = Nl::createOutputDevice("hw:2,0", audioOutputBuffer, 1024); // Traktor Audio 2

        // Configure Audio (if needed, or use default)
        //audioOutput->setSampleFormat(...);
        audioOutput->setSamplerate(48000);

        // We want midi as well
        auto midiBuffer = Nl::createBuffer("MidiBuffer");
        auto midiInput = Nl::createRawMidiDevice("hw:1,0", midiBuffer);

        // Start Audio and Midi Thread
        audioOutput->start();
        midiInput->start();

        // Register a Callback
        auto threadHandle = Nl::registerOutputCallbackOnBuffer(audioOutputBuffer, callback);

        // Wait for user to exit by pressing 'q'
        // Print buffer statistics on other keys
        while(getchar() != 'q') {
            if (audioOutput) std::cout << "Output Statistics:" << std::endl
                                       << audioOutput->getStats() << std::endl;
        }

        // Tell worker thread to cleanup and quit
        Nl::terminateWorkingThread(threadHandle);

    } catch (Nl::AudioAlsaException& e) {
        std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
    } catch(...) {
        std::cout << "### Exception ###" << std::endl << "  default" << std::endl;
    }

    return 0;
}

