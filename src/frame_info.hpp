#pragma once

#include "./camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace lve {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LveCamera &camera;
    };
}