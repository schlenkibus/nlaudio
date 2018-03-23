#include "midisine.h"

#include <audio/audioalsainput.h>
#include <audio/audioalsaoutput.h>
#include <audio/audioalsaexception.h>

#include <common/stopwatch.h>
#include <common/tools.h>

Nl::StopWatch sw("AudioCallback", 10000);

int main()
{
    try
    {
        auto availableDevices = Nl::getDetailedCardInfos();
        for(auto it=availableDevices.begin(); it!=availableDevices.end(); ++it)
            std::cout << *it << std::endl;

        auto availableDevs = Nl::AlsaAudioCardIdentifier::getCardIdentifiers();
        for (auto it=availableDevs.begin(); it!=availableDevs.end(); ++it)
            std::cout << *it << std::endl;

        Nl::AlsaAudioCardIdentifier audioIn(1,0,0, "USB Device");
        Nl::AlsaAudioCardIdentifier audioOut(1,0,0, "USB Device");
        Nl::AlsaMidiCardIdentifier midiIn(2,0,0, "Midi In");

        const int buffersize = 256;
        const int samplerate = 48000;

        auto handle = Nl::Examples::midiSine(audioOut, midiIn, buffersize, samplerate);

        while(getchar() != 'q')
        {
            std::cout << sw << std::endl;

            if (handle.audioOutput) std::cout << "Audio: Output Statistics:" << std::endl
                                              << handle.audioOutput->getStats() << std::endl;
            if (handle.audioInput) std::cout << "Audio: Input Statistics:" << std::endl
                                             << handle.audioInput->getStats() << std::endl;

            if (handle.inMidiBuffer) {
                unsigned long rxBytes, txBytes;
                handle.inMidiBuffer->getStat(&rxBytes, &txBytes);
                std::cout << "Midi: Input Statistics:" << std::endl
                          << "rxBytes=" << rxBytes << "  txBytes=" << txBytes << std::endl;
            }

            std::cout << "BufferCount: " << handle.audioInput->getBufferCount() << std::endl;
        }

        // Tell worker thread to cleanup and quit
        Nl::terminateWorkingThread(handle.workingThreadHandle);
        if (handle.audioOutput) handle.audioOutput->stop();
        if (handle.audioInput) handle.audioInput->stop();

    } catch (Nl::AudioAlsaException& e) {
        std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
    } catch(...) {
        std::cout << "### Exception ###" << std::endl << "  default" << std::endl;
    }

    return 0;
}

namespace Nl {
namespace Examples {

void midiSineCallback(u_int8_t *out,
                      const SampleSpecs &sampleSpecs,
                      SharedUserPtr ptr __attribute__ ((unused)))
{
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
        memset(out, 0, sampleSpecs.buffersizeInBytesPerPeriode);
    }
}

// Midi Sine example
JobHandle midiSine(const AlsaAudioCardIdentifier &audioOutCard,
                   const AlsaMidiCardIdentifier &midiInCard,
                   unsigned int buffersize,
                   unsigned int samplerate)
{
    JobHandle ret;

    // Not needed, since we only playback here
    ret.inBuffer = nullptr;
    ret.audioInput = nullptr;

    // Lets create a buffer, which we have to pass to the output soundcard
    ret.outBuffer = createBuffer("AudioOutput");
    // Open soundcard, using above buffer
    ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);

    // Configure Audio (if needed, or use default)
    //ret.audioOutput->setSampleFormat(...);
    //ret.audioOutput->setSamplerate(samplerate);
    ret.audioOutput->setChannelCount(2);
    //ret.audioOutput->setSampleFormat("S16_LE");

    // We want midi as well
    ret.inMidiBuffer = createBuffer("MidiBuffer");
    auto midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

    // Start Audio and Midi Thread
    ret.audioOutput->start();
    midiInput->start();

    std::cout << "MidiBufferSize: " << midiInput->getAlsaMidiBufferSize() << std::endl;

    // Register a Callback
    SharedUserPtr ptr(new UserPtr("unused", nullptr));
    ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, midiSineCallback, ptr);

    return ret;
}

} // namespace Nl
} // namespace Examples
