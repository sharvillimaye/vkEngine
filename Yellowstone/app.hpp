#pragma once

#include "yellowstone_window.hpp"
#include "yellowstone_pipeline.hpp"
#include "yellowstone_device.hpp"
#include "yellowstone_swap_chain.hpp"

#include <memory>
#include <vector>

namespace yellowstone {

	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		void run();

		App();
		~App();
		App(const App&) = delete;
		App& operator=(const App&) = delete;

	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		YellowstoneWindow yellowstoneWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
		YellowstoneDevice yellowstoneDevice{yellowstoneWindow};
		YellowstoneSwapChain yellowstoneSwapChain{ yellowstoneDevice, yellowstoneWindow.getExtent() };
		std::unique_ptr<YellowstonePipeline> yellowstonePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}