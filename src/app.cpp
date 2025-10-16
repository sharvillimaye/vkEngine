#include "app.hpp"

#include <stdexcept>
#include <array>

namespace yellowstone {

	App::App() {
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	App::~App() {
		vkDestroyPipelineLayout(yellowstoneDevice.device(), pipelineLayout, nullptr);
	}

	void App::run() {
		while (!yellowstoneWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(yellowstoneDevice.device());
	}

	void App::loadModels() {
		std::vector<YellowstoneModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		yellowstoneModel = std::make_unique<YellowstoneModel>(yellowstoneDevice, vertices);
	}

	void App::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(yellowstoneDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void App::createPipeline() {
		assert(yellowstoneSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		YellowstonePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = yellowstoneSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		yellowstonePipeline = std::make_unique<YellowstonePipeline>(
			yellowstoneDevice,
			"../src/shaders/simple_shader.vert.spv",
			"../src/shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void App::createCommandBuffers() {
		commandBuffers.resize(yellowstoneSwapChain->imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = yellowstoneDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		if (vkAllocateCommandBuffers(yellowstoneDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void App::freeCommandBuffers() {
		vkFreeCommandBuffers(yellowstoneDevice.device(), yellowstoneDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void App::recordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = yellowstoneSwapChain->getRenderPass();
		renderPassInfo.framebuffer = yellowstoneSwapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = yellowstoneSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(yellowstoneSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(yellowstoneSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 0.0f;
		VkRect2D scissor{ {0, 0}, yellowstoneSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		yellowstonePipeline->bind(commandBuffers[imageIndex]);
		yellowstoneModel->bind(commandBuffers[imageIndex]);
		yellowstoneModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void App::recreateSwapChain() {
		auto extent = yellowstoneWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = yellowstoneWindow.getExtent();
			glfwWaitEvents();
		}
		
		vkDeviceWaitIdle(yellowstoneDevice.device());

		if (yellowstoneSwapChain == nullptr) {
			yellowstoneSwapChain = std::make_unique<YellowstoneSwapChain>(yellowstoneDevice, extent);
		} else {
			yellowstoneSwapChain = std::make_unique<YellowstoneSwapChain>(yellowstoneDevice, extent, std::move(yellowstoneSwapChain));
			if (yellowstoneSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		
		createPipeline();
	}
	
	void App::drawFrame() {
		uint32_t imageIndex;
		auto result = yellowstoneSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = yellowstoneSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || yellowstoneWindow.wasWindowResized()) {
			yellowstoneWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}
}