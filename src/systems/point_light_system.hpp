#pragma once

#include "../yellowstone_pipeline.hpp"
#include "../yellowstone_device.hpp"
#include "../yellowstone_frame_info.hpp"

#include <memory>
#include <vector>

namespace yellowstone {

    class PointLightSystem {
    public:
        PointLightSystem(YellowstoneDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();
        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void render(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        YellowstoneDevice& yellowstoneDevice;
        std::unique_ptr<YellowstonePipeline> yellowstonePipeline;
        VkPipelineLayout pipelineLayout;
    };
}