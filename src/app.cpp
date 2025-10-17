#include "app.hpp"
#include "simple_render_system.hpp"
#include "yellowstone_camera.hpp"
#include "keyboard_movement_controller.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <cassert>
#include <chrono>

namespace yellowstone {

	App::App() {
		loadGameObjects();
	}

	App::~App() {}

	void App::run() {
		SimpleRenderSystem simpleRenderSystem{ yellowstoneDevice, yellowstoneRenderer.getSwapChainRenderPass() };
		YellowstoneCamera camera{};
		camera.setViewTarget(glm::vec3(-1.0f, -2.0f, -5.0f), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewer = YellowstoneGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

        while (!yellowstoneWindow.shouldClose()) {
			glfwPollEvents();

        	auto newTime = std::chrono::high_resolution_clock::now();
        	float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
        	currentTime = newTime;
			// frameTime = std::min(frameTime, MAX_FRAME_TIME);

        	cameraController.moveInPlaneXZ(yellowstoneWindow.getWindow(), frameTime, viewer);
        	camera.setViewYXZ(viewer.transform.translation, viewer.transform.rotation);

			float aspect = yellowstoneRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
			if (auto commandBuffer = yellowstoneRenderer.beginFrame()) {
				yellowstoneRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				yellowstoneRenderer.endSwapChainRenderPass(commandBuffer);
				yellowstoneRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(yellowstoneDevice.device());
	}

	void App::loadGameObjects() {
		std::shared_ptr<YellowstoneModel> yellowstoneModel = YellowstoneModel::createModelFromFile(yellowstoneDevice, "../src/models/smooth_vase.obj");
		auto gameObject = YellowstoneGameObject::createGameObject();
		gameObject.model = yellowstoneModel;
		gameObject.transform.translation = {0.0f, 0.0f, 2.5f};
		gameObject.transform.scale = glm::vec3(3.0f);
		gameObjects.push_back(std::move(gameObject));
	}
}