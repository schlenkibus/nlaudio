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

#include "audio/alsa/alsacardinfo.h"

#include <ostream>
#include <alsa/asoundlib.h>

namespace Nl {

std::ostream& operator<<(std::ostream& lhs, const AlsaCardInfo& rhs)
{
	lhs << " CardId:      " << rhs.cardId << std::endl <<
		   " Name:        " << rhs.name << std::endl <<
		   " Long name:   " << rhs.longname << std::endl <<
		   " Mixer name:  " << rhs.mixername << std::endl <<
		   " Id:          " << rhs.id << std::endl <<
		   " Driver:      " << rhs.driver << std::endl <<
		   " Components:  " << rhs.components << std::endl <<
		   " Devices:     " << rhs.devices.size() << std::endl;

	for (auto it=rhs.devices.begin(); it!=rhs.devices.end(); ++it) {

		lhs << "   " << *it << std::endl;

		for (auto subit=it->subdevices.begin(); subit!=it->subdevices.end(); ++subit) {
			lhs << *subit << std::endl;
		}
	}
	return lhs;
}

/** \ingroup Audio
 *
 * \brief Static function, that returns available devices
 * \return All available cards on the plattform, represented by a list of \ref AlsaCardInfo
 *
 * Returns a list of available cards on the plattform, represented by \ref AlsaCardInfo objects,
 * stored in a std::list
 *
 */
std::list<AlsaCardInfo> getDetailedCardInfos()
{
	int card = -1;
	int dev = -1;

	std::list<AlsaCardInfo> ret;

	while (snd_card_next(&card) >= 0 && card >= 0) {

		AlsaCardInfo currentCardInfo;

		std::string controlName = "hw:" + std::to_string(card);
		snd_ctl_t *handle;
		if (snd_ctl_open(&handle, controlName.c_str(), 0) == 0) {

			snd_ctl_card_info_t *cardinfo;
			snd_ctl_card_info_alloca(&cardinfo);
			if (snd_ctl_card_info(handle, cardinfo) == 0) {

				//snd_ctl_elem_info_get_interface()

				dev = -1;
				while (snd_ctl_pcm_next_device(handle, &dev) == 0 && dev >= 0) {

					snd_pcm_info_t *pcminfo;
					snd_pcm_info_alloca(&pcminfo);

					snd_pcm_info_set_device(pcminfo, dev);
					snd_pcm_info_set_subdevice(pcminfo, 0);

					snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
					snd_pcm_info_set_stream(pcminfo, stream);

					if (snd_ctl_pcm_info(handle, pcminfo) == 0) {

						AlsaDeviceInfo currentDeviceInfo;

						int count = snd_pcm_info_get_subdevices_count(pcminfo);
						for (int i=0; i<count; i++) {
							snd_pcm_info_set_subdevice(pcminfo, i);

							AlsaSubdeviceInfo currentSubdeviceInfo;
							currentSubdeviceInfo.subdeviceId = i;
							currentSubdeviceInfo.name = snd_pcm_info_get_subdevice_name(pcminfo);
							currentDeviceInfo.subdevices.push_back(currentSubdeviceInfo);
						}

						currentDeviceInfo.deviceId = dev;
						currentDeviceInfo.id = snd_pcm_info_get_id(pcminfo); // USB Audio
						currentDeviceInfo.name = snd_pcm_info_get_name(pcminfo); // USB Audio #1

						currentCardInfo.devices.push_back(currentDeviceInfo);
						currentCardInfo.cardId = card;
						currentCardInfo.components = snd_ctl_card_info_get_components(cardinfo); // USB0a92:0054
						currentCardInfo.driver = snd_ctl_card_info_get_driver(cardinfo); // USB-Audio
						currentCardInfo.id = snd_ctl_card_info_get_id(cardinfo); // nano
						currentCardInfo.longname = snd_ctl_card_info_get_longname(cardinfo); // ESI Audiotechnik GmbH Dr. DAC nano at usb-0000:00:14.0-1, full speed
						currentCardInfo.name = snd_ctl_card_info_get_name(cardinfo); // Dr. DAC nano
						currentCardInfo.mixername = snd_ctl_card_info_get_mixername(cardinfo); // USB Mixer
					}
				}
			} else {
				//TODO: Throw axception
				//std::cout << "EEE2" << std::endl;
			}
		} else {
			//TODO: Throw exception
			//std::cout << "EEE1" << std::endl;
		}
		ret.push_back(currentCardInfo);
		snd_ctl_close(handle);
	}

	return ret;
}

} // namespace Nl
