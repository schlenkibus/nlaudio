#if 0

#pragma once

#include "audiojack.h"

namespace Nl {

class AudioJackInput : public Audio
{
public:
	typedef Audio basetype;

	AudioJackInput(const AlsaCardIdentifier& card, SharedBufferHandle buffer);
};

} // namespace Nl

#endif
