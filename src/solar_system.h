#include "structs.h"
#include <framework/shader.h>

std::vector<Planet> populatePlanets();
void renderSolarSystemScene(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderComet(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix); 
void renderCometTrajectory(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
void renderCometTrail(Data& data, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);