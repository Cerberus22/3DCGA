#include <glm/glm.hpp>
#include "mesh.h"
#include <framework/trackball.h>

/*
	The idea of this struct is to allow numbering shaders.
	This allows determining which uniforms to pass to it during rendering.
*/
struct IndexedShader {
	int index;
	Shader* shader;
};

/*
	This struct can be used for passing interface stuff to the actual shaders.
*/
struct InterfaceData {
	Material material;
	float time;
	Trackball* trackball;
};

struct Planet {
	float radius;			// Size of the planet
	float distParent;		// Distance to parent this planet orbits (center to center)
	float spinSpeed;		// Angular speed with which this planet spins
	float orbitSpeed;		// Angular speed with which this planet orbits parent
	Material material;		// Material of this planet
	Planet* parentPlanet;	// Parent planet to orbit around
};
