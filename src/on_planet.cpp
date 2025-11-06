#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/shader.h>
#include <iostream>

std::vector<glm::vec3> computeTangents(const GPUMesh& mesh) {
	std::vector<Vertex> vertices = mesh.vertices;
	std::vector<glm::uvec3> triangles = mesh.triangles;
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

void shadeOnPlanetScene(Data& data, glm::mat4 modelMatrix, glm::mat4 mvpMatrix) {
	std::vector<GPUMesh>& cup = *data.meshes.cup;
	Shader* shader;

	std::vector<glm::vec3> tangents = computeTangents(cup.at(4));

	glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	glm::mat4 lightMatrix = glm::rotate(glm::mat4(1), (data.time * 3), glm::vec3(1, 0, 0));
	glm::vec3 light1Position = glm::vec3(lightMatrix * glm::vec4(0, 4, 2, 1));
	glm::vec3 light2Position = glm::vec3(lightMatrix * glm::vec4(0, -4, -2, 1));

	for (int i = 0; i < cup.size(); i++) {
		GPUMesh& mesh = cup.at(i);
		if (data.useAdvancedShading) shader = data.shaders.advancedShader;
		else shader = data.shaders.normalShader;

		shader->bind();

		glUniformMatrix4fv(shader->getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
		glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix3fv(shader->getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

		glUniform1f(shader->getUniformLocation("ambientCoeff"), 0);
		glUniform3fv(shader->getUniformLocation("kd"), 1, glm::value_ptr(data.cupMaterial.m.kd));

		glUniform3fv(shader->getUniformLocation("cameraPosition"), 1, glm::value_ptr(data.trackball->position()));

		if (data.useAdvancedShading) {
			glUniform1f(shader->getUniformLocation("rho"), data.cupMaterial.rho);
			glUniform1f(shader->getUniformLocation("sigma"), data.cupMaterial.sigma);
			
			glUniform3fv(shader->getUniformLocation("lightPosition"), 1, glm::value_ptr(glm::vec3(0, 4, 2)));
			glUniform3fv(shader->getUniformLocation("lightColor"), 1, glm::value_ptr(data.advancedLightColor * glm::pi<float>()));
		}
		else {
			if (data.dayNightCycle) {
				glUniform3fv(shader->getUniformLocation("light1Position"), 1, glm::value_ptr(light1Position));
				glUniform3fv(shader->getUniformLocation("light1Color"), 1, glm::value_ptr(data.dayColor));

				glUniform3fv(shader->getUniformLocation("light2Position"), 1, glm::value_ptr(light2Position));
				glUniform3fv(shader->getUniformLocation("light2Color"), 1, glm::value_ptr(data.nightColor));
			}
			else {
				glUniform3fv(shader->getUniformLocation("light1Position"), 1, glm::value_ptr(glm::vec3(0, 4, 2)));
				glUniform3fv(shader->getUniformLocation("light1Color"), 1, glm::value_ptr(data.advancedLightColor));

				glUniform3fv(shader->getUniformLocation("light2Position"), 1, glm::value_ptr(glm::vec3(0, 4, 2)));
				glUniform3fv(shader->getUniformLocation("light2Color"), 1, glm::value_ptr(glm::vec3(0)));
			}


			glUniform3fv(shader->getUniformLocation("ks"), 1, glm::value_ptr(data.cupMaterial.m.ks));
			glUniform1f(shader->getUniformLocation("shininess"), data.cupMaterial.m.shininess);

			glUniform3fv(shader->getUniformLocation("tangent"), 1, glm::value_ptr(tangents.at(0)));
			
			data.textures.wallNormal->bind(GL_TEXTURE0);

			glUniform1i(shader->getUniformLocation("useNormalMap"), data.useNormalMap);
			glUniform1i(shader->getUniformLocation("normalMap"), 0);
		}

		mesh.draw(*shader);
	}
}

void renderOnPlanetScene(Data& data, GLuint& minimapFramebuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	std::vector<GPUMesh>* cup = data.meshes.cup;
	

	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

	Shader* shader;

	// Render scene
	shadeOnPlanetScene(data, modelMatrix, mvpMatrix);
	

	// Render minimap texture
	Trackball* t = data.trackball;
	viewMatrix = glm::lookAt(glm::vec3(t->lookAt().x,10,t->lookAt().z), t->lookAt(), glm::vec3(0, 0, 1));
	mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glBindFramebuffer(GL_FRAMEBUFFER, minimapFramebuffer);

	glViewport(0, 0, 1024, 1024);

	glClearDepth(1.f);
	glClearColor(1.f, 0.f, 1.f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	shadeOnPlanetScene(data, modelMatrix, mvpMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1800, 900);


	// Render minimap
	const Shader& minimapShader = *data.shaders.minimapShader;
	const GLuint minimapTexture = data.textures.minimapTexture;
	GPUMesh& quad = data.meshes.quad->at(0);

	modelMatrix = glm::translate(glm::mat4(1), glm::vec3(0.8, -0.7, 0)) 
				* glm::rotate(glm::mat4(1), glm::pi<float>()/2, glm::vec3(1,0,0))
				* glm::scale(glm::mat4(1), glm::vec3(0.125, 0.25, 0.25));
	glm::mat4 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	// Skip viewMatrix to make quad fixed on screen
	mvpMatrix = modelMatrix;

	minimapShader.bind();
	glUniformMatrix4fv(minimapShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(minimapShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(minimapShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, minimapTexture);

	glUniform1i(minimapShader.getUniformLocation("tex"), 0);
	
	if (data.drawMinimap) {
		quad.draw(minimapShader);
	}
}
