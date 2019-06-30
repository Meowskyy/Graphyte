#pragma once

#include "glad\glad.h"

namespace Graphyte {
	class Framebuffer {
	protected:
		GLuint framebuffer;
		GLuint rbo;
	public:
		int width, height;
		GLuint framebufferTexture;

		virtual bool Init(unsigned int width, unsigned int height);
		virtual void BindForWriting();
		virtual void BindForReading(GLenum TextureUnit);
	};
}