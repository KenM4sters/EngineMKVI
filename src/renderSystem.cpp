#include "renderSystem.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>

// std
#include <cassert>
#include <stdexcept>
#include <array>


namespace lve {

    struct SimplePushConstantData {
        glm::mat4 transform{1.0f};
        alignas(16) glm::vec3 color;
    };

    RenderSystem::RenderSystem(LveDevice& device, VkRenderPass renderPass) : lveDevice{device} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    RenderSystem::~RenderSystem() {vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);}

    void RenderSystem::createPipelineLayout() {
        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);



        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if(vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create PipelineLayout!");
        }

    }

    void RenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/vert.spv",
            "shaders/frag.spv",
            pipelineConfig
        );
    }

    void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &gameObjects) {
        lvePipeline->bind(commandBuffer);

        for(auto& obj: gameObjects) {   
            obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
            obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.01f, glm::two_pi<float>());
            SimplePushConstantData push{};
            push.color = obj.color;
            push.transform = obj.transform.mat4();
                
            vkCmdPushConstants(
                commandBuffer, 
                pipelineLayout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
                );
            obj.mesh->bind(commandBuffer);
            obj.mesh->draw(commandBuffer);
            
        }
    }
}