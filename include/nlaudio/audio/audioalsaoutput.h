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

#pragma once

#include "audioalsa.h"

namespace Nl {

class AudioAlsaOutput : public AudioAlsa
{
public:
	typedef AudioAlsa basetype;

	AudioAlsaOutput(const AlsaCardIdentifier& card, SharedBufferHandle buffer);

	virtual void open();
	virtual void start();
	virtual void stop();
	virtual void init();

	static void worker(SampleSpecs specs, AudioAlsaOutput *ptr);
};

typedef std::shared_ptr<AudioAlsaOutput> SharedAudioAlsaOutputHandle;

} // Namespace Nl
