#include "vamphost.h"

VampHost::VampHost(const std::string &libraryName, const std::string &pluginName, unsigned int samplerate) :
	m_libraryName(libraryName),
	m_pluginName(pluginName),
	m_initialized(false),
	m_loader(nullptr),
	m_plugin(nullptr),
	m_buffer(nullptr),
	m_channels(-1),
	m_blockSize(-1),
	m_stepSize(-1)
{
	if (!loadPlugin(libraryName, pluginName, samplerate))
		std::cerr << "ERROR: Can not load plugin!" << std::endl;
}

VampHost::~VampHost()
{
	//for (int i=0; i<m_channels; i++) delete[] m_buffer[i];
	//delete[] m_buffer;
}

bool VampHost::loadPlugin(const std::string &libraryName, const std::string &pluginName, unsigned int samplerate)
{

	m_loader = PluginLoader::getInstance();
	PluginLoader::PluginKey key = m_loader->composePluginKey(libraryName, pluginName);
	m_plugin = m_loader->loadPlugin(key, samplerate, PluginLoader::ADAPT_ALL_SAFE);
	if (!m_plugin)
		return false;

	Plugin::OutputList outputs = m_plugin->getOutputDescriptors();

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
	if (!m_plugin || !m_loader) {
		std::cerr << "ERROR: Can not initialize plugin, because it's not loaded!" << std::endl;
		return false;
	}

	if (!m_plugin->initialise(channels, stepSize, blockSize)) {
		std::cerr << "ERROR: Plugin initialise (channels = " << channels
				  << ", stepSize = " << stepSize << ", blockSize = "
				  << blockSize << ") failed." << std::endl;
		return false;
	}

	m_buffer = new float*[channels];
	for (int i=0; i<channels; i++) m_buffer[i] = new float[blockSize+2];

	return true;
}

void VampHost::printFeatures(int frame, int sr, int output, Plugin::FeatureSet features, bool useFrames)
{
	for (unsigned int i = 0; i < features[output].size(); ++i) {

		if (useFrames) {

			int displayFrame = frame;

			if (features[output][i].hasTimestamp) {
				displayFrame = RealTime::realTime2Frame
						(features[output][i].timestamp, sr);
			}

			std::cout << displayFrame;

			if (features[output][i].hasDuration) {
				displayFrame = RealTime::realTime2Frame
						(features[output][i].duration, sr);
				std::cout << "," << displayFrame;
			}

			std::cout  << ":";

		} else {
			static RealTime oldRt = RealTime::zeroTime;
			RealTime rt = RealTime::frame2RealTime(frame, sr);
			RealTime delta = rt - oldRt;
			oldRt = rt;

			if (features[output][i].hasTimestamp) {
				rt = features[output][i].timestamp;
			}

			std::cout << delta.toString() << " BPM=" << 120.f / static_cast<double>((double)delta.msec()/1000.f + (double)delta.usec()/1000000.f + delta.sec);

			if (features[output][i].hasDuration) {
				rt = features[output][i].duration;
				std::cout << "," << rt.toString();
			}

			std::cout << ":";
		}

		for (unsigned int j = 0; j < features[output][i].values.size(); ++j) {
			std::cout << " " << features[output][i].values[j];
		}
		std::cout << " " << features[output][i].label;
		std::cout << std::endl;
	}
}

