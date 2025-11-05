#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/opengl_includes.h>

void renderOcean(InterfaceData interfaceData, Shader& oceanShader, std::vector<GPUMesh>& ocean) {
	oceanShader.bind();

	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 viewMatrix = interfaceData.trackball->viewMatrix();
	glm::mat4 projectionMatrix = interfaceData.trackball->projectionMatrix();
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glm::mat4 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	glUniformMatrix4fv(oceanShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(oceanShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(oceanShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	glUniform1f(oceanShader.getUniformLocation("time"), interfaceData.time);
	glUniform1f(oceanShader.getUniformLocation("a"), interfaceData.oceanData.amplitude);
	glUniform1f(oceanShader.getUniformLocation("kx"), interfaceData.oceanData.fx);
	glUniform1f(oceanShader.getUniformLocation("kz"), interfaceData.oceanData.fz);
	glUniform1f(oceanShader.getUniformLocation("w"), interfaceData.oceanData.ft);
	glUniform1i(oceanShader.getUniformLocation("doSubdivide"), interfaceData.oceanData.doSubdivide);

	for (GPUMesh& m : ocean) {
		m.draw(oceanShader);
	}
}