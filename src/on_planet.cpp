#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/shader.h>
#include <iostream>

std::vector<glm::vec3> computeTangents(std::vector<Vertex> vertices, std::vector<glm::uvec3> triangles) {
	std::vector<glm::vec3> tangents;

	for (int i = 0; i < triangles.size(); i++) {
		glm::uvec3 triangle = triangles.at(i);
		glm::vec3 p0 = vertices.at(triangle.x).position;
		glm::vec3 p1 = vertices.at(triangle.y).position;
		glm::vec3 p2 = vertices.at(triangle.z).position;

		glm::vec2 uv0 = vertices.at(triangle.x).texCoord;
		glm::vec2 uv1 = vertices.at(triangle.y).texCoord;
		glm::vec2 uv2 = vertices.at(triangle.z).texCoord;

		glm::vec3 edge1 = p1 - p0;
		glm::vec3 edge2 = p2 - p0;

		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;
		
		float f = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
	
		tangents.push_back(tangent);
	}
	return tangents;
}

void renderOnPlanetScene(InterfaceData interfaceData, Shader& simpleShader, Shader& advancedShader, Shader& minimapShader, GLuint& minimapTexture, GLuint& minimapFramebuffer, std::vector<GPUMesh>& cup, GPUMesh& quad, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	std::vector<glm::vec3> tangents = computeTangents(cup.at(4).vertices, cup.at(4).triangles);

	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	// Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
	// https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
	glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
	Shader* shader;

	// Render scene
	for (int i = 0; i < cup.size(); i++) {
		GPUMesh& mesh = cup.at(i);
		if (i > 3) shader = &simpleShader;
		else shader = &advancedShader;

		shader->bind();
		glUniformMatrix4fv(shader->getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
		glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix3fv(shader->getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

		glUniform3fv(shader->getUniformLocation("kd"), 1, glm::value_ptr(interfaceData.cupMaterial.m.kd));
		glUniform1f(shader->getUniformLocation("ambientCoeff"), 0);

		glUniform3fv(shader->getUniformLocation("cameraPosition"), 1, glm::value_ptr(interfaceData.trackball->position()));

		glUniform3fv(shader->getUniformLocation("lightPosition"), 1, glm::value_ptr(glm::vec3(0, 4, 2)));
		glUniform3fv(shader->getUniformLocation("lightColor"), 1, glm::value_ptr(glm::vec3(1)));

		if (i > 3) {
			glUniform3fv(shader->getUniformLocation("ks"), 1, glm::value_ptr(interfaceData.cupMaterial.m.ks));
			glUniform1f(shader->getUniformLocation("shininess"), interfaceData.cupMaterial.m.shininess);

			glUniform3fv(shader->getUniformLocation("tangent"), 1, glm::value_ptr(tangents.at(0)));

			glUniform1i(shader->getUniformLocation("normalMap"), 0);
			interfaceData.wallNormal->bind(GL_TEXTURE0);
		} else {
			glUniform1f(shader->getUniformLocation("rho"), interfaceData.cupMaterial.rho);
			glUniform1f(shader->getUniformLocation("sigma"), interfaceData.cupMaterial.sigma);
		}

		mesh.draw(*shader);
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

	for (int i = 0; i < cup.size(); i++) {
		GPUMesh& mesh = cup.at(i);
		if (i > 3) shader = &simpleShader;
		else shader = &advancedShader;

		shader->bind();
		glUniformMatrix4fv(shader->getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
		glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix3fv(shader->getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

		glUniform3fv(shader->getUniformLocation("kd"), 1, glm::value_ptr(interfaceData.cupMaterial.m.kd));
		glUniform1f(shader->getUniformLocation("ambientCoeff"), 0);

		glUniform3fv(shader->getUniformLocation("cameraPosition"), 1, glm::value_ptr(interfaceData.trackball->position()));

		glUniform3fv(shader->getUniformLocation("lightPosition"), 1, glm::value_ptr(glm::vec3(0, 4, 2)));
		glUniform3fv(shader->getUniformLocation("lightColor"), 1, glm::value_ptr(glm::vec3(1)));

		if (i > 3) {
			glUniform3fv(shader->getUniformLocation("ks"), 1, glm::value_ptr(interfaceData.cupMaterial.m.ks));
			glUniform1f(shader->getUniformLocation("shininess"), interfaceData.cupMaterial.m.shininess);

			glUniform3fv(shader->getUniformLocation("tangent"), 1, glm::value_ptr(tangents.at(0)));

			glUniform1i(shader->getUniformLocation("normalMap"), 0);
			interfaceData.wallNormal->bind(GL_TEXTURE0);
		}
		else {
			glUniform1f(shader->getUniformLocation("rho"), interfaceData.cupMaterial.rho);
			glUniform1f(shader->getUniformLocation("sigma"), interfaceData.cupMaterial.sigma);
		}

		mesh.draw(*shader);
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
