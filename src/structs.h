#include <glm/glm.hpp>
#include "mesh.h"
#include "texture.h"
#include <framework/trackball.h>

struct Planet {
	std::string name;		// Name of the planet
	float radius;			// Size of the planet
	float distParent;		// Distance to parent this planet orbits (center to center)
	float spinSpeed;		// Angular speed with which this planet spins
	float orbitSpeed;		// Angular speed with which this planet orbits parent
	Material material;		// Material of this planet
	int parentPlanet;		// Parent planet to orbit around
	float ambientCoeff = 0;
	bool hasNormalMap = false;
	bool hasSunTexture = false;
};

struct CupMaterial {
	Material m;
	glm::vec3 floorKd;
	float rho;
	float sigma;
};

struct OceanData {
	float amplitude;
	float fx;	// x frequency
	float fz;	// z frequency
	float ft;	// t frequency
	bool doSubdivide = false;
};

struct Shaders {
	Shader* simpleShader;
	Shader* normalShader;
	Shader* advancedShader;
	Shader* cometShader;
	Shader* cometTrailShader;
	Shader* nightSkyShader;
	Shader* minimapShader;
	Shader* oceanShader;
};

struct Meshes {
	std::vector<GPUMesh>* ball;
	std::vector<GPUMesh>* quad;
	std::vector<GPUMesh>* cup;
	std::vector<GPUMesh>* ocean;
};

struct Textures {
	Texture* noise;
	Texture* nightSky;
	Texture* wallNormal;
	GLuint minimapTexture;
};

struct Framebuffers {
	GLuint minimapFramebuffer;
};

/*
	This struct can be used for passing interface stuff to the actual shaders.
*/
struct Data {
	Trackball* trackball;
		
	Shaders shaders;
	Meshes meshes;
	Textures textures;
	Framebuffers framebuffers;

	// Solar System Stuff
	std::vector<Planet> planets;
	float time;
	float t_step;
	int selectedPlanetIndex;
	bool useEnvironmentMap;
	float cometTrailLength;

	// On-planet Stuff
	CupMaterial cupMaterial;
	bool useNormalMap;
	bool useAdvancedShading;
	glm::vec3 advancedLightColor;
	bool drawMinimap;

	bool dayNightCycle;
	glm::vec3 dayColor;
	glm::vec3 nightColor;

	// Ocean stuff
	OceanData oceanData;
};

struct BezierSegment {
    glm::vec3 p0, p1, p2, p3;
};

