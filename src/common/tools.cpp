/***
  Copyright (c) 2018 Nonlinear Labs GmbH

  Authors: Pascal Huerst <pascal.huerst@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
***/

#include "common/tools.h"
#include "audio/audioalsaexception.h"
#include <cmath>
#include <sched.h>
#include <signal.h>

namespace Nl {


/** \ingroup Tools
 *
 * \brief Request and set realtime priority for this process
 * \return 0 on success, an negative errorcode otherwise
 *
 * Request and set realtime priority for this process. Also scheduler
 * policy is changed to round robin
 *
*/
int	requestRealtime(void)
{
	struct sched_param sched_param;
	int ret = sched_getparam(0, &sched_param);
	if (ret != 0) {
		printf("Scheduler getparam failed...\n");
		return ret;
	}
	sched_param.sched_priority = sched_get_priority_max(SCHED_RR);
	ret = sched_setscheduler(0, SCHED_RR, &sched_param);
	if (ret != 0) {
		printf("Scheduler setprio failed...\n");
		return ret;
	}

	printf("!!! Scheduler set to Round Robin with priority %i !!!\n", sched_param.sched_priority);
	return 0;
}

/** \ingroup Tools
 *
 * \brief Catch signals that might be thrown by alsa
 *
 * Alsa tends to throw a signal. This handler catches it and transforms it
 * into a \ref AudioAlsaException. To register a signal handler, call
 * \ref initSignalHandler()
 *
*/
void signalHandler(int signo)
{
	//throw AudioAlsaException(__PRETTY_FUNCTION__, "libalsa", -1, signo, "Library has thrown a Signal");
	std::cout << "### " << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl
			  << "### caught signal! terminating here..." << std::endl;

	exit (signo);
}

/** \ingroup Tools
 *
 * \brief Call this function to register a signal handler
 *
 * Alsa tends to throw a signal that terminates the program. This installs
 * a signal handler to catch those signals
 *
*/
void initSignalHandler()
{
	if (signal(SIGABRT, signalHandler) == SIG_ERR) {
		std::cout << "Can not register signalhandler!" << std::endl;
	}
}

} // namespace Nl
