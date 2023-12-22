#pragma once

//Local
#include "B3DCamera.h"

//Vulkan
#include <vulkan/vulkan.h>


struct FrameInfo
{
	int frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	B3DCamera& camera;
	VkDescriptorSet globalDescriptorSet;
};