#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <framework/shader.h>

void renderOnPlanetScene(InterfaceData interfaceData, Shader& normalShader, Shader& advancedShader, Shader& minimapShader, GLuint& minimapTexture, GLuint& minimapFramebuffer, std::vector<GPUMesh>& cup, GPUMesh& quad, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);