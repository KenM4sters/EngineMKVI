#include "./program.hpp"

#include "./renderSystem.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <cassert>
#include <array>


namespace lve {

    App::App() {
        loadGameObjects();
    }

    App::~App() {}

    void App::run() {

        RenderSystem renderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};

        while(!lveWindow.shouldClose()) { 
            glfwPollEvents();
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }

    void App::loadGameObjects() {
        std::vector<LveMesh::Vertex> vertices {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}
        };

        auto lveMesh = std::make_shared<LveMesh>(lveDevice, vertices);

        auto triangle = LveGameObject::createGameObject();
        triangle.mesh = lveMesh;
        triangle.color = {0.1f, 0.8f, 0.1f};
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = {2.0f, 0.5f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();
        gameObjects.push_back(std::move(triangle));
    }

}