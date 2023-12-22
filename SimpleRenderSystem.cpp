#include "SimpleRenderSystem.h"

struct SimplePushConstantData
{
	glm::mat4 transform{ 1.f };
	alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(B3DDevice& device, VkRenderPass renderPass) : rSysDevice{device}
{
	createPipelineLayout();
	createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem()
{
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<B3DGameObj>& gameObjects, const B3DCamera& camera)
{
	rSysPipeline->bind(commandBuffer);

	auto projectionView = camera.getProjection() * camera.getView();

	for (auto& obj : gameObjects)
	{
		SimplePushConstantData push{};
		push.color = obj.color;
		push.transform = projectionView * obj.transform.mat4();

		vkCmdPushConstants(commandBuffer, rSysPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
		obj.model->bind(commandBuffer);
		obj.model->draw(commandBuffer);
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
