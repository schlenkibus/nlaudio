#pragma once

#include <audio/audiofactory.h>

using namespace Nl;

struct ExamplesHandle {
	WorkingThreadHandle workingThreadHandle;
	SharedAudioHandle audioInput;
	SharedAudioHandle audioOutput;
	SharedRawMidiDeviceHandle midiInput;
	SharedRawMidiDeviceHandle midiOutput;
	SharedBufferHandle inBuffer;
	SharedBufferHandle outBuffer;
	SharedBufferHandle inMidiBuffer;
};
