#pragma once

#include <vulkan/vulkan.h>

#include "yellowstone_camera.hpp"
#include "yellowstone_game_object.hpp"

namespace yellowstone {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        YellowstoneCamera camera;
        VkDescriptorSet descriptorSet;
        YellowstoneGameObject::Map& gameObjects;
    };
}
