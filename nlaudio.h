#pragma once

#include "nlaudioconfig.h"

class NlAudio
{
public:
	NlAudio();


	void open();
	void close();

	void registerCallback();
	void unregisterCallback();

	void setConfiguration(const NlAudioConfig& config);











};
