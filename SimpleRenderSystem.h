#pragma once

//STD
#include <memory>
#include <vector>
#include <cassert>

//GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//Local
#include "B3DDevice.h"
#include "B3DGameObj.h"
#include "B3DPipeline.h"
#include "B3DCamera.h"

class SimpleRenderSystem
{
	public:
		SimpleRenderSystem(B3DDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<B3DGameObj>& gameObjects, const B3DCamera& camera);

	private:

		B3DDevice& rSysDevice;

		std::unique_ptr<B3DPipeline> rSysPipeline;
		VkPipelineLayout rSysPipelineLayout;

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
};