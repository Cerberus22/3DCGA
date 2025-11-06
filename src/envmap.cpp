#include "structs.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <framework/opengl_includes.h>
#include <iostream>

void renderEnvMap(Data& data) {
	// Make ball at center
	GPUMesh& ball = data.meshes.ball->at(0);
	const Shader& shader = *data.shaders.envShader;
	const Shader& nightSkyShader = *data.shaders.nightSkyShader;

	Trackball* t = data.trackball;
	
	// Rotating here to turn the texture upright
	// Seems like the minimap shader has an upside down texture but the texcords are also upside down
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1), data.trackball->position()) * glm::rotate(glm::mat4(1), glm::pi<float>(), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), glm::pi<float>(), glm::vec3(1, 0, 0));
	glm::mat4 mvpMatrix = t->projectionMatrix() * t->viewMatrix() * modelMatrix;
	glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));



	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	nightSkyShader.bind();
	glUniformMatrix4fv(nightSkyShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(nightSkyShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(nightSkyShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	data.textures.envMap->bind(GL_TEXTURE0);
	glUniform1i(nightSkyShader.getUniformLocation("tex"), 0);

	ball.draw(nightSkyShader);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	modelMatrix = glm::mat4(1);
	mvpMatrix = t->projectionMatrix() * t->viewMatrix();
	normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));


	shader.bind();

	glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	glUniform3fv(shader.getUniformLocation("cameraPosition"), 1, glm::value_ptr(t->position()));

	data.textures.envMap->bind(GL_TEXTURE0);
	glUniform1i(shader.getUniformLocation("envMap"), 0);

	ball.draw(shader);
}