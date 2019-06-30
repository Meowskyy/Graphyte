#pragma once

#include "Rendering\Framebuffer.h"

namespace Graphyte {
	class ShadowMap : public Framebuffer {
	public:
		bool Init(unsigned int width, unsigned int height);
		void BindForWriting();
	};
}