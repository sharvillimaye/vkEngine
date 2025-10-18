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

	struct GlobalUbo {
		glm::mat4 projectionViewMatrix{1.0f};
		glm::vec4 ambientLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.02f);
		glm::vec3 lightPosition = glm::vec3(-1.0f);
		alignas(16) glm::vec4 lightColor = glm::vec4(1.0f);
	};

	App::App() {
		globalPool = YellowstoneDescriptorPool::Builder(yellowstoneDevice)
			.setMaxSets(YellowstoneSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, YellowstoneSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	App::~App() {}

	void App::run() {
		std::vector<std::unique_ptr<YellowstoneBuffer>> uboBuffers(YellowstoneSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (auto & uboBuffer : uboBuffers) {
			uboBuffer = std::make_unique<YellowstoneBuffer>(
				yellowstoneDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffer->map();
		}

		auto globalSetLayout = YellowstoneDescriptorSetLayout::Builder(yellowstoneDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(YellowstoneSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			YellowstoneDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ yellowstoneDevice, yellowstoneRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		YellowstoneCamera camera{};
		camera.setViewTarget(glm::vec3(-1.0f, -2.0f, -5.0f), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = YellowstoneGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

        while (!yellowstoneWindow.shouldClose()) {
			glfwPollEvents();

        	auto newTime = std::chrono::high_resolution_clock::now();
        	float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
        	currentTime = newTime;
			// frameTime = std::min(frameTime, MAX_FRAME_TIME);

        	cameraController.moveInPlaneXZ(yellowstoneWindow.getWindow(), frameTime, viewerObject);
        	camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = yellowstoneRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
			if (auto commandBuffer = yellowstoneRenderer.beginFrame()) {
				int frameIndex = yellowstoneRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};

				// Update
				GlobalUbo ubo{};
				ubo.projectionViewMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				yellowstoneRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				yellowstoneRenderer.endSwapChainRenderPass(commandBuffer);
				yellowstoneRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(yellowstoneDevice.device());
	}

	void App::loadGameObjects() {
		std::shared_ptr<YellowstoneModel> yellowstoneModel = YellowstoneModel::createModelFromFile(yellowstoneDevice, "../src/models/smooth_vase.obj");
		auto smoothVase = YellowstoneGameObject::createGameObject();
		smoothVase.model = yellowstoneModel;
		smoothVase.transform.translation = {-0.5f, 0.5f, 0.0f};
		smoothVase.transform.scale = glm::vec3(3.0f, 1.5f, 3.0f);
		gameObjects.push_back(std::move(smoothVase));

		yellowstoneModel = YellowstoneModel::createModelFromFile(yellowstoneDevice, "../src/models/flat_vase.obj");
		auto flatVase = YellowstoneGameObject::createGameObject();
		flatVase.model = yellowstoneModel;
		flatVase.transform.translation = {0.5f, 0.5f, 0.0f};
		flatVase.transform.scale = glm::vec3(3.0f, 1.5f, 3.0f);
		gameObjects.push_back(std::move(flatVase));

		yellowstoneModel = YellowstoneModel::createModelFromFile(yellowstoneDevice, "../src/models/quad.obj");
		auto quadFloor = YellowstoneGameObject::createGameObject();
		quadFloor.model = yellowstoneModel;
		quadFloor.transform.translation = {0.0f, 0.5f, 0.0f};
		quadFloor.transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
		gameObjects.push_back(std::move(quadFloor));
	}
}