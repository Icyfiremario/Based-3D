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
#include "B3DFrameInfo.h"

class SimpleRenderSystem
{
	public:
		SimpleRenderSystem(B3DDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects( FrameInfo &frameInfo, std::vector<B3DGameObj>& gameObjects);

	private:

		B3DDevice& rSysDevice;

		std::unique_ptr<B3DPipeline> rSysPipeline;
		VkPipelineLayout rSysPipelineLayout;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
};