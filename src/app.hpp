#pragma once

#include "yellowstone_window.hpp"
#include "yellowstone_device.hpp"
#include "yellowstone_game_object.hpp"
#include "yellowstone_renderer.hpp"

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
		void loadGameObjects();

		YellowstoneWindow yellowstoneWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
		YellowstoneDevice yellowstoneDevice{yellowstoneWindow};
		YellowstoneRenderer yellowstoneRenderer{yellowstoneWindow, yellowstoneDevice};
		std::vector<YellowstoneGameObject> gameObjects;
	};
}