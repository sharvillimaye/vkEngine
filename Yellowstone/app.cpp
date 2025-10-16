#include "app.hpp"

#include <stdexcept>
#include <array>

namespace yellowstone {

	App::App() {
		loadModels();
		createPipelineLayout();
		createPipeline();
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
			{{0.0f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}}
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
		auto pipelineConfig = YellowstonePipeline::defaultPipelineConfigInfo(yellowstoneSwapChain.width(), yellowstoneSwapChain.height());
		pipelineConfig.renderPass = yellowstoneSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		yellowstonePipeline = std::make_unique<YellowstonePipeline>(
			yellowstoneDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void App::createCommandBuffers() {
		commandBuffers.resize(yellowstoneSwapChain.imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = yellowstoneDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		if (vkAllocateCommandBuffers(yellowstoneDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = yellowstoneSwapChain.getRenderPass();
			renderPassInfo.framebuffer = yellowstoneSwapChain.getFrameBuffer(i);
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = yellowstoneSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			yellowstonePipeline->bind(commandBuffers[i]);
			yellowstoneModel->bind(commandBuffers[i]);
			yellowstoneModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}
	
	void App::drawFrame() {
		uint32_t imageIndex;
		auto result = yellowstoneSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		result = yellowstoneSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}
}