#pragma once
#include "./device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp> 

//std
#include <vector>

namespace lve {
    class LveMesh {
        public:

            struct Vertex {
                glm::vec2 position;
                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            };

            LveMesh(LveDevice& device, const std::vector<Vertex> &vertices);
            ~LveMesh();

            LveMesh(const LveWindow&) = delete;
            LveMesh &operator=(const LveWindow&) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);


        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);

            LveDevice& lveDevice;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;

    };
}