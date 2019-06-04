#pragma once

#include "Rendering\Framebuffer.h"

class ShadowMap : public Framebuffer {
public:
	bool Init(unsigned int width, unsigned int height);
};