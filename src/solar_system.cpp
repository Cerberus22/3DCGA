#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include "mesh.h"
#include <framework/shader.h>
#include <iostream>

void renderPlanet(InterfaceData interfaceData, IndexedShader indexedShader, GPUMesh* ball, Planet planet, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	float time = interfaceData.time;
	
	// Compute modelmatrix
	glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(planet.radius)) * glm::rotate(glm::mat4(1), (time * planet.spinSpeed), glm::vec3(0, 1, 0));
	
	Planet* parent = planet.parentPlanet;
	Planet* current = &planet;

	while (parent != NULL) {
		modelMatrix = glm::rotate(glm::mat4(1), (time * current->orbitSpeed), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), glm::vec3(current->distParent,0,0)) * modelMatrix;
		current = current->parentPlanet;
		parent = parent->parentPlanet;
	}

	const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
	const glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

	
	// Pass uniforms
	Shader* shader = indexedShader.shader;

	shader->bind();

	glUniformMatrix4fv(shader->getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(shader->getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	switch (indexedShader.index) {
		case 0: {
			glUniform3fv(shader->getUniformLocation("kd"), 1, glm::value_ptr(interfaceData.material.kd));
			break;
		}
		case 1: {
			glUniform3fv(shader->getUniformLocation("ks"), 1, glm::value_ptr(interfaceData.material.ks));
			glUniform1f(shader->getUniformLocation("shininess"), interfaceData.material.shininess);

			glUniform3fv(shader->getUniformLocation("cameraPosition"), 1, glm::value_ptr(interfaceData.trackball->position()));
			break;
		}
	}

	ball->draw(*shader);
}

void renderSolarSystemScene(InterfaceData interfaceData, std::vector<IndexedShader> indexedShaders, GPUMesh* ball, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	Material material = {
		glm::vec3(1,0,0), // kd
		glm::vec3(1,0,0), // ks
		0.5f, // shininess
		1.0f, // alpha
		NULL
	};

	std::vector<Planet> planets = {};
	planets.reserve(20);

	Planet rootPlanet = {
		2,			// radius
		0,			// distParent
		1,			// spinSpeed
		0,			// orbitSpeed
		material,	// material
		NULL		// parent
	};

	Planet mercury = {
		0.6,			// radius
		4,				// distParent
		1,				// spinSpeed
		3,				// orbitSpeed
		material,		// material
		& rootPlanet	// parent

	};

	Planet venus = {
		1,			// radius
		8,			// distParent
		2,			// spinSpeed
		5,			// orbitSpeed
		material,	// material
		&rootPlanet	// parent
	};

	Planet earth = {
		1,			// radius
		12,			// distParent
		100,		// spinSpeed
		4,			// orbitSpeed
		material,	// material
		&rootPlanet	// parent
	}; 

	Planet moon1 = {
		0.3,		// radius
		2,			// distParent
		2,			// spinSpeed
		10,			// orbitSpeed
		material,	// material
		&earth		// parent
	};
	Planet moon2 = {
		0.2,		// radius
		3,			// distParent
		2,			// spinSpeed
		11,			// orbitSpeed
		material,	// material
		&earth		// parent
	};

	Planet mars = {
		0.5,		// radius
		16,			// distParent
		3,			// spinSpeed
		8,			// orbitSpeed
		material,	// material
		&rootPlanet	// parent
	};

	Planet nestRootPlanet = {
		1,
		20,
		0.1,
		1,
		material,
		&rootPlanet
	};

	//Planet* nestRoot = &nestRootPlanet;

	//for (int i = 2; i < 10; i++) {
	//	planets.emplace_back(
	//		1.f / i,
	//		10.f / i,
	//		0.1 * i,
	//		1 * i,
	//		material,
	//		nestRoot
	//	);

	//	nestRoot = &planets.back();
	//}

	planets.push_back(rootPlanet);
	planets.push_back(mercury);
	planets.push_back(venus);
	planets.push_back(earth);
	planets.push_back(moon1);
	planets.push_back(moon2);
	planets.push_back(mars);
	//planets.push_back(nestRootPlanet);

	for (int i = 0; i < indexedShaders.size(); i++) {
		for (Planet& p : planets) {
			IndexedShader& s = indexedShaders.at(i);

			if (i == 0) {
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				// Disable accumulating rendering
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ZERO);
			}
			else if (i == 1) {
				// Enable accumulating rendering
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
			}
			renderPlanet(interfaceData, s, ball, p, projectionMatrix, viewMatrix);
		}
	}
}
