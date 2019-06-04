#pragma once

#include "glad\glad.h"

class Framebuffer {
protected:
	GLuint m_fbo;
	GLuint m_fboTexture;
private:

public:
	int width, height;

	virtual bool Init(unsigned int width, unsigned int height);

	virtual void BindForWriting();

	virtual void BindForReading(GLenum TextureUnit);
};