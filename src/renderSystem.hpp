#pragma once

#include "./pipeline.hpp"
#include "./game_object.hpp"
#include "./camera.hpp"
#include <memory>
#include <vector>

namespace lve {
    class RenderSystem {

        public:

            RenderSystem(LveDevice& device, VkRenderPass renderPass);
            ~RenderSystem();

            RenderSystem(const LveWindow&) = delete;
            RenderSystem &operator=(const LveWindow&) = delete;

            void renderGameObjects(
                VkCommandBuffer commandBuffer, 
                std::vector<LveGameObject> &gameObjects, 
                const LveCamera &camera);

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            LveDevice &lveDevice;
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;

    };
}