#pragma once

#include <string>
#include <vector>

#include "B3DDevice.h"
#include "B3DModel.h"

struct PipelineConfigInfo
{
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class B3DPipeline
{
	public:
		B3DPipeline(B3DDevice &device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo &configInfo);
		~B3DPipeline();

		B3DPipeline(const B3DPipeline&) = delete;
		B3DPipeline& operator=(const B3DPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		static void deafultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:

		std::vector<char> readFile(const std::string& filePath);

		B3DDevice& B3DPipelineDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;


		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo &configInfo);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
};

