//#include "Image.h"
#include "texture.h"
#include "solar_system.h"
#include "on_planet.h"
// Always include window first (because it includes glfw, which includes GL which needs to be included AFTER glew).
// Can't wait for modules to fix this stuff...
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glad/glad.h>
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <imgui/imgui.h>
DISABLE_WARNINGS_POP()
#include <framework/shader.h>
#include <framework/window.h>
#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <framework/trackball.h>

class Application {
public:
    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL41)
        , m_texture(RESOURCE_ROOT "resources/checkerboard.png")
        , trackball(&m_window, glm::radians(45.f))
    {
        m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                onKeyPressed(key, mods);
            else if (action == GLFW_RELEASE)
                onKeyReleased(key, mods);
        });
        /*
        m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
        m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
            if (action == GLFW_PRESS)
                onMouseClicked(button, mods);
            else if (action == GLFW_RELEASE)
                onMouseReleased(button, mods);
        });
        */

        ball = GPUMesh::loadMeshGPU(RESOURCE_ROOT "resources/ball.obj");
        cup = GPUMesh::loadMeshGPU(RESOURCE_ROOT "resources/champions.obj");

        interfaceData.time = 0.f;
        t_step = 0.05f;
        interfaceData.planets = populatePlanets();
        interfaceData.trackball = &trackball;
        interfaceData.selectedPlanetIndex = 0;
        interfaceData.cometOffset = glm::vec3(2.5f, 2.5f, 0);
        interfaceData.cometSpeed = 0.05f;

        try {
            ShaderBuilder defaultBuilder;
            defaultBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            defaultBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shader_frag.glsl");
            m_defaultShader = defaultBuilder.build();

            ShaderBuilder shadowBuilder;
            shadowBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shadow_vert.glsl");
            shadowBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shadow_frag.glsl");
            m_shadowShader = shadowBuilder.build();

            // Any new shaders can be added below in similar fashion.
            // ==> Don't forget to reconfigure CMake when you do!
            //     Visual Studio: PROJECT => Generate Cache for ComputerGraphics
            //     VS Code: ctrl + shift + p => CMake: Configure => enter
            // ....

            ShaderBuilder lambertianShaderBuilder;
            lambertianShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_general.glsl");
            lambertianShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shading/frag_lambert.glsl");
            lambertianShader = lambertianShaderBuilder.build();
            IndexedLambertianShader = { 0, &lambertianShader };

            ShaderBuilder phongShaderBuilder;
            phongShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_general.glsl");
            phongShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shading/frag_phong.glsl");
            phongShader = phongShaderBuilder.build();
            IndexedPhongShader = { 1, &phongShader };

            ShaderBuilder cometShaderBuilder;
            cometShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_general.glsl");
            cometShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shading/frag_comet.glsl");
            cometShader = cometShaderBuilder.build();

            ShaderBuilder cometTrailShaderBuilder;
            cometTrailShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_comet_trail.glsl");
            cometTrailShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shading/frag_comet_trail.glsl");
            cometTrailShader = cometTrailShaderBuilder.build();

        } catch (ShaderLoadingException e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void renderSolarSystemGUI() {
        ImGui::SliderFloat("Time Speed", &t_step, 0.001f, 1.f, "%.3f");

        // Display planets in scene
        std::vector<std::string> planetNames = {};
        for (Planet planet: interfaceData.planets) {
            planetNames.push_back(planet.name);
        }

        std::vector<const char*> itemCStrings = {};
        for (const auto& string : planetNames) {
            itemCStrings.push_back(string.c_str());
        }

        ImGui::Separator();
        ImGui::Text("Planets");
        int tempSelectedItem = interfaceData.selectedPlanetIndex;
        if (ImGui::ListBox(" ", &tempSelectedItem, itemCStrings.data(), (int) itemCStrings.size(), 10)) {
            interfaceData.selectedPlanetIndex = static_cast<size_t>(tempSelectedItem);
        }

        ImGui::Separator();
        ImGui::Text("Current planet material");
        ImGui::ColorEdit3("Diffuse", glm::value_ptr(interfaceData.planets[interfaceData.selectedPlanetIndex].material.kd));
        ImGui::ColorEdit3("Specular", glm::value_ptr(interfaceData.planets[interfaceData.selectedPlanetIndex].material.ks));
        ImGui::DragFloat("Shininess", &interfaceData.planets[interfaceData.selectedPlanetIndex].material.shininess, 0.1, 0.0, 100.0, "%.2f");

        ImGui::Separator();
        ImGui::Text("Comet (Bezier curve)");
        ImGui::Checkbox("Draw comet trajectory", &drawCometTrajectory);
        ImGui::DragFloat("Comet speed", &interfaceData.cometSpeed, 0.05f, 0.0f, 0.2f, "%.05f");
        ImGui::InputFloat("Comet offset x", &interfaceData.cometOffset[0]);
        ImGui::InputFloat("Comet offset y", &interfaceData.cometOffset[1]);
        ImGui::InputFloat("Comet offset z", &interfaceData.cometOffset[2]);
    }

    void update()
    {
        int sceneNr = 0;
        const char* scenes[] = { "Solar System", "On planet" };

        const char* viewpoints[] = { "First", "Second" };

        while (!m_window.shouldClose()) {
            interfaceData.time += t_step / 100;

            // This is your game loop
            // Put your real-time logic and rendering in here
            m_window.updateInput();

            m_viewMatrix = trackball.viewMatrix();
            m_projectionMatrix = trackball.projectionMatrix();

            // Use ImGui for easy input/output of ints, floats, strings, etc...
            ImGui::Begin("Assignment 2");

            ImGui::Combo("Scene", &sceneNr, scenes, 2);

            if (sceneNr == 0) {
                renderSolarSystemGUI();
            }
            else {
                ImGui::Combo("Viewpoint", &selectedViewpoint, viewpoints, 2);
            }

            ImGui::End();

            // Clear the screen
            glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            std::vector<IndexedShader> shaders = { IndexedLambertianShader, IndexedPhongShader };

            if (sceneNr == 0) {
                m_viewMatrix = trackball.viewMatrix(); 
                renderSolarSystemScene(interfaceData, shaders, &(ball.at(0)), m_projectionMatrix, m_viewMatrix);
                renderComet(interfaceData, t_step, &(ball.at(0)), cometShader, m_projectionMatrix, m_viewMatrix);
                if (drawCometTrajectory) {
                    renderCometTrajectory(interfaceData, cometShader, m_projectionMatrix, m_viewMatrix);
                }
                renderCometTrail(cometTrailShader, m_projectionMatrix, m_viewMatrix);
            }
            else {
                if (selectedViewpoint == 0) {
                    m_viewMatrix = trackball.viewMatrix(); 
                } else {
                    // TODO: trackball still gets updated, even though we do not intend to
                    glm::vec3 cameraPos(5, 3, 5);
                    glm::vec3 target(0, 0, 0);
                    glm::vec3 up(0, 1, 0);
                    m_viewMatrix = glm::lookAt(cameraPos, target, up);
                }
                renderOnPlanetScene(m_defaultShader, cup, m_projectionMatrix, m_viewMatrix);
            }
            // Processes input and swaps the window buffer
            m_window.swapBuffers();
        }
    }

    // In here you can handle key presses
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyPressed(int key, int mods)
    {
        std::cout << "Key pressed: " << key << std::endl;
    }

    // In here you can handle key releases
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyReleased(int key, int mods)
    {
        std::cout << "Key released: " << key << std::endl;
    }

    // If the mouse is moved this function will be called with the x, y screen-coordinates of the mouse
    void onMouseMove(const glm::dvec2& cursorPos)
    {
        std::cout << "Mouse at position: " << cursorPos.x << " " << cursorPos.y << std::endl;
    }

    // If one of the mouse buttons is pressed this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseClicked(int button, int mods)
    {
        std::cout << "Pressed mouse button: " << button << std::endl;
    }

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods)
    {
        std::cout << "Released mouse button: " << button << std::endl;
    }

private:
    Window m_window;

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_shadowShader;

    // Normal Shaders!
    Shader lambertianShader;
    Shader phongShader;
    Shader cometShader;
    Shader cometTrailShader;
    
    // Indexed Shaders!
    IndexedShader IndexedLambertianShader;
    IndexedShader IndexedPhongShader;

    std::vector<GPUMesh>* m_meshes;
    std::vector<GPUMesh> ball;
    std::vector<GPUMesh> cup;

    Texture m_texture;
    bool m_useMaterial { true };

    Trackball trackball;

    // Projection and view matrices for you to fill in and use
    glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };

    InterfaceData interfaceData;

    float t_step;

    int selectedViewpoint = 0;

    bool drawCometTrajectory = false;
};

int main()
{
    Application app;
    app.update();

    return 0;
}