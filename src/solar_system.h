#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/shader.h>

void renderPlanet(float time, Shader* shader, GPUMesh* ball, Planet planet, glm::mat4 mvpMatrix, glm::mat4 modelMatrix, glm::mat3 normalModelMatrix);
void renderSolarSystemScene(float time, Shader* shader, GPUMesh* ball, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);