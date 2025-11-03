#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/shader.h>
#include <iostream>

void renderOnPlanetScene(InterfaceData interfaceData, Shader& shader, std::vector<GPUMesh>& cup, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
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

		glUniform3fv(shader.getUniformLocation("kd"), 1, glm::value_ptr(interfaceData.cupMaterial.m.kd));
		glUniform1f(shader.getUniformLocation("rho"), interfaceData.cupMaterial.rho);
		glUniform1f(shader.getUniformLocation("sigma"), interfaceData.cupMaterial.sigma);
		glUniform1f(shader.getUniformLocation("ambientCoeff"), 0);
		
		glUniform3fv(shader.getUniformLocation("cameraPosition"), 1, glm::value_ptr(interfaceData.trackball->position()));
		
		mesh.draw(shader);
	}
}
