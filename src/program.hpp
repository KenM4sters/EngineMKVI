#pragma once

#include "./window.hpp"
#include "./pipeline.hpp"
#include "./swap_chain.hpp"
#include "./mesh.hpp"
#include <memory>
#include <vector>

namespace lve {
    class App {

        public:
            static const int WIDTH = 800;
            static const int HEIGHT = 600;

            App();
            ~App();

            App(const LveWindow&) = delete;
            App &operator=(const LveWindow&) = delete;

            void run();


        private:
            void loadMeshes();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();

            LveWindow lveWindow{WIDTH, HEIGHT, "EngineMKII"};
            LveDevice lveDevice{lveWindow};
            LveSwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<LveMesh> lveMesh;

    };
}