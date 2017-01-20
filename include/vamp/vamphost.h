#ifndef VAMPHOST_H
#define VAMPHOST_H

#include <string>
#include <list>

#include "vamp-hostsdk/PluginHostAdapter.h"
#include "vamp-hostsdk/PluginInputDomainAdapter.h"
#include "vamp-hostsdk/PluginLoader.h"

#include "audio/samplespecs.h"

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

class VampHost
{
public:
	VampHost(const std::string &libraryName, const std::string& pluginName, unsigned int samplerate, unsigned int printIndex);
	virtual ~VampHost();

	bool initialize(unsigned int channels, unsigned int stepSize, unsigned int blockSize);
	void printFeatures(int frame, int sr, int output, Plugin::FeatureSet features, bool useFrames);

	//Remider: virtual might be to expensive for audio processing !?
	virtual void process(uint8_t *samples, const Nl::SampleSpecs& specs) = 0;

private:
	std::string m_pluginName;
	std::string m_libraryName;
	bool m_initialized;
	PluginLoader *m_loader;

	bool loadPlugin(const std::__cxx11::string &libraryName, const std::__cxx11::string &pluginName, unsigned int samplerate);
	unsigned int m_channels;
	unsigned int m_stepSize;
	unsigned int m_blockSize;

protected:
	Plugin *m_plugin;
	float **m_buffer;
	unsigned int m_printIndex;

};

#endif // VAMPHOST_H
