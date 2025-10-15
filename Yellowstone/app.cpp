#include "app.hpp"

#include <stdexcept>

namespace yellowstone {

	App::App() {
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
		}
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
		
	}
	
	void App::drawFrame() {
		
	}
}