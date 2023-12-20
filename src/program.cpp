#include "./program.hpp"
#include "./camera.hpp"
#include "./renderSystem.hpp"
#include "./input.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <array>


namespace lve {

    App::App() {
        loadGameObjects();
    }

    App::~App() {}

    void App::run() {

        RenderSystem renderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
        LveCamera camera{};

        auto viewerObject = LveGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while(!lveWindow.shouldClose()) { 
            glfwPollEvents();
            float aspect = lveRenderer.getAspectRatio();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = 
                std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);


            // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }


    void App::loadGameObjects() {
        std::shared_ptr<LveMesh> lveMesh = LveMesh::createModelFromFile(
            lveDevice, "assets/models/smooth_vase.obj");

        auto cube = LveGameObject::createGameObject();
        cube.mesh = lveMesh;
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = glm::vec3(3.0f);
        gameObjects.push_back(std::move(cube));
    }

}