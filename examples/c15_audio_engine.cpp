/*! \mainpage NlAudio - A lightwight C++ wrapper for alsa and jack
 *
 * \section intro_sec This documentation is split into the following subsections:
 * \ref Audio\n
 * \ref Midi\n
 * \ref Tools\n
 * \ref Factory\n
 * \n
 * More has to be done, but first a want to make a proper library from that
 *
 * \section install_sec Installation
 *
 * sudo make install blub und so
 *
 * \subsection step1 Step 1: Opening the box
 *
 * This could be a subsection...
 *
 */

#include <iostream>
#include <ostream>
#include <stdio.h>

#include <audio/audioalsainput.h>
#include <audio/audioalsaoutput.h>
#include <audio/audiofactory.h>
#include <audio/audioalsa.h>
#include <audio/alsa/alsacardidentifier.h>
#include <audio/audioalsaexception.h>

#include <midi/midi.h>
#include <midi/rawmididevice.h>

#include <common/tools.h>
#include <common/stopwatch.h>
#include <common/blockingcircularbuffer.h>

// temporary...
#include "c15_audio_engine/minisynth.h"
// new:
#include "c15_audio_engine/dsp_host_handle.h"


#include <chrono>

using namespace std;

//TODO: Glbal Variables are bad (even in a namespace)
Nl::StopWatch sw("AudioCallback");

//
//#include "c15_audio_engine/soundgenerator.h"

int main()
{
    try
    {
        auto availableDevices = Nl::getDetailedCardInfos();
        for(auto it=availableDevices.begin(); it!=availableDevices.end(); ++it)
            std::cout << *it << std::endl;

        auto availableDevs = Nl::AlsaCardIdentifier::getCardIdentifiers();
        for (auto it=availableDevs.begin(); it!=availableDevs.end(); ++it)
            std::cout << *it << std::endl;

        Nl::AlsaCardIdentifier audioIn(1,0,0, "USB Device");
        Nl::AlsaCardIdentifier audioOut(1,0,0, "USB Device");
        Nl::AlsaCardIdentifier midiIn(2,0,0, "Midi In");

        const int buffersize = 256;
        const int samplerate = 48000;

        //auto handle = Nl::Examples::inputToOutput(audioIn, audioOut, buffersize, samplerate);
        //auto handle = Nl::Examples::silence(audioOutDevice, buffersize, samplerate);
        //auto handle = Nl::Examples::midiSine(audioOut, midiIn, buffersize, samplerate);
        //auto handle = Nl::Examples::midiSineWithMidi(audioOut, midiIn, buffersize, samplerate);
        //auto handle = Nl::Examples::inputToOutputWithMidi(audioIn, audioOut, midiIn, buffersize, samplerate);

        //this is for the MiniSynth
        auto handle = Nl::MINISYNTH::miniSynthMidiControl(audioIn, audioOut, midiIn, buffersize, samplerate);

        // first try of dsp_host
//        auto handle = Nl::DSP_HOST_HANDLE::dspHostTCDControl(audioIn, audioOut, midiIn, buffersize, samplerate);

        // Wait for user to exit by pressing 'q'
        // Print buffer statistics on other keys
        // TODO: We might have a deadlock here:
        //		 sw.printSummary() holds a lock
        //		 audioXX->getStats() holds a lock
        //		 inMidiBuffer->getStats() holds a lock
        //	     The calls should happen in this order. Otherwise we trigger
        //		 a deadlock with the audio callback.

		//while(true) {
#if 1
        while(getchar() != 'q')
        {
//            std::cout << getchar() << std::endl;
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

            //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
#endif

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
}

