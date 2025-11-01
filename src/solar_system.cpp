#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include "mesh.h"
#include <framework/shader.h>
#include <iostream>

std::vector<Planet> populatePlanets() {
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
		"Black Hole",
		1,			// radius
		0,			// distParent
		1,			// spinSpeed
		0,			// orbitSpeed
		material,	// material
		-1			// parent
	};

	Planet sun = {
		"Sun",
		0.5f,		// radius
		10,			// distParent
		1,			// spinSpeed
		10,			// orbitSpeed
		material,	// material
		0			// parent
	};

	Planet mercury = {
		"Mercury",
		0.25f,			// radius
		2,				// distParent
		1,				// spinSpeed
		3,				// orbitSpeed
		material,		// material
		1				// parent

	};

	Planet venus = {
		"Venus",
		0.2f,		// radius
		4,			// distParent
		2,			// spinSpeed
		5,			// orbitSpeed
		material,	// material
		1			// parent
	};

	Planet earth = {
		"Earth",
		0.3f,		// radius
		6,			// distParent
		100,		// spinSpeed
		4,			// orbitSpeed
		material,	// material
		1			// parent
	}; 

	Planet moon1 = {
		"Moon 1",
		0.1f,		// radius
		1,			// distParent
		2,			// spinSpeed
		10,			// orbitSpeed
		material,	// material
		4			// parent
	};
	Planet moon2 = {
		"Moon 2",
		0.1f,		// radius
		1.5f,		// distParent
		2,			// spinSpeed
		11,			// orbitSpeed
		material,	// material
		4			// parent
	};

	Planet mars = {
		"Mars",
		0.15f,		// radius
		8,			// distParent
		3,			// spinSpeed
		8,			// orbitSpeed
		material,	// material
		1			// parent
	};

	Planet nestRootPlanet = {
		"Nest root planet",
		0.5f,
		10,
		0.1f,
		1,
		material,
		1
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
	planets.push_back(sun);
	planets.push_back(mercury);
	planets.push_back(venus);
	planets.push_back(earth);
	planets.push_back(moon1);
	planets.push_back(moon2);
	planets.push_back(mars);
	//planets.push_back(nestRootPlanet);

	return planets;
}

void renderPlanet(InterfaceData interfaceData, IndexedShader indexedShader, GPUMesh* ball, Planet planet, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	float time = interfaceData.time;
	
	// Compute modelmatrix
	glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(planet.radius)) * glm::rotate(glm::mat4(1), (time * planet.spinSpeed), glm::vec3(0, 1, 0));
	
	Planet* current = &planet;
	int parentIndex = planet.parentPlanet;
	

	while (parentIndex != -1) {
		modelMatrix = glm::rotate(glm::mat4(1), (time * current->orbitSpeed), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), glm::vec3(current->distParent,0,0)) * modelMatrix;
		current = &interfaceData.planets.at(current->parentPlanet);
		parentIndex = interfaceData.planets.at(parentIndex).parentPlanet;
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
			glUniform3fv(shader->getUniformLocation("kd"), 1, glm::value_ptr(planet.material.kd));
			break;
		}
		case 1: {
			glUniform3fv(shader->getUniformLocation("ks"), 1, glm::value_ptr(planet.material.ks));
			glUniform1f(shader->getUniformLocation("shininess"), planet.material.shininess);

			glUniform3fv(shader->getUniformLocation("cameraPosition"), 1, glm::value_ptr(interfaceData.trackball->position()));
			break;
		}
	}

	ball->draw(*shader);
}

void renderSolarSystemScene(InterfaceData interfaceData, std::vector<IndexedShader> indexedShaders, GPUMesh* ball, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	for (int i = 0; i < indexedShaders.size(); i++) {
		for (Planet p : interfaceData.planets) {
			IndexedShader& s = indexedShaders.at(i);

			switch (i) {
				case 0: {
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					// Disable accumulating rendering
					glEnable(GL_BLEND);
					glBlendFunc(GL_ONE, GL_ZERO);
					break;
				}
				case 1: {
					// Enable accumulating rendering
					glEnable(GL_BLEND);
					glBlendFunc(GL_ONE, GL_ONE);
					break;
				}
			}
			
			renderPlanet(interfaceData, s, ball, p, projectionMatrix, viewMatrix);
		}
	}
}
