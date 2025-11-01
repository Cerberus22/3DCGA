#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <framework/shader.h>

void renderOnPlanetScene(Shader& shader, std::vector<GPUMesh>& cup, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);