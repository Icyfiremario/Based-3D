#include "B3DPipeline.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

B3DPipeline::B3DPipeline(B3DDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo) : B3DPipelineDevice{ device }
{
	createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
}


PipelineConfigInfo B3DPipeline::deafultPipelineConfigInfo(uint32_t width, uint32_t height)
{
	PipelineConfigInfo configInfo{};

	return configInfo;
}

std::vector<char> B3DPipeline::readFile(const std::string& filePath)
{
	std::ifstream file{filePath, std::ios::ate | std::ios::binary};

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file: " + filePath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

void B3DPipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo)
{
	auto vertCode = readFile(vertFilePath);
	auto fragCode = readFile(fragFilePath);

	std::cout << "Vertex Shader Code Size: " << vertCode.size() << std::endl;
	std::cout << "Fragment Shader Code Size: " << fragCode.size() << std::endl;
}

void B3DPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(B3DPipelineDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}
}
