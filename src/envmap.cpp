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

	Trackball* t = data.trackball;

	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 mvpMatrix = t->projectionMatrix() * t->viewMatrix();
	glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	shader.bind();

	glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	glUniform3fv(shader.getUniformLocation("cameraPosition"), 1, glm::value_ptr(t->position()));

	data.textures.envMap->bind(GL_TEXTURE0);
	glUniform1i(shader.getUniformLocation("envMap"), 0);

	std::cout << t->position().x << ", " << t->position().y << ", " << t->position().z << ", " << std::endl;

	ball.draw(shader);
}