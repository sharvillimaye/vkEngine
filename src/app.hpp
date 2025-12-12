#pragma once

#include "yellowstone_window.hpp"
#include "yellowstone_device.hpp"
#include "yellowstone_game_object.hpp"
#include "yellowstone_renderer.hpp"
#include "yellowstone_descriptors.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

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
		void resetSimulation();

		struct InitialState {
			glm::vec3 translation;
			glm::vec3 scale;
			glm::vec3 rotation;
			glm::vec3 velocity;
			float mass;
			bool isStatic;
		};

		YellowstoneWindow yellowstoneWindow{WIDTH, HEIGHT, " Game Engine"};
		YellowstoneDevice yellowstoneDevice{yellowstoneWindow};
		YellowstoneRenderer yellowstoneRenderer{yellowstoneWindow, yellowstoneDevice};
		std::unique_ptr<YellowstoneDescriptorPool> globalPool{};

		YellowstoneGameObject::Map gameObjects;
		std::unordered_map<YellowstoneGameObject::id_t, InitialState> initialStates;
	};
}