#pragma once

#include <audio/audiofactory.h>

namespace Nl {
namespace Examples {

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

}
}
