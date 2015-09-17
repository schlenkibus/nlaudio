#pragma once

#include <cstdint>
#include <exception>
#include <string>

class NlAudioException : std::exception
{
public:
	NlAudioException(std::string what) : msg(what) {}
private:
	virtual const char* what() const throw() { return msg.c_str(); }
	std::string msg;
};


class NlAudioConfig
{
public:
	NlAudioConfig();


private:

	uint32_t m_sampleRate;
	uint32_t m_bitDepth;
	uint8_t m_channels;

};
