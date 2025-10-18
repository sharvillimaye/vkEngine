#pragma once

#include "yellowstone_pipeline.hpp"
#include "yellowstone_device.hpp"
#include "yellowstone_game_object.hpp"
#include "yellowstone_camera.hpp"
#include "yellowstone_frame_info.hpp"

#include <memory>
#include <vector>

namespace yellowstone {

    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(YellowstoneDevice& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();
        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(FrameInfo& frameInfo, std::vector<YellowstoneGameObject>& gameObjects);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        YellowstoneDevice& yellowstoneDevice;
        std::unique_ptr<YellowstonePipeline> yellowstonePipeline;
        VkPipelineLayout pipelineLayout;
    };
}