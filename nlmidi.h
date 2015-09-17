#pragma once

typedef std::string devicename_t;


class NlMidi
{
public:
	virtual void open() = 0;
	virtual void close() = 0;

	virtual void start() = 0;
	virtual void stop() = 0;


};
