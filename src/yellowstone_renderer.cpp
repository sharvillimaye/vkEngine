#include "yellowstone_renderer.hpp"

#include <stdexcept>
#include <cassert>
#include <array>

namespace yellowstone {

	YellowstoneRenderer::YellowstoneRenderer(YellowstoneWindow& window, YellowstoneDevice& device) : yellowstoneWindow{window}, yellowstoneDevice{device} {
		recreateSwapChain();
		createCommandBuffers();
	}

	YellowstoneRenderer::~YellowstoneRenderer() {
		freeCommandBuffers();
	}

	void YellowstoneRenderer::recreateSwapChain() {
		auto extent = yellowstoneWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = yellowstoneWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(yellowstoneDevice.device());

		if (yellowstoneSwapChain == nullptr) {
			yellowstoneSwapChain = std::make_unique<YellowstoneSwapChain>(yellowstoneDevice, extent);
		} else {
			std::shared_ptr<YellowstoneSwapChain> oldSwapChain = std::move(yellowstoneSwapChain);
			yellowstoneSwapChain = std::make_unique<YellowstoneSwapChain>(yellowstoneDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*yellowstoneSwapChain.get())) {
				throw std::runtime_error("Swap chain formats are not compatible!");
			}

		}

		// TODO
	}

	void YellowstoneRenderer::createCommandBuffers() {
		commandBuffers.resize(YellowstoneSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = yellowstoneDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		if (vkAllocateCommandBuffers(yellowstoneDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void YellowstoneRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(yellowstoneDevice.device(), yellowstoneDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer YellowstoneRenderer::beginFrame() {
		assert(!isFrameStarted && "Frame already started!");
		auto result = yellowstoneSwapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentFrameCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin command buffer!");
		}

		return commandBuffer;
	}

	void YellowstoneRenderer::endFrame() {
		assert(isFrameStarted && "Frame not started!");
		auto commandBuffer = getCurrentFrameCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = yellowstoneSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || yellowstoneWindow.wasWindowResized()) {
			yellowstoneWindow.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % YellowstoneSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void YellowstoneRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Frame not started!");
		assert(commandBuffer == getCurrentFrameCommandBuffer() && "CommandBuffer is not the current frame command buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = yellowstoneSwapChain->getRenderPass();
		renderPassInfo.framebuffer = yellowstoneSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = yellowstoneSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(yellowstoneSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(yellowstoneSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 0.0f;
		VkRect2D scissor{ {0, 0}, yellowstoneSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void YellowstoneRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Frame not started!");
		assert(commandBuffer == getCurrentFrameCommandBuffer() && "CommandBuffer is not the current frame command buffer!");

		vkCmdEndRenderPass(commandBuffer);
	}


}