#include "structs.h"
#include <framework/shader.h>

std::vector<Planet> populatePlanets();
void renderPlanet(InterfaceData interfaceData, Shader& shader, GPUMesh* ball, Planet planet, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderSolarSystemScene(InterfaceData interfaceData, Shader& shader, Shader& nightSkyShader, GPUMesh* ball, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderComet(InterfaceData interfaceData, float deltaTime, GPUMesh* ballMesh, Shader& cometShader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderCometTrajectory(const InterfaceData& interfaceData, Shader& trajectoryShader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderCometTrail(Shader& trailShader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
