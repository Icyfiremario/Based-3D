#pragma once

//Local
#include "B3DDevice.h"
#include "B3DUtils.h"
#include "B3DBuffer.h"

//GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

//STD
#include <vector>
#include <cassert>
#include <cstring>
#include <memory>
#include <unordered_map>


class B3DModel
{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDecriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDecriptions();

			bool operator==(const Vertex& other) const { return position == other.position && color == other.color && normal == other.normal && uv == other.uv; }
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModels(const std::string& filePath);
		};

		B3DModel(B3DDevice& device, const B3DModel::Builder &builder);
		~B3DModel();

		B3DModel(const B3DModel&) = delete;
		B3DModel& operator=(const B3DModel&) = delete;

		static std::unique_ptr<B3DModel> createModelFromFile(B3DDevice &device, const std::string &filePath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		B3DDevice& modelDevice;

		std::unique_ptr<B3DBuffer> vertexBuffer;
		uint32_t vertexCount;

		std::unique_ptr<B3DBuffer> indexBuffer;
		uint32_t indexCount;
		bool hasIndexBuffer = false;

		void createVertexBuffers(const std::vector<Vertex>& verticies);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
};