#include "silence.h"


#include <audio/audioalsainput.h>
#include <audio/audioalsaoutput.h>
#include <audio/audioalsaexception.h>

#include <common/stopwatch.h>
#include <common/tools.h>

#include <stdio.h>

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

        Nl::AlsaAudioCardIdentifier audioOut(0,0,0, "USB Device");

        const int buffersize = 1024;
        const int samplerate = 48000;

        auto handle = Nl::Examples::silence(audioOut, buffersize, samplerate);

        while(::getchar() != 'q')
        {
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

            std::cout << "BufferCount: " << handle.audioOutput->getBufferCount() << std::endl;
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

// Silence Example
void silenceCallback(u_int8_t *out,
                     const SampleSpecs &sampleSpecs __attribute__ ((unused)),
                     SharedUserPtr ptr __attribute__ ((unused)))
{
    memset(out, 0, sampleSpecs.buffersizeInBytesPerPeriode);
}

JobHandle silence(const AlsaAudioCardIdentifier &audioOutCard,
                  unsigned int buffersize,
                  unsigned int samplerate)
{
    JobHandle ret;

    // Not nedded, since we only playback here w/o midi
    ret.inBuffer = nullptr;
    ret.audioInput = nullptr;
    ret.inMidiBuffer = nullptr;

    // Create an output buffer and an output device
    ret.outBuffer = createBuffer("AudioOutput");
    ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);

    // Configure audio device
    ret.audioOutput->setSamplerate(samplerate);

    // Start audio Thread
    ret.audioOutput->start();

    // Register a Callback
    SharedUserPtr ptr(new UserPtr("unused", nullptr));
    ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, silenceCallback, ptr);

    return ret;
}

} // namespace Nl
} // namespace Examples
