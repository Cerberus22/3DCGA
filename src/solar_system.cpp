#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include "mesh.h"
#include <framework/shader.h>
#include <iostream>

// -------------------------------------------- PLANETS --------------------------------------------

// Populates planet scene with planets
std::vector<Planet> populatePlanets() {
	std::vector<Planet> planets = {};
	planets.reserve(20);

	Planet rootPlanet = {
		"Black Hole",
		1,			// radius
		0,			// distParent
		1,			// spinSpeed
		0,			// orbitSpeed
		{
			glm::vec3(0),	// kd
			glm::vec3(0), 	// ks
			5,				// shininess
			1, 				// alpha
			NULL
		},
		-1,			// parent
		0			// ambient coeff
	};

	Planet sun = {
		"Sun",
		0.5f,		// radius
		10,			// distParent
		1,			// spinSpeed
		10,			// orbitSpeed
		{
			glm::vec3(1),	// kd
			glm::vec3(1), 	// ks
			5,				// shininess
			1, 				// alpha
			NULL
		},
		0,			// parent
		1,			// ambient coeff
		false,		// has normal map
		true		// has sun texture
	};

	Planet mercury = {
		"Mercury",
		0.15f,			// radius
		2,				// distParent
		1,				// spinSpeed
		3,				// orbitSpeed
		{
			glm::vec3(0.3f, 0.6f, 0.2f),	// kd
			glm::vec3(0.3f, 0.8f, 0.2f), 	// ks
			5,								// shininess
			1, 								// alpha
			NULL
		},
		1,				// parent
		0,
		true
	};

	Planet venus = {
		"Venus",
		0.2f,		// radius
		4,			// distParent
		2,			// spinSpeed
		5,			// orbitSpeed
		{
			glm::vec3(1.0f, 0.6f, 0.0f),	// kd
			glm::vec3(1.0f, 0.8f, 0.0f), 	// ks
			5,								// shininess
			1, 								// alpha
			NULL
		},
		1,			// parent
		0,
		true
	};

	Planet earth = {
		"Earth",
		0.3f,		// radius
		6,			// distParent
		100,		// spinSpeed
		4,			// orbitSpeed
		{
			glm::vec3(0.0f, 0.2f, 1.0f),	// kd
			glm::vec3(0.0f, 0.6f, 0.4f), 	// ks
			5,								// shininess
			1, 								// alpha
			NULL
		},
		1			// parent
	}; 

	Planet moon1 = {
		"Moon 1",
		0.1f,		// radius
		1,			// distParent
		2,			// spinSpeed
		10,			// orbitSpeed
		{
			glm::vec3(0.2f, 0.2f, 0.2f),	// kd
			glm::vec3(0.2f, 0.2f, 0.2f), 	// ks
			5,								// shininess
			1, 								// alpha
			NULL
		},
		4			// parent
	};
	Planet moon2 = {
		"Moon 2",
		0.1f,		// radius
		1.5f,		// distParent
		2,			// spinSpeed
		11,			// orbitSpeed
		{
			glm::vec3(0.4f, 0.2f, 0.4f),	// kd
			glm::vec3(0.4f, 0.2f, 0.4f), 	// ks
			5,								// shininess
			1, 								// alpha
			NULL
		},
		4			// parent
	};

	Planet mars = {
		"Mars",
		0.15f,		// radius
		8,			// distParent
		3,			// spinSpeed
		8,			// orbitSpeed
		{
			glm::vec3(1.0f, 0.2f, 0.0f),	// kd
			glm::vec3(1.0f, 0.4f, 0.0f), 	// ks
			5,								// shininess
			1, 								// alpha
			NULL
		},
		1			// parent
	};

	Planet nestRootPlanet = {
		"Nest root planet",
		0.5f,
		10,
		0.1f,
		1,
		{
			glm::vec3(1),	// kd
			glm::vec3(1), 	// ks
			5,				// shininess
			1, 				// alpha
			NULL
		},
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

// Renders a single planet
void renderPlanet(Data& data, Planet planet, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	float time = data.time;

	const Shader& shader = *data.shaders.simpleShader;
	GPUMesh* ball = &data.meshes.ball->at(0);

	// Compute modelmatrix
	glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(planet.radius)) * glm::rotate(glm::mat4(1), (time * planet.spinSpeed), glm::vec3(0, 1, 0));

	Planet* current = &planet;
	int parentIndex = planet.parentPlanet;

	while (parentIndex != -1) {
		modelMatrix = glm::rotate(glm::mat4(1), (time * current->orbitSpeed), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), glm::vec3(current->distParent, 0, 0)) * modelMatrix;
		current = &data.planets.at(current->parentPlanet);
		parentIndex = data.planets.at(parentIndex).parentPlanet;
	}
	
	Planet* sun = &data.planets.at(1);
	glm::mat4 sunMatrix = glm::rotate(glm::mat4(1), (time * sun->orbitSpeed), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), glm::vec3(sun->distParent, 0, 0));

	const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
	const glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;


	// Pass uniforms
	shader.bind();

	glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

	glUniform1f(shader.getUniformLocation("ambientCoeff"), planet.ambientCoeff);
	glUniform3fv(shader.getUniformLocation("kd"), 1, glm::value_ptr(planet.material.kd));
	glUniform3fv(shader.getUniformLocation("ks"), 1, glm::value_ptr(planet.material.ks));
	glUniform1f(shader.getUniformLocation("shininess"), planet.material.shininess);
	glUniform3fv(shader.getUniformLocation("cameraPosition"), 1, glm::value_ptr(data.trackball->position()));
	
	glUniform3fv(shader.getUniformLocation("lightPosition"), 1, glm::value_ptr(sunMatrix * glm::vec4(0,0,0,1)));
	glUniform3fv(shader.getUniformLocation("lightColor"), 1, glm::value_ptr(sun->material.kd));

	glUniform1i(shader.getUniformLocation("hasSunTexture"), planet.hasSunTexture);
	glUniform1i(shader.getUniformLocation("normalMap"), 0);
	data.textures.noise->bind(GL_TEXTURE0);

	ball->draw(shader);
}

