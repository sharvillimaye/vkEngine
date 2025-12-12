#include "app.hpp"
#include "yellowstone_camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "systems/physics_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <cassert>
#include <chrono>


namespace yellowstone {

	struct GlobalUbo {
		glm::mat4 projection{1.0f};
		glm::mat4 view{1.0f};
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
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(YellowstoneSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			YellowstoneDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ yellowstoneDevice, yellowstoneRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ yellowstoneDevice, yellowstoneRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PhysicsSystem physicsSystem{};
		YellowstoneCamera camera{};
		camera.setViewTarget(glm::vec3(-1.0f, -2.0f, -5.0f), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = YellowstoneGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		bool rKeyPressedLastFrame = false;

        while (!yellowstoneWindow.shouldClose()) {
			glfwPollEvents();

			// Check for R key to reset simulation (only trigger once per press)
			bool rKeyPressed = glfwGetKey(yellowstoneWindow.getWindow(), GLFW_KEY_R) == GLFW_PRESS;
			if (rKeyPressed && !rKeyPressedLastFrame) {
				resetSimulation();
			}
			rKeyPressedLastFrame = rKeyPressed;

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
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				// Update
				physicsSystem.update(frameInfo);
				
				GlobalUbo ubo{};
				ubo.projection = camera.getProjectionMatrix();
				ubo.view = camera.getViewMatrix();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				yellowstoneRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				yellowstoneRenderer.endSwapChainRenderPass(commandBuffer);
				yellowstoneRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(yellowstoneDevice.device());
	}

	void App::loadGameObjects() {
		// Load models
		std::shared_ptr<YellowstoneModel> cubeModel = YellowstoneModel::createModelFromFile(yellowstoneDevice, "../src/models/cube.obj");
		std::shared_ptr<YellowstoneModel> quadModel = YellowstoneModel::createModelFromFile(yellowstoneDevice, "../src/models/quad.obj");

		// Create ground plane (static)
		auto ground = YellowstoneGameObject::createGameObject();
		ground.model = quadModel;
		ground.transform.translation = {0.0f, 0.1f, 0.0f};
		ground.transform.scale = glm::vec3(10.0f, 1.0f, 10.0f);
		ground.physics.isStatic = true;
		ground.color = glm::vec3(0.3f, 0.3f, 0.3f);
		auto groundId = ground.getId();
		initialStates[groundId] = {
			ground.transform.translation,
			ground.transform.scale,
			ground.transform.rotation,
			ground.physics.velocity,
			ground.physics.mass,
			ground.physics.isStatic
		};
		gameObjects.emplace(groundId, std::move(ground));

		// Create falling cubes with different initial positions and velocities
		// In Y-down system: negative Y is above ground, positive Y is below ground
		for (int i = 0; i < 5; i++) {
			auto cube = YellowstoneGameObject::createGameObject();
			cube.model = cubeModel;
			cube.transform.translation = {
				-2.0f + i * 1.0f,
				-5.0f - i * 0.5f,  // Negative Y = above ground
				0.0f
			};
			cube.transform.scale = glm::vec3(0.3f, 0.3f, 0.3f);
			cube.physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			cube.physics.mass = 1.0f;
			cube.physics.isStatic = false;
			// Different colors for visual variety
			cube.color = glm::vec3(
				0.5f + (i % 3) * 0.2f,
				0.3f + ((i + 1) % 3) * 0.2f,
				0.4f + ((i + 2) % 3) * 0.2f
			);
			auto cubeId = cube.getId();
			initialStates[cubeId] = {
				cube.transform.translation,
				cube.transform.scale,
				cube.transform.rotation,
				cube.physics.velocity,
				cube.physics.mass,
				cube.physics.isStatic
			};
			gameObjects.emplace(cubeId, std::move(cube));
		}

		// Add a few cubes with initial horizontal velocity
		for (int i = 0; i < 3; i++) {
			auto cube = YellowstoneGameObject::createGameObject();
			cube.model = cubeModel;
			cube.transform.translation = {
				-1.5f + i * 1.5f,
				-8.0f,  // Negative Y = above ground
				1.0f
			};
			cube.transform.scale = glm::vec3(0.4f, 0.4f, 0.4f);
			cube.physics.velocity = glm::vec3(0.5f - i * 0.5f, 0.0f, -0.3f);
			cube.physics.mass = 1.5f;
			cube.physics.isStatic = false;
			cube.color = glm::vec3(0.8f, 0.2f, 0.2f);
			auto cubeId = cube.getId();
			initialStates[cubeId] = {
				cube.transform.translation,
				cube.transform.scale,
				cube.transform.rotation,
				cube.physics.velocity,
				cube.physics.mass,
				cube.physics.isStatic
			};
			gameObjects.emplace(cubeId, std::move(cube));
		}
	}

	void App::resetSimulation() {
		// Reset all game objects to their initial state
		for (auto& kv : gameObjects) {
			auto& obj = kv.second;
			auto id = obj.getId();
			
			// Only reset if we have initial state stored
			if (initialStates.find(id) != initialStates.end()) {
				const auto& initialState = initialStates[id];
				obj.transform.translation = initialState.translation;
				obj.transform.scale = initialState.scale;
				obj.transform.rotation = initialState.rotation;
				obj.physics.velocity = initialState.velocity;
				obj.physics.mass = initialState.mass;
				obj.physics.isStatic = initialState.isStatic;
			}
		}
	}
}