//#include "Image.h"
#include "texture.h"
#include "solar_system.h"
#include "on_planet.h"
#include "ocean.h"
#include "maps.h"
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
        : m_window("Final Project", glm::ivec2(1800, 900), OpenGLVersion::GL41)
        , m_texture(RESOURCE_ROOT "resources/checkerboard.png")
        , noise(RESOURCE_ROOT "resources/textures/noise.png")
        , nightSky(RESOURCE_ROOT "resources/textures/nightsky.jpg")
        , wallNormal(RESOURCE_ROOT "resources/textures/normalmappy.jpg")
        , trackball(&m_window, glm::radians(45.f))
    {
        m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                onKeyPressed(key, mods);
            else if (action == GLFW_RELEASE)
                onKeyReleased(key, mods);
        });

        showUI = true;

        /*
        m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
        m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
            if (action == GLFW_PRESS)
                onMouseClicked(button, mods);
            else if (action == GLFW_RELEASE)
                onMouseReleased(button, mods);
        });
        */

        ball = GPUMesh::loadMeshGPU(RESOURCE_ROOT "resources/ball_s.obj");
        quad = GPUMesh::loadMeshGPU(RESOURCE_ROOT "resources/quad.obj");
        cup = GPUMesh::loadMeshGPU(RESOURCE_ROOT "resources/champions.obj");
        ocean = GPUMesh::loadMeshGPU(RESOURCE_ROOT "resources/ocean.obj");

        data.meshes.ball = &ball;
        data.meshes.quad = &quad;
        data.meshes.cup = &cup;
        data.meshes.ocean = &ocean;

        data.t_step = 0.05f;

        data.time = 0.f;
        data.planets = populatePlanets();
        data.trackball = &trackball;
        data.selectedPlanetIndex = 0;
        data.cometTrailLength = 5.0f;

        data.cupMaterial.rho = 1.f;
        data.cupMaterial.sigma = 0.f;
        data.cupMaterial.m.kd = glm::vec3(1.0);
        data.cupMaterial.floorKd = glm::vec3(1.f);
        
        data.useEnvironmentMap = true;
        data.textures.noise = &noise;
        data.textures.nightSky = &nightSky;
        data.textures.wallNormal = &wallNormal;

        data.dayNightCycle = true;
        data.dayColor = glm::vec3(1, 0.8, 0.8);
	    data.nightColor = glm::vec3(0.6, 0.6, 1);

        createTexture(minimapTexture, minimapFramebuffer);
        data.textures.minimapTexture = minimapTexture;
        data.framebuffers.minimapFramebuffer = minimapFramebuffer;

        createBuffers();

        try {
            // Any new shaders can be added below in similar fashion.
            // ==> Don't forget to reconfigure CMake when you do!
            //     Visual Studio: PROJECT => Generate Cache for ComputerGraphics
            //     VS Code: ctrl + shift + p => CMake: Configure => enter
            // ....

            ShaderBuilder simpleShaderBuilder;
            simpleShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_general.glsl");
            simpleShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shading/frag_simple_shading.glsl");
            simpleShader = simpleShaderBuilder.build();
            data.shaders.simpleShader = &simpleShader;

            ShaderBuilder normalShaderBuilder;
            normalShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/texture/vert_normal.glsl");
            normalShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/texture/frag_normal.glsl");
            normalShader = normalShaderBuilder.build();
            data.shaders.normalShader = &normalShader;

            ShaderBuilder advancedShaderBuilder;
            advancedShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_general.glsl");
            advancedShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shading/frag_oren_nayar.glsl");
            advancedShader = advancedShaderBuilder.build();
            data.shaders.advancedShader = &advancedShader;

            ShaderBuilder cometShaderBuilder;
            cometShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_general.glsl");
            cometShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/comet/frag_comet.glsl");
            cometShader = cometShaderBuilder.build();
            data.shaders.cometShader = &cometShader;

            ShaderBuilder cometTrailShaderBuilder;
            cometTrailShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/comet/vert_comet_trail.glsl");
            cometTrailShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/comet/frag_comet_trail.glsl");
            cometTrailShader = cometTrailShaderBuilder.build();
            data.shaders.cometTrailShader = &cometTrailShader;

            ShaderBuilder nigthSkyShaderBuilder;
            nigthSkyShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shading/vert_general.glsl");
            nigthSkyShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/texture/frag_minimap.glsl");
            nightSkyShader = nigthSkyShaderBuilder.build();
            data.shaders.nightSkyShader = &nightSkyShader;

            ShaderBuilder minimapShaderBuilder;
            minimapShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/texture/vert_minimap.glsl");
            minimapShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/texture/frag_minimap.glsl");
            minimapShader = minimapShaderBuilder.build();
            data.shaders.minimapShader = &minimapShader;

            ShaderBuilder oceanShaderBuilder;
            oceanShaderBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/ocean/vert_ocean.glsl");
            oceanShaderBuilder.addStage(GL_GEOMETRY_SHADER, RESOURCE_ROOT "shaders/ocean/geom_ocean.glsl");
            oceanShaderBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/ocean/frag_ocean.glsl");
            oceanShader = oceanShaderBuilder.build();
            data.shaders.oceanShader = &oceanShader;

        } catch (ShaderLoadingException e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void renderSolarSystemGUI() {
        // Display planets in scene
        std::vector<std::string> planetNames = {};
        for (Planet planet: data.planets) {
            planetNames.push_back(planet.name);
        }

        std::vector<const char*> itemCStrings = {};
        for (const auto& string : planetNames) {
            itemCStrings.push_back(string.c_str());
        }

        ImGui::Separator();
        ImGui::Text("Planets");
        int tempSelectedItem = data.selectedPlanetIndex;
        if (ImGui::ListBox(" ", &tempSelectedItem, itemCStrings.data(), (int) itemCStrings.size(), 10)) {
            data.selectedPlanetIndex = static_cast<size_t>(tempSelectedItem);
        }

        Planet& p = data.planets[data.selectedPlanetIndex];

        ImGui::Separator();
        ImGui::Text("Current planet material");
        ImGui::ColorEdit3("Diffuse", glm::value_ptr(p.material.kd));
        ImGui::ColorEdit3("Specular", glm::value_ptr(p.material.ks));
        ImGui::DragFloat("Shininess", &p.material.shininess, 0.1f, 0.1f, 10.0f, "%.2f");
        ImGui::SliderFloat("Ambient coeff.", &p.ambientCoeff, 0.f, 1.f, "%.1f");

        ImGui::Separator();
        ImGui::Text("Comet (Bezier curve)");
        ImGui::Checkbox("Draw comet trajectory", &drawCometTrajectory);
        ImGui::SliderFloat("Comet trail length", &data.cometTrailLength, 0.f, 10.f, "%.1f");

        ImGui::Separator();
        ImGui::Checkbox("Environment map", &data.useEnvironmentMap);
    }

    void update()
    {
        int sceneNr = 0;
        const char* scenes[] = { "Solar System", "On planet", "Ocean"};

        const char* viewpoints[] = { "First", "Second" };

        while (!m_window.shouldClose()) {
            data.time += data.t_step / 100;

            // This is your game loop
            // Put your real-time logic and rendering in here
            m_window.updateInput();

            m_viewMatrix = trackball.viewMatrix();
            m_projectionMatrix = trackball.projectionMatrix();

            // Use ImGui for easy input/output of ints, floats, strings, etc...
            if (showUI) {
                ImGui::Begin("Assignment 2");

                ImGui::Combo("Scene", &sceneNr, scenes, 3);
                ImGui::SliderFloat("Time Speed", &data.t_step, 0.f, 1.f, "%.3f");
                ImGui::Text("Time: %.3f", data.time);
                ImGui::Separator();

                switch(sceneNr) {
                case 0: 
                    renderSolarSystemGUI();
                    break;
                case 1:
                    ImGui::Combo("Viewpoint", &selectedViewpoint, viewpoints, 2);
                    ImGui::ColorEdit3("Diffuse", glm::value_ptr(data.cupMaterial.m.kd));
                    ImGui::ColorEdit3("Floor", glm::value_ptr(data.cupMaterial.floorKd));

                    ImGui::Checkbox("Normal map", &data.useNormalMap);
                    ImGui::Checkbox("Advanced shading", &data.useAdvancedShading);
                    ImGui::Checkbox("Minimap", &data.drawMinimap);

                    ImGui::SliderFloat("Rho (Albedo)", &data.cupMaterial.rho, 0, 1, "%.2f");
                    ImGui::SliderFloat("Sigma (Rough)", &data.cupMaterial.sigma, 0, 1, "%.2f");
                    ImGui::Checkbox("Day/night cycle", &data.dayNightCycle);
                    ImGui::ColorEdit3("Daylight color", glm::value_ptr(data.dayColor));
                    ImGui::ColorEdit3("Nightlight color", glm::value_ptr(data.nightColor));
                    break;
                case 2:
                    ImGui::SliderFloat("Amplitude", &data.oceanData.amplitude, 0, 1, "%.2f");
                    ImGui::SliderFloat("X Frequency", &data.oceanData.fx, 0, 1, "%.2f");
                    ImGui::SliderFloat("Y Frequency", &data.oceanData.fz, 0, 1, "%.2f");
                    ImGui::SliderFloat("Time Frequency", &data.oceanData.ft, 0, 1, "%.2f");
                    ImGui::Checkbox("Subdivide", &data.oceanData.doSubdivide);
                    break;
                }

                ImGui::End();
            }
            // Clear the screen
            glClearColor(0.04f, 0.04f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            switch (sceneNr) {
            case 0:
                m_viewMatrix = trackball.viewMatrix();
                renderSolarSystemScene(data, m_projectionMatrix, m_viewMatrix);
                renderComet(data, m_projectionMatrix, m_viewMatrix);
                if (drawCometTrajectory) {
                    renderCometTrajectory(data, m_projectionMatrix, m_viewMatrix);
                }
                renderCometTrail(data, m_projectionMatrix, m_viewMatrix);
                break;
            case 1:
                if (selectedViewpoint == 0) {
                    m_viewMatrix = trackball.viewMatrix();
                }
                else {
                    // TODO: trackball still gets updated, even though we do not intend to
                    glm::vec3 cameraPos(5, 3, 5);
                    glm::vec3 target(0, 0, 0);
                    glm::vec3 up(0, 1, 0);
                    m_viewMatrix = glm::lookAt(cameraPos, target, up);
                }
                renderOnPlanetScene(data, minimapFramebuffer, m_projectionMatrix, m_viewMatrix);
                break;
            case 2:
                renderOcean(data);
            }
            // Processes input and swaps the window buffer
            m_window.swapBuffers();
        }
        deleteTexture(minimapTexture, minimapFramebuffer);
        destroyBuffers();
    }

    // In here you can handle key presses
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyPressed(int key, int mods)
    {
        std::cout << "Key pressed: " << key << std::endl;
        switch (key) {
            case GLFW_KEY_BACKSLASH: {
                showUI = !showUI;
                break;
            }
            default: {
                break;
            }
        }
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

    bool showUI;

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_shadowShader;

    // Normal Shaders!
    Shader simpleShader;
    Shader normalShader;
    Shader advancedShader;
    Shader cometShader;
    Shader cometTrailShader;
    Shader nightSkyShader;
    Shader minimapShader;
    Shader oceanShader;

    std::vector<GPUMesh> ball;
    std::vector<GPUMesh> cup;
    std::vector<GPUMesh> quad;
    std::vector<GPUMesh> ocean;

    Texture m_texture;
    bool m_useMaterial { true };

    Trackball trackball;

    // Projection and view matrices for you to fill in and use
    glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };

    Data data;

    Texture nightSky;
    Texture noise;
    Texture wallNormal;
    GLuint normalMap;

    GLuint minimapTexture;
    GLuint minimapFramebuffer;

    int selectedViewpoint = 0;

    bool drawCometTrajectory = false;
};

int main()
{
    Application app;
    app.update();

    return 0;
}