#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/shader.h>
#include <iostream>

void renderOnPlanetScene(InterfaceData interfaceData, Shader& shader, Shader& minimapShader, GLuint& minimapTexture, GLuint& minimapFramebuffer, std::vector<GPUMesh>& cup, GPUMesh& quad, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	glm::mat4 modelMatrix = glm::mat4(1);
	
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	// Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
	// https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
	glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	// Render scene
	for (GPUMesh& mesh : cup) {
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

	// Render minimap texture
	Trackball* t = interfaceData.trackball;
	viewMatrix = glm::lookAt(glm::vec3(t->lookAt().x,10,t->lookAt().z), t->lookAt(), glm::vec3(0, 0, 1));
	mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glBindFramebuffer(GL_FRAMEBUFFER, minimapFramebuffer);

	glViewport(0, 0, 1024, 1024);

	glClearDepth(1.f);
	glClearColor(1.f, 0.f, 1.f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for (GPUMesh& mesh : cup) {
		shader.bind();
		glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
		glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

		glUniform3fv(shader.getUniformLocation("kd"), 1, glm::value_ptr(interfaceData.cupMaterial.m.kd));
		glUniform1f(shader.getUniformLocation("rho"), interfaceData.cupMaterial.rho);
		glUniform1f(shader.getUniformLocation("sigma"), interfaceData.cupMaterial.sigma);
		glUniform1f(shader.getUniformLocation("ambientCoeff"), 0.3);

		glUniform3fv(shader.getUniformLocation("cameraPosition"), 1, glm::value_ptr(interfaceData.trackball->position()));

		mesh.draw(shader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1800, 900);


	// Render minimap
	modelMatrix = glm::translate(glm::mat4(1), glm::vec3(0.8, -0.7, 0)) 
				* glm::rotate(glm::mat4(1), glm::pi<float>()/2, glm::vec3(1,0,0))
				* glm::scale(glm::mat4(1), glm::vec3(0.125, 0.25, 0.25));
	normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	// Skip viewMatrix to make quad fixed on screen
	mvpMatrix = modelMatrix;

	minimapShader.bind();
	glUniformMatrix4fv(minimapShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(minimapShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(minimapShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, minimapTexture);

	glUniform1i(minimapShader.getUniformLocation("minimapTexture"), 0);
	
	quad.draw(minimapShader);
}
