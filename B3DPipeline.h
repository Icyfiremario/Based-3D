#pragma once

#include <string>
#include <vector>
#include "B3DDevice.h"

struct PipelineConfigInfo {};

class B3DPipeline
{
	public:
		B3DPipeline(B3DDevice &device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo &configInfo);
		~B3DPipeline() {}

		B3DPipeline(const B3DPipeline&) = delete;
		void operator=(const B3DPipeline&) = delete;

		static PipelineConfigInfo deafultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:

		std::vector<char> readFile(const std::string& filePath);

		B3DDevice& B3DPipelineDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;


		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo &configInfo);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
};

