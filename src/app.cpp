#include "app.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <cassert>

namespace yellowstone {

	App::App() {
		loadGameObjects();
	}

	App::~App() {}

	void App::run() {
		SimpleRenderSystem simpleRenderSystem{ yellowstoneDevice, yellowstoneRenderer.getSwapChainRenderPass() };

		while (!yellowstoneWindow.shouldClose()) {
			glfwPollEvents();
			if (auto commandBuffer = yellowstoneRenderer.beginFrame()) {
				yellowstoneRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				yellowstoneRenderer.endSwapChainRenderPass(commandBuffer);
				yellowstoneRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(yellowstoneDevice.device());
	}

	void App::loadGameObjects() {
		std::vector<YellowstoneModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		auto yellowstoneModel = std::make_shared<YellowstoneModel>(yellowstoneDevice, vertices);
		auto triangle = YellowstoneGameObject::createGameObject();
		triangle.model = yellowstoneModel;
		triangle.color = { 1.0f, 0.0f, 0.0f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = { 2.f, .5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();
		gameObjects.push_back(std::move(triangle));
	}
}