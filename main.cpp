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

//#define OSCGUI
#include <QApplication>     //for the gui
#include "oscshapeui.h"

#include <iostream>
#include <ostream>

#include "audioalsainput.h"
#include "audioalsaoutput.h"

#include "midi.h"
#include "rawmididevice.h"

#include <stdio.h>

#include "audiofactory.h"
#include "examples.h"
#include "minisynth.h"
#include "effects.h"

#include "tools.h"

#include "audioalsa.h" //AlsaCardIdentifier -> TODO: Put this in extra file

#include "stopwatch.h"

#include "alsacardidentifier.h"

#include "blockingcircularbuffer.h"

#include "audioalsaexception.h"

#include "alsacardidentifier.h"

using namespace std;

//TODO: Glbal Variables are bad (even in a namespace)
Nl::StopWatch sw("AudioCallback");

//
#include "soundgenerator.h"

#if OSCGUI
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    OscShapeUI w;
    w.show();
#else
int main()
{
#endif

	try {
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

        //this is for the Effects
//        auto handle = Nl::EFFECTS::effectsMidiControl(audioIn, audioOut, midiIn, buffersize, samplerate);

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

#ifdef OSCGUI
    return a.exec();
#endif
}

