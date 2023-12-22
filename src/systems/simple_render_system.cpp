#include "simple_render_system.hpp"

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
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };

    RenderSystem::RenderSystem(LveDevice& device, 
        VkRenderPass renderPass, 
        VkDescriptorSetLayout globalSetLayout) : lveDevice{device} {

        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    RenderSystem::~RenderSystem() {vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);}

    void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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
            "shaders/shader.vert.spv",
            "shaders/shader.frag.spv",
            pipelineConfig
        );
    }

    void RenderSystem::renderGameObjects(FrameInfo &frameInfo) {

        lvePipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globaDescriptorSet,
            0,
            nullptr);
        
        

        for(auto& kv: frameInfo.gameObjects) {
            auto& obj = kv.second;
            if(obj.mesh == nullptr) continue;
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();
    
            vkCmdPushConstants(
                frameInfo.commandBuffer, 
                pipelineLayout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
                );
            obj.mesh->bind(frameInfo.commandBuffer);
            obj.mesh->draw(frameInfo.commandBuffer);
            
        }
    }
}