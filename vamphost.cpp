#include "vamphost.h"

VampHost::VampHost()
{
}

bool VampHost::loadPlugin(const VampPluginIdentifier &id)
{
	loader = PluginLoader::getInstance();
	PluginLoader::PluginKey key = loader->composePluginKey(libraryName, pluginName);
	plugin = loader->loadPlugin(key, samplerate, PluginLoader::ADAPT_ALL_SAFE);
	if (!plugin)
		return false;

	Plugin::OutputList outputs = plugin->getOutputDescriptors();

	if (outputs.empty()) {
		std::cerr << "ERROR: Plugin has no outputs!" << std::endl;
	}

	for (int i=0; i<outputs.size(); i++) {
		Plugin::OutputDescriptor od = outputs[i];
		std::cerr << "Output[" << i << "] is: \"" << od.identifier << "\"" << std::endl;
	}

	return true;
}

bool VampHost::initialize(unsigned int channels, unsigned int stepSize, unsigned int blockSize)
{
	if (!plugin->initialise(channels, stepSize, blockSize)) {
		std::cerr << "ERROR: Plugin initialise (channels = " << channels
				  << ", stepSize = " << stepSize << ", blockSize = "
				  << blockSize << ") failed." << std::endl;
		return false;
	}

	return true;
}


