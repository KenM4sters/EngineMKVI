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
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <array>


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
        viewerObject.transform.translation.z = -2.5f;
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
            lveDevice, "assets/models/smooth_vase.obj");

        std::shared_ptr<LveMesh> lveMesh_2 = LveMesh::createModelFromFile(
            lveDevice, "assets/models/flat_vase.obj");

        std::shared_ptr<LveMesh> lve_floor = LveMesh::createModelFromFile(
            lveDevice, "assets/models/quad.obj");

        auto vase_smooth = LveGameObject::createGameObject();
        vase_smooth.mesh = lveMesh;
        vase_smooth.transform.translation = {-0.5f, 0.5f, 0.0f};
        vase_smooth.transform.scale = glm::vec3(3.0f);
        gameObjects.emplace(vase_smooth.getId(), std::move(vase_smooth));
        
        auto vase_flat = LveGameObject::createGameObject();
        vase_flat.mesh = lveMesh_2;
        vase_flat.transform.translation = {0.5f, 0.5f, 0.0f};
        vase_flat.transform.scale = glm::vec3(3.0f);
        gameObjects.emplace(vase_flat.getId(), std::move(vase_flat));

        auto floor = LveGameObject::createGameObject();
        floor.mesh = lve_floor;
        floor.transform.translation = {0.0f, 0.5f, 0.0f};
        floor.transform.scale = glm::vec3(3.0f);
        gameObjects.emplace(floor.getId(), std::move(floor));
        
         std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f} 
        };
        
        for(int i = 0; i < lightColors.size(); i++) {
            auto pointLight = LveGameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(
                glm::mat4(1.0f), 
                (i * glm::two_pi<float>() / lightColors.size()),
                {0.0f, -1.0f, 0.0f});
            
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
    }
}