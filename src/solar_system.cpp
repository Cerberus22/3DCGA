#include <glm/glm.hpp>
#include "structs.h"

void renderSolarSystemScene() {
	Material material = {
		glm::vec3(1,0,0), // kd
		glm::vec3(1,0,0), // ks
		0.5f, // shininess
		1.0f  // alpha
	};

	Planet rootPlanet = {
		2, // radius
		0, // distParent
		0, // spinSpeed
		0, // orbitSpeed
		material, // material
		NULL	  // parent
	};


}