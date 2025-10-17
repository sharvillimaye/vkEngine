#pragma once

#include "yellowstone_window.hpp"
#include "yellowstone_device.hpp"
#include "yellowstone_swap_chain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace yellowstone {

    class YellowstoneRenderer {
    public:
        YellowstoneRenderer(YellowstoneWindow& window, YellowstoneDevice& device);
        ~YellowstoneRenderer();
        YellowstoneRenderer(const YellowstoneRenderer&) = delete;
        YellowstoneRenderer& operator=(const YellowstoneRenderer&) = delete;

        bool isFrameInProgress() { return isFrameStarted; }
        VkRenderPass getSwapChainRenderPass() const { return yellowstoneSwapChain->getRenderPass(); }

        VkCommandBuffer getCurrentFrameCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer before frame has started");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index before frame has started");
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

        YellowstoneWindow& yellowstoneWindow;
        YellowstoneDevice& yellowstoneDevice;
        std::unique_ptr<YellowstoneSwapChain> yellowstoneSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;
        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted = false;
    };
}