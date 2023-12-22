#pragma once

#include "B3DDevice.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <cassert>
#include <cstring>

class B3DModel
{
	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDecriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDecriptions();
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		B3DModel(B3DDevice& device, const std::vector<Vertex>& verticies, const B3DModel::Builder &builder);
		~B3DModel();

		B3DModel(const B3DModel&) = delete;
		B3DModel& operator=(const B3DModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		B3DDevice& modelDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		void createVertexBuffers(const std::vector<Vertex>& verticies);
};