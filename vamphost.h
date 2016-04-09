#ifndef VAMPHOST_H
#define VAMPHOST_H

#include <string>
#include <list>
#include "vamp-hostsdk/PluginHostAdapter.h"
#include "vamp-hostsdk/PluginInputDomainAdapter.h"
#include "vamp-hostsdk/PluginLoader.h"
//Das ding representiert einen host, der ein plugin laden kann

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

struct VampPluginIdentifier {
	std::string libraryName;
	std::string pluginname;
};

class VampHost
{
public:
	VampHost();

	bool loadPlugin(const VampPluginIdentifier& id);
	bool initialize(unsigned int channels, unsigned int stepSize, unsigned int blockSize);

private:
	PluginLoader *loader;
	Plugin *plugin;


};

#endif // VAMPHOST_H
