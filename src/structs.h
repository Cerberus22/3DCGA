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
	float kx = 0.75;	// x frequency
	float ax = 1;	// x frequency
	float kz = 0.27;	// z frequency
	float az = 1.2;	// z frequency
	float kt = 0.6;	// t frequency

	float k1 = 0.3;	// k1 frequency
	float a1 = 1;	// k1 frequency
	float k1angle = -0.35;

	float k2 = 0.05;	// k2 frequency
	float a2 = 1.8;	// k2 frequency
	float k2angle = -1;
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
	Shader* shadowShader;
	Shader* envShader;
};

struct Meshes {
	std::vector<GPUMesh>* ball;
	std::vector<GPUMesh>* quad;
	std::vector<GPUMesh>* cup;
	std::vector<GPUMesh>* ocean;
	std::vector<GPUMesh>* cube;
};

struct Textures {
	Texture* noise;
	Texture* nightSky;
	Texture* wallNormal;
	Texture* envMap;
	GLuint minimapTexture;

	Texture* frame1;
	Texture* frame2;
	Texture* frame3;
	Texture* frame4;
	Texture* frame5;
	Texture* frame6;
	Texture* frame7;
	Texture* frame8;
	Texture* frame9;
	Texture* frame10;
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
	bool useAnimatedTexture;
	float animationSpeed;
	float cometTrailLength;

	// On-planet Stuff
	CupMaterial cupMaterial;
	bool useNormalMap;
	bool useShadows;
	bool useAdvancedShading;
	bool drawMinimap = false;
	glm::vec3 light1Color;
	glm::vec3 light2Color;

	bool dayNightCycle = false;
	glm::vec3 dayColor;
	glm::vec3 nightColor;

	// Ocean stuff
	OceanData oceanData;
};

struct BezierSegment {
    glm::vec3 p0, p1, p2, p3;
};

