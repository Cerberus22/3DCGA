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

/*
	This struct can be used for passing interface stuff to the actual shaders.
*/
struct InterfaceData {
	Trackball* trackball;
		
	// Solar System Stuff
	std::vector<Planet> planets;
	float time;
	int selectedPlanetIndex;
	float cometSpeed;
	Texture* noise;
	Texture* nightSky;
	float normalOffsetStrength;

	// On-planet Stuff
	CupMaterial cupMaterial;
	Texture* wallNormal;
	bool useNormalMap;
	bool shadeCupSimple;

	bool dayNightCycle;
	glm::vec3 dayColor;
	glm::vec3 nightColor;
};

struct BezierSegment {
    glm::vec3 p0, p1, p2, p3;
};

