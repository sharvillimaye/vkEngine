#pragma once

#include "yellowstone_device.hpp"

#include <string>
#include <vector>

namespace yellowstone {

	struct PipelineConfigInfo {

	};

	class YellowstonePipeline {
	public:
		YellowstonePipeline(YellowstoneDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
		~YellowstonePipeline() {}
		YellowstonePipeline(const YellowstonePipeline&) = delete;
		void operator=(const YellowstonePipeline&) = delete;
		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		YellowstoneDevice& yellowstoneDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}