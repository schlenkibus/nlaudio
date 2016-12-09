#include "tools.h"
#include <cmath>

namespace Nl {

#include <sched.h>
#include <errno.h>
#include <string.h>

/** \ingroup Tools
 *
 * \brief Request and set realtime priority for this process
 * \return 0 on success, an negative errorcode otherwise
 *
 * Request and set realtime priority for this process. Also scheduler
 * policy is changed to round robin
 *
*/
int requestRealtime(void)
{
		struct sched_param sched_param;
		int ret = sched_getparam(0, &sched_param);

        if (ret != 0) {
                printf("Scheduler getparam failed; %i...\n", ret);
				return ret;
		}

		sched_param.sched_priority = sched_get_priority_max(SCHED_RR);

        ret = sched_setscheduler(0, SCHED_RR, &sched_param);

        if (ret != 0) {
                printf("Scheduler setprio failed; %i ...\n", ret);
                printf("%i\n", errno);
                printf("%s\n", strerror(errno));
				return ret;
		}

		printf("!!! Scheduler set to Round Robin with priority %i !!!\n", sched_param.sched_priority);
		return 0;
}

/*
auto getTypeForAudioSpec(const SampleSpecs_t &specs)
{

	if (specs.isFloat)
		return 1.f;

	// Signed
	if (specs.isSigned) {
		if (specs.bytesPerSample == 1)
			return (int8_t)0;
		else if (specs.bytesPerFrame == 2)
			return (int16_t)0;
		else if (specs.bytesPerSample == 3 || specs.bytesPerSample == 4)
			return (int32_t)0;
	// Unsigned
	} else {
		if (specs.bytesPerSample == 1)
			return (uint8_t)0;
		else if (specs.bytesPerFrame == 2)
			return (uint16_t)0;
		else if (specs.bytesPerFrame == 3 || specs.bytesPerFrame == 4)
			return (uint32_t)0;
	}
}
*/



} // namespace Nl
