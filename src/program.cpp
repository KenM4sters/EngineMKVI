#include "./program.hpp"
#include "./camera.hpp"
#include "./systems/simple_render_system.hpp"
#include "./systems/point_light_system.hpp"
#include "./input.hpp"
#include "./buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>

// std
#include <cstdlib>
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <array>
#include <iostream>


namespace lve {
    
    App::App() {
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        loadGameObjects();
    }

    App::~App() {}

    void App::run() {

        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();

        }

        auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i <globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        RenderSystem renderSystem{
            lveDevice, 
            lveRenderer.getSwapChainRenderPass(), 
            globalSetLayout->getDescriptorSetLayout()};

        PointLightSystem pointLightSystem{
            lveDevice, 
            lveRenderer.getSwapChainRenderPass(), 
            globalSetLayout->getDescriptorSetLayout()};

        LveCamera camera{};

        auto viewerObject = LveGameObject::createGameObject();
        auto shipIndex = gameObjects.find(0);

        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};
        KeyboardMovementController shipController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while(!lveWindow.shouldClose()) { 
            glfwPollEvents();
            float aspect = lveRenderer.getAspectRatio();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = 
                std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // cameraController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            shipController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, shipIndex->second);

            viewerObject.transform.translation = shipIndex->second.transform.translation + glm::vec3(0.0f, -0.5f, -4.0f);

            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();

                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects};

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                
                // order here matters - rendersystem before pointLightSystem for accurate alpha blending
                renderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }


    void App::loadGameObjects() {
        std::shared_ptr<LveMesh> lveMesh = LveMesh::createModelFromFile(
            lveDevice, "assets/models/Star_destroyer.obj");

        // std::shared_ptr<LveMesh> lveMesh_2 = LveMesh::createModelFromFile(
        //     lveDevice, "assets/models/flat_vase.obj");

        // std::shared_ptr<LveMesh> lve_floor = LveMesh::createModelFromFile(
        //     lveDevice, "assets/models/quad.obj");

        auto star_destroyer = LveGameObject::createGameObject();
        star_destroyer.mesh = lveMesh;
        star_destroyer.transform.translation.y = {0.5f};
        star_destroyer.transform.scale = glm::vec3(0.25f);
        gameObjects.emplace(star_destroyer.getId(), std::move(star_destroyer));
        
        // auto vase_flat = LveGameObject::createGameObject();
        // vase_flat.mesh = lveMesh_2;
        // vase_flat.transform.translation = {0.5f, 0.5f, 0.0f};
        // vase_flat.transform.scale = glm::vec3(3.0f);
        // gameObjects.emplace(vase_flat.getId(), std::move(vase_flat));

        // auto floor = LveGameObject::createGameObject();
        // floor.mesh = lve_floor;
        // floor.transform.translation = {0.0f, 0.5f, 0.0f};
        // floor.transform.scale = glm::vec3(3.0f);
        // gameObjects.emplace(floor.getId(), std::move(floor));
        
         std::vector<glm::vec3> lightColors{
            {1.0f, 0.1f, 0.1f},
            {0.1f, 0.1f, 1.0f},
            {1.0f, 0.1f, 1.0f}
        };
        
        srand((unsigned)time(NULL));

        auto centerPointLight = LveGameObject::makePointLight();
        centerPointLight.color = {1.0f, 1.0f, 1.0f};
        centerPointLight.transform.translation = glm::vec3(0.0f, -5.0f, 0.0f);
        gameObjects.emplace(centerPointLight.getId(), std::move(centerPointLight));

        for(int i = 0; i < MAX_LIGHTS - 1; i++) {
            
            int colorIndex = rand() % lightColors.size();

            int randX = (rand() % 100) - 50;
            int randY = (rand() % 100) - 50;
            int randZ = (rand() % 100) - 50;

            // std::cout << randX << std::endl;
            // std::cout << randY << std::endl;
            // std::cout << randZ << std::endl;

            auto pointLight = LveGameObject::makePointLight(0.2f);
            pointLight.color = lightColors[colorIndex];
            pointLight.transform.translation = glm::vec3(randX, randY, randZ);
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));

        }
        updateGameObject();
    }

    void App::updateGameObject() {
        auto it = gameObjects.find(0);
        
        std::cout << it->second.transform.scale.y << std::endl; 
    }
}