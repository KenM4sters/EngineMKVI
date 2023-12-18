#pragma once

#include "./window.hpp"
#include "./swap_chain.hpp"
#include <memory>
#include <vector>
#include <cassert>

namespace lve {
    class LveRenderer {

        public:

            LveRenderer(LveWindow &lveWindow, LveDevice &lveDevice);
            ~LveRenderer();

            LveRenderer(const LveWindow&) = delete;
            LveRenderer &operator=(const LveWindow&) = delete;

            VkRenderPass getSwapChainRenderPass() const {
                return lveSwapChain->getRenderPass();
            }
            bool isFrameInProgress() const {return isFrameStarted;}

            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(isFrameStarted && "Cannot get command buffer when frame is not in progress!");
                return commandBuffers[currentFrameIndex];
            }

            int getFrameIndex() const {
                assert(isFrameStarted && "cannot get frame index when frame is not in progress!");
                return currentFrameIndex;
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            LveWindow& lveWindow;
            LveDevice& lveDevice;
            std::unique_ptr<LveSwapChain> lveSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex = 0;
            bool isFrameStarted = false;

    };
}