#pragma once

#include "./pipeline.hpp"
#include "./game_object.hpp"
#include "./camera.hpp"
#include "./frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
    class RenderSystem {

        public:

            RenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~RenderSystem();

            RenderSystem(const LveWindow&) = delete;
            RenderSystem &operator=(const LveWindow&) = delete;

            void renderGameObjects(
                FrameInfo &frameInfo
            ); 

        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);

            LveDevice &lveDevice;
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;

    };
}