#include <framework/shader.h>

void renderOnPlanetScene(InterfaceData interfaceData, Shader& normalShader, Shader& advancedShader, Shader& minimapShader, GLuint& minimapTexture, GLuint& minimapFramebuffer, std::vector<GPUMesh>& cup, GPUMesh& quad, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);