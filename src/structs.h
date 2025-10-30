#include <glm/glm.hpp>

struct Material {
	glm::vec3 kd;
	glm::vec3 ks{ 0.0f };
	float shininess{ 1.0f };
	float alpha{ 1.0f };
};

struct Planet {
	float radius;			// Size of the planet
	float distParent;		// Distance to parent this planet orbits (center to center)
	float spinSpeed;		// Angular speed with which this planet spins
	float orbitSpeed;		// Angular speed with which this planet orbits parent
	Material material;		// Material of this planet
	Planet* parentPlanet;	// Parent planet to orbit around
};
