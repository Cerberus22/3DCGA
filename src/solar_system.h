#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "structs.h"
#include <framework/shader.h>

std::vector<Planet> populatePlanets();
void renderPlanet(InterfaceData interfaceData, IndexedShader indexedShader, GPUMesh* ball, Planet planet, glm::mat4 mvpMatrix, glm::mat4 modelMatrix, glm::mat3 normalModelMatrix);
void renderSolarSystemScene(InterfaceData interfaceData, std::vector<IndexedShader> indexedShaders, GPUMesh* ball, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderComet(InterfaceData interfaceData, float deltaTime, GPUMesh* ballMesh, Shader& cometShader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderCometTrajectory(const InterfaceData& interfaceData, Shader& trajectoryShader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderCometTrail(Shader& trailShader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);