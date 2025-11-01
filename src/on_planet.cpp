#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"
#include <framework/shader.h>
#include <iostream>

void renderOnPlanetScene(Shader& shader, std::vector<GPUMesh>& cup, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	const glm::mat4 modelMatrix = glm::mat4(1);
	
	const glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	// Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
	// https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html

	for (GPUMesh& mesh : cup) {
		const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

		shader.bind();
		glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
		glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
		//if (mesh.hasTextureCoords()) {
		//    m_texture.bind(GL_TEXTURE0);
		//    glUniform1i(shader.getUniformLocation("colorMap"), 0);
		//    glUniform1i(shader.getUniformLocation("hasTexCoords"), GL_TRUE);
		//    glUniform1i(shader.getUniformLocation("useMaterial"), GL_FALSE);
		//}
		//else {
		//    glUniform1i(shader.getUniformLocation("hasTexCoords"), GL_FALSE);
		//    glUniform1i(shader.getUniformLocation("useMaterial"), m_useMaterial);
		//}
		mesh.draw(shader);
	}
}
