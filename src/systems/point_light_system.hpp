#pragma once

#include "../pipeline.hpp"
#include "../game_object.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
    class PointLightSystem {

        public:

            PointLightSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~PointLightSystem();

            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem &operator=(const PointLightSystem&) = delete;

            void update(FrameInfo &frameInfo, GlobalUbo &ubo); 
            void render(FrameInfo &frameInfo); 

        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);

            LveDevice &lveDevice;
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;

    };
}