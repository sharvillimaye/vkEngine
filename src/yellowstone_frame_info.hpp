#pragma once

#include <vulkan/vulkan.h>

#include "yellowstone_camera.hpp"

namespace yellowstone {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        YellowstoneCamera camera;
    };
}
