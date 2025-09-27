#pragma once

#include "yellowstone_window.hpp"
#include "yellowstone_pipeline.hpp"

namespace yellowstone {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		void run();
	private:
		YellowstoneWindow yellowstoneWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		YellowstonePipeline yellowstonePipeline{ "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv" };
	};
}