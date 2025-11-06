#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/opengl_includes.h>

void renderOcean(Data& data) {
	const Shader& oceanShader = *data.shaders.oceanShader;
	std::vector<GPUMesh>& ocean = *data.meshes.ocean;

	oceanShader.bind();

	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 viewMatrix = data.trackball->viewMatrix();
	glm::mat4 projectionMatrix = data.trackball->projectionMatrix();
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glm::mat4 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	glUniformMatrix4fv(oceanShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(oceanShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(oceanShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	glUniform1f(oceanShader.getUniformLocation("time"), data.time);
	glUniform1f(oceanShader.getUniformLocation("w"), data.oceanData.kt);
	glUniform1f(oceanShader.getUniformLocation("kx"), data.oceanData.kx);
	glUniform1f(oceanShader.getUniformLocation("ax"), data.oceanData.ax);
	glUniform1f(oceanShader.getUniformLocation("kz"), data.oceanData.kz);
	glUniform1f(oceanShader.getUniformLocation("az"), data.oceanData.az);
	glUniform1f(oceanShader.getUniformLocation("k1"), data.oceanData.k1);
	glUniform1f(oceanShader.getUniformLocation("k1dir"), data.oceanData.k1angle);
	glUniform1f(oceanShader.getUniformLocation("a1"), data.oceanData.a1);
	glUniform1f(oceanShader.getUniformLocation("k2"), data.oceanData.k2);
	glUniform1f(oceanShader.getUniformLocation("k2dir"), data.oceanData.k2angle);
	glUniform1f(oceanShader.getUniformLocation("a2"), data.oceanData.a2);
	glUniform1i(oceanShader.getUniformLocation("doSubdivide"), data.oceanData.doSubdivide);

	for (GPUMesh& m : ocean) {
		m.draw(oceanShader);
	}
}