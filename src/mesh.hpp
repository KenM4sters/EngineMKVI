#pragma once
#include "./device.hpp"
#include "./buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp> 

//std
#include <memory>
#include <vector>

namespace lve {
    class LveMesh {
        public:

            struct Vertex {
                glm::vec3 position{};
                glm::vec3 color{};
                glm::vec3 normal{};
                glm::vec2 uv{};

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

                bool operator==(const Vertex& other) const {
                    return position == other.position && color == other.color
                    && normal == other.normal && uv == other.uv;
                }
            };

            struct MeshData {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};

                void loadModel(const std::string &filePath);
            };

            LveMesh(LveDevice& device, const LveMesh::MeshData &builder);
            ~LveMesh();

            LveMesh(const LveWindow&) = delete;
            LveMesh &operator=(const LveWindow&) = delete;

            static std::unique_ptr<LveMesh> createModelFromFile(
                LveDevice &device, const std::string &filePath);

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);


        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);
            void createIndexBuffer(const std::vector<uint32_t> &indices);

            LveDevice& lveDevice;

            std::unique_ptr<LveBuffer> vertexBuffer;
            uint32_t vertexCount;

            bool hasIndexBuffer = false;
            
            std::unique_ptr<LveBuffer> indexBuffer;
            uint32_t indexCount;

    };
}