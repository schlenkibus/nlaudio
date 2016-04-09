#include "vamphostaubiotempo.h"
#include "vamp-hostsdk/PluginHostAdapter.h"
#include "vamp-hostsdk/PluginInputDomainAdapter.h"
#include "vamp-hostsdk/PluginLoader.h"

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

VampHostAubioTempo::VampHostAubioTempo(unsigned int samplerate) :
	basetype("vamp-aubio", "aubiotempo", samplerate)
{
}

void VampHostAubioTempo::process(uint8_t *samples, const Nl::SampleSpecs& specs)
{
	static unsigned long frameNo;
	frameNo++;

	RealTime adjustment = RealTime::zeroTime;

	for (unsigned int frameIndex=0; frameIndex<specs.buffersizeInFramesPerPeriode; ++frameIndex) {
		for (unsigned int channelIndex=0; channelIndex<specs.channels; ++channelIndex) {
			float currentSample = getSample(samples, frameIndex, channelIndex, specs);
			basetype::m_buffer[channelIndex][frameIndex] = currentSample;
		}
	}

	auto wrapper = dynamic_cast<PluginWrapper *>(m_plugin);
	if (wrapper) {
		// See documentation for
		// PluginInputDomainAdapter::getTimestampAdjustment
		PluginInputDomainAdapter *ida =
				wrapper->getWrapper<PluginInputDomainAdapter>();
		if (ida)
			adjustment = ida->getTimestampAdjustment();
	}

	auto rt = RealTime::frame2RealTime(frameNo, specs.samplerate);
	//auto features = plugin->process(plugbuf, rt);

	int outputNo = 0;

	basetype::printFeatures(RealTime::realTime2Frame(rt + adjustment, specs.samplerate),
				  specs.samplerate, outputNo, basetype::m_plugin->process(basetype::m_buffer, rt), false);
}

