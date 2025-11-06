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


GLuint shadowBuffer1, shadowBuffer2, shadowTex1, shadowTex2;

void generateShadowStuff() {
	glGenFramebuffers(1, &shadowBuffer1);

	glGenTextures(1, &shadowTex1);
	glBindTexture(GL_TEXTURE_2D, shadowTex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 10240, 10240, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex1, 0);

	glClearDepth(1.f);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);


	glGenFramebuffers(1, &shadowBuffer2);

	glGenTextures(1, &shadowTex2);
	glBindTexture(GL_TEXTURE_2D, shadowTex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 10240, 10240, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex2, 0);

	glClearDepth(1.f);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void deleteShadowStuff() {
	glDeleteTextures(1, &shadowTex1);
	glDeleteFramebuffers(1, &shadowBuffer1);
	glDeleteTextures(1, &shadowTex2);
	glDeleteFramebuffers(1, &shadowBuffer2);
}

void updateShadowTextures(Data& data, glm::mat4 modelMatrix, glm::mat4 lightMVP1, glm::mat4 lightMVP2) {
	const glm::mat4 normalModelMatrix = glm::inverseTranspose(modelMatrix);
	std::vector<GPUMesh>& cup = *data.meshes.cup;

	const Shader& shader = *data.shaders.shadowShader;
	glViewport(0, 0, 10240, 10240);

	// SHADOW MAP 1
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer1);

	glClearDepth(1.f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glDepthMask(GL_TRUE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	shader.bind();

	glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(lightMVP1));
	glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	for (int i = 0; i < cup.size(); i++) {
		GPUMesh& mesh = cup.at(i);
		mesh.draw(shader);
	}

	// SHADOW MAP 2
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer2);

	glClearDepth(1.f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glDepthMask(GL_TRUE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	shader.bind();

	glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(lightMVP2));
	glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
	
	for (int i = 0; i < cup.size(); i++) {
		GPUMesh& mesh = cup.at(i);
		mesh.draw(shader);
	}

	// Reset to normal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, 1800, 900);
}

void shadeOnPlanetScene(Data& data, glm::vec3 light1Position, glm::vec3 light2Position, glm::mat4 lightMVP1, glm::mat4 lightMVP2, glm::mat4 modelMatrix, glm::mat4 mvpMatrix) {
	std::vector<GPUMesh>& cup = *data.meshes.cup;
	Shader* shader;

	std::vector<glm::vec3> tangents = computeTangents(cup.at(5));

	glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

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
			glUniform3fv(shader->getUniformLocation("lightColor"), 1, glm::value_ptr(data.light1Color * glm::pi<float>()));
		}
		else {
			if (data.dayNightCycle) {
				glUniform3fv(shader->getUniformLocation("light1Position"), 1, glm::value_ptr(light1Position));
				glUniform3fv(shader->getUniformLocation("light1Color"), 1, glm::value_ptr(data.dayColor));

				glUniform3fv(shader->getUniformLocation("light2Position"), 1, glm::value_ptr(light2Position));
				glUniform3fv(shader->getUniformLocation("light2Color"), 1, glm::value_ptr(data.nightColor));
			}
			else {
				glUniform3fv(shader->getUniformLocation("light1Position"), 1, glm::value_ptr(light1Position));
				glUniform3fv(shader->getUniformLocation("light1Color"), 1, glm::value_ptr(data.light1Color));

				glUniform3fv(shader->getUniformLocation("light2Position"), 1, glm::value_ptr(light2Position));
				glUniform3fv(shader->getUniformLocation("light2Color"), 1, glm::value_ptr(data.light2Color));
			}


			glUniform3fv(shader->getUniformLocation("ks"), 1, glm::value_ptr(data.cupMaterial.m.ks));
			glUniform1f(shader->getUniformLocation("shininess"), data.cupMaterial.m.shininess);

			glUniform3fv(shader->getUniformLocation("tangent"), 1, glm::value_ptr(tangents.at(0)));

			glUniform1i(shader->getUniformLocation("useShadows"), data.useShadows);
			
			// Normal map
			data.textures.wallNormal->bind(GL_TEXTURE0);
			glUniform1i(shader->getUniformLocation("useNormalMap"), data.useNormalMap);
			glUniform1i(shader->getUniformLocation("normalMap"), 0);

			// Shadow maps
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, shadowTex1);
			glUniform1i(shader->getUniformLocation("shadowMap1"), 1);
			glUniformMatrix4fv(shader->getUniformLocation("lightMVP1"), 1, GL_FALSE, glm::value_ptr(lightMVP1));

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, shadowTex2);
			glUniform1i(shader->getUniformLocation("shadowMap2"), 2);
			glUniformMatrix4fv(shader->getUniformLocation("lightMVP2"), 1, GL_FALSE, glm::value_ptr(lightMVP2));
		}

		mesh.draw(*shader);
	}
}

void renderOnPlanetScene(Data& data, GLuint& minimapFramebuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	std::vector<GPUMesh>* cup = data.meshes.cup;
	
	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

	glm::vec3 light1Position;
	glm::vec3 light2Position;

	if (data.dayNightCycle) {
		glm::mat4 lightMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 3, 0)) * glm::rotate(glm::mat4(1), (data.time * 3), glm::vec3(1, 0, 0));
		light1Position = glm::vec3(lightMatrix * glm::vec4(0, 8, 0, 1));
		light2Position = glm::vec3(lightMatrix * glm::vec4(0, -8, 0, 1));
	}
	else {
		light1Position = glm::vec3(-1.3, 3, 4);
		light2Position = glm::vec3(6, 5, 6);
	}

	glm::mat4 lightMVP1 =
		glm::perspective(glm::radians(120.f), 1.f, 0.1f, 45.f) *
		glm::lookAt(light1Position, glm::vec3(0), glm::vec3(1, 0, 0)) *
		modelMatrix
	;

	glm::mat4 lightMVP2 =
		glm::perspective(glm::radians(120.f), 1.f, 0.1f, 45.f) *
		glm::lookAt(light2Position, glm::vec3(0), glm::vec3(1, 0, 0)) *
		modelMatrix
	;

	updateShadowTextures(data, modelMatrix, lightMVP1, lightMVP2);


	// Render scene
	shadeOnPlanetScene(data, light1Position, light2Position, lightMVP1, lightMVP2, modelMatrix, mvpMatrix);
	

	// Render minimap texture
	Trackball* t = data.trackball;
	viewMatrix = glm::lookAt(glm::vec3(t->lookAt().x,10,t->lookAt().z), t->lookAt(), glm::vec3(0, 0, 1));
	mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glBindFramebuffer(GL_FRAMEBUFFER, minimapFramebuffer);

	glViewport(0, 0, 1024, 1024);

	glClearDepth(1.f);
	glClearColor(1.f, 0.f, 1.f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	shadeOnPlanetScene(data, light1Position, light2Position, lightMVP1, lightMVP2, modelMatrix, mvpMatrix);

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