// Renders the planets
void renderSolarSystemScene(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	const Shader& nightSkyShader = *data.shaders.nightSkyShader;
	GPUMesh* ball = &data.meshes.ball->at(0);
	
	const glm::mat4 modelMatrix = glm::translate(glm::mat4(1), data.trackball->position());
	const glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

	if (data.useEnvironmentMap) {
		// Render starry background
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		nightSkyShader.bind();
		glUniformMatrix4fv(nightSkyShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
		glUniformMatrix4fv(nightSkyShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix3fv(nightSkyShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

		data.textures.nightSky->bind(GL_TEXTURE0);

		glUniform1i(nightSkyShader.getUniformLocation("tex"), 0);

		ball->draw(nightSkyShader);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	// Render planets
	for (Planet p : data.planets) {
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		// Disable accumulating rendering
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
			
		renderPlanet(data, p, projectionMatrix, viewMatrix);
	}
}

// -------------------------------------------- COMET --------------------------------------------

glm::vec3 evaluateCubicBezier(const BezierSegment& seg, float t) {
    float u = 1.0f - t;
    return u*u*u*seg.p0 + 3*u*u*t*seg.p1 + 3*u*t*t*seg.p2 + t*t*t*seg.p3;
}

float cometPathProgress = 0;

std::vector<glm::vec3> cometTrail;
static float accumulatedDistance = 0.0f;
static glm::vec3 lastCometPos = glm::vec3(0);

glm::vec3 offset = glm::vec3(2.5f, 2.5f, 0);

std::vector<BezierSegment> cometPath = {
	{glm::vec3(-5, 0, -5) + offset, glm::vec3(-2, 3, -3) + offset, glm::vec3(2, 3, 3) + offset, glm::vec3(5, 0, 5) + offset},
	{glm::vec3(5, 0, 5) + offset, glm::vec3(7, -2, 8) + offset, glm::vec3(-7, 2, 8) + offset, glm::vec3(-5, 0, 5) + offset},
	{glm::vec3(-5, 0, 5) + offset, glm::vec3(-8, 3, 2) + offset, glm::vec3(8, -3, -2) + offset, glm::vec3(5, 0, -5) + offset},
	{glm::vec3(5, 0, -5) + offset, glm::vec3(7, 2, -8) + offset, glm::vec3(-7, -2, -8) + offset, glm::vec3(-5, 0, -5) + offset}
};

// Renders the comet
void renderComet(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	const Shader& cometShader = *data.shaders.cometShader;
	GPUMesh* ball = &data.meshes.ball->at(0);

	int numSegments = (int)cometPath.size();

	cometPathProgress += 0.02 * data.t_step;
    if (cometPathProgress > 1) cometPathProgress -= 1;
    
    int currentSegmentIndex = std::floor(cometPathProgress * numSegments);
	if (currentSegmentIndex >= numSegments) currentSegmentIndex = numSegments - 1; // edge case when we are exactly at 1
    
	float posAlongSegment = (cometPathProgress * numSegments) - currentSegmentIndex;
    glm::vec3 cometPos = evaluateCubicBezier(cometPath[currentSegmentIndex], posAlongSegment);

	// Adding data to comet trail
	if (cometTrail.empty()) {
		cometTrail.push_back(cometPos);
		lastCometPos = cometPos;
	}

	float distance = glm::length(cometPos - lastCometPos);
	accumulatedDistance += distance;

	// only add new point if comet moved enough distance
	if (accumulatedDistance >= 0.1f) {
		cometTrail.push_back(cometPos);
		lastCometPos = cometPos;
		accumulatedDistance = 0.0f;
	}

	// compute total trail length and remove oldest points if needed
	float totalLength = 0.0f;
	for (int i = cometTrail.size() - 1; i > 0; i--) {
		totalLength += glm::length(cometTrail[i] - cometTrail[i - 1]);
		if (totalLength > data.cometTrailLength) {
			cometTrail.erase(cometTrail.begin(), cometTrail.begin() + i - 1);
			break;
		}
	}

	// Comet itself
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), cometPos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
    glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    cometShader.bind();
    glUniformMatrix4fv(cometShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(cometShader.getUniformLocation("emissiveColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.8f, 0.6f)));

    ball->draw(cometShader);
}

GLuint trajVao, trajVbo;
std::vector<glm::vec3> trajectoryPoints;

GLuint vao, vboPos, vboAlpha;
std::vector<float> alphas;

void createBuffers() {
	glGenVertexArrays(1, &trajVao);
	glGenBuffers(1, &trajVbo);

	glBindVertexArray(trajVao);
	glBindBuffer(GL_ARRAY_BUFFER, trajVbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);


	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboPos);
	glGenBuffers(1, &vboAlpha);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, vboAlpha);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
}

void destroyBuffers() {
	glDeleteBuffers(1, &vboPos);
	glDeleteBuffers(1, &vboAlpha);
	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &trajVbo);
	glDeleteVertexArrays(1, &trajVao);
}


// Renders the trajectory (Bezier curve) of the comet
void renderCometTrajectory(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	const Shader& shader = *data.shaders.cometShader;
	
	// Sample points along the entire path
	trajectoryPoints.clear();
    const int samplesPerSegment = 40;
    for (const auto& segment : cometPath) {
        for (int i = 0; i <= samplesPerSegment; ++i) {
            float t = (float)i / samplesPerSegment;
            trajectoryPoints.push_back(evaluateCubicBezier(segment, t));
        }
    }

    glm::mat4 mvp = projectionMatrix * viewMatrix * glm::mat4(1.0f);

    shader.bind();
    glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform3fv(shader.getUniformLocation("emissiveColor"), 1, glm::value_ptr(glm::vec3(0.0f, 0.2f, 0.1f)));

	glBindVertexArray(trajVao);
	glBindBuffer(GL_ARRAY_BUFFER, trajVbo);
	glBufferData(GL_ARRAY_BUFFER, trajectoryPoints.size() * sizeof(glm::vec3), trajectoryPoints.data(), GL_STATIC_DRAW);

    glDrawArrays(GL_LINE_STRIP, 0, trajectoryPoints.size());
}

// Renders the fading comet trail
void renderCometTrail(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	const Shader& trailShader = *data.shaders.cometTrailShader;
	
	if (cometTrail.size() < 2) return;

    // Compute fading alphas
	alphas.clear();
    alphas.reserve(cometTrail.size());
    for (size_t i = 0; i < cometTrail.size(); ++i) {
        float t = (float)i / (float)(cometTrail.size() - 1);
        alphas.push_back(t * t * 0.8f);
    }

    glm::mat4 mvp = projectionMatrix * viewMatrix;

    trailShader.bind();
    glUniformMatrix4fv(trailShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvp));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, cometTrail.size() * sizeof(glm::vec3), cometTrail.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboAlpha);
	glBufferData(GL_ARRAY_BUFFER, alphas.size() * sizeof(float), alphas.data(), GL_STATIC_DRAW);

    glDrawArrays(GL_LINE_STRIP, 0, cometTrail.size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}