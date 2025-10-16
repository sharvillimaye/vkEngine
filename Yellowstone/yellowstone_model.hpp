#pragma once

#include "yellowstone_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace yellowstone {
	
	class YellowstoneModel {
	
	public:

		struct Vertex {
			glm::vec2 position;
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		YellowstoneModel(YellowstoneDevice &device, const std::vector<Vertex> &vertices);
		~YellowstoneModel();
		YellowstoneModel(const YellowstoneModel&) = delete;
		void operator=(const YellowstoneModel&) = delete;
		
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	
	private:

		void createVertexBuffers(const std::vector<Vertex>& vertices);

		YellowstoneDevice& yellowstoneDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}