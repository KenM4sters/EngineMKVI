#pragma once

#include "./camera.hpp"
#include "./game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace lve {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LveCamera &camera;
        VkDescriptorSet globaDescriptorSet;
        LveGameObject::Map &gameObjects;

    };
}