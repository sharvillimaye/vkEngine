#pragma once

#include "yellowstone_window.hpp"
#include "yellowstone_pipeline.hpp"
#include "yellowstone_device.hpp"

namespace yellowstone {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		void run();
	private:
		YellowstoneWindow yellowstoneWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
		YellowstoneDevice yellowstoneDevice{yellowstoneWindow};
		YellowstonePipeline yellowstonePipeline{
			yellowstoneDevice, 
			"shaders/simple_shader.vert.spv", 
			"shaders/simple_shader.frag.spv", 
			YellowstonePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
		};
	};
}