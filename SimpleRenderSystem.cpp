#include "SimpleRenderSystem.h"

struct SimplePushConstantData
{
	glm::mat4 transform{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
};

SimpleRenderSystem::SimpleRenderSystem(B3DDevice& device, VkRenderPass renderPass) : rSysDevice{device}
{
	createPipelineLayout();
	createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem()
{
}

void SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo, std::vector<B3DGameObj>& gameObjects)
{
	rSysPipeline->bind(frameInfo.commandBuffer);

	auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

	for (auto& obj : gameObjects)
	{
		SimplePushConstantData push{};
		auto modelMatrix = obj.transform.mat4();
		push.transform = projectionView * modelMatrix;
		push.normalMatrix = obj.transform.normalMatrix();

		vkCmdPushConstants(frameInfo.commandBuffer, rSysPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}
}

void SimpleRenderSystem::createPipelineLayout()
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(rSysDevice.device(), &pipelineLayoutInfo, nullptr, &rSysPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
{
	assert(rSysPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};

	B3DPipeline::deafultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = rSysPipelineLayout;

	rSysPipeline = std::make_unique<B3DPipeline>(rSysDevice, "simple_shader.vert.spv", "simple_shader.frag.spv", pipelineConfig);
}
