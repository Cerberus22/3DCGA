#include <framework/opengl_includes.h>
#include "mesh.h"

GLuint framebuffer;
GLuint normalMap;

GLuint renderSmoothNormalMap() {
	// Setup and stuff
	glGenFramebuffers(1, &framebuffer);

	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1024, 1024, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);

	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	// Generate texture
	Shader texShader;
	/// build sader

	texShader.bind();

	glViewport(0, 0, 1024, 1024);

	GLuint trianglesvao;

	glDrawElements(GL_TRIANGLES, 0, 3, &trianglesvao);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return normalMap;
}

void destruct() {
	glDeleteTextures(1, &normalMap);
	glDeleteFramebuffers(1, &framebuffer);
}