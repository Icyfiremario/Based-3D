#include "B3DModel.h"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyObjectLoader/tiny_obj_loader.h>

namespace std
{
	template<>
	struct hash<B3DModel::Vertex>
	{
		size_t operator()(B3DModel::Vertex const& vertex) const
		{
			size_t seed = 0;
			B3DUtills::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
};

B3DModel::B3DModel(B3DDevice& device, const B3DModel::Builder& builder) : modelDevice{device}
{
	createVertexBuffers(builder.vertices);
	createIndexBuffers(builder.indices);
}

B3DModel::~B3DModel()
{
}

std::unique_ptr<B3DModel> B3DModel::createModelFromFile(B3DDevice& device, const std::string& filePath)
{
	Builder builder{};
	builder.loadModels(filePath);

	return std::make_unique<B3DModel>(device, builder);
}

void B3DModel::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { vertexBuffer->getBuffer()};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (hasIndexBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}

void B3DModel::draw(VkCommandBuffer commandBuffer)
{
	if (hasIndexBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}
}

void B3DModel::createVertexBuffers(const std::vector<Vertex>& verticies)
{
	vertexCount = static_cast<uint32_t>(verticies.size());

	assert(vertexCount >= 3 && "Vertex count must be at least 3");

	VkDeviceSize bufferSize = sizeof(verticies[0]) * vertexCount;

	uint32_t vertexSize = sizeof(verticies[0]);

	B3DBuffer stagingBuffer{modelDevice, vertexSize, vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)verticies.data());

	vertexBuffer = std::make_unique<B3DBuffer>(modelDevice, vertexSize, vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	modelDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void B3DModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
	indexCount = static_cast<uint32_t>(indices.size());

	hasIndexBuffer = indexCount > 0;

	if (!hasIndexBuffer) return;

	VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
	uint32_t indexSize = sizeof(indices[0]);

	B3DBuffer stagingBuffer{ modelDevice, indexSize, indexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)indices.data());

	indexBuffer = std::make_unique<B3DBuffer>(modelDevice, indexSize, indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	modelDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

std::vector<VkVertexInputBindingDescription> B3DModel::Vertex::getBindingDecriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> B3DModel::Vertex::getAttributeDecriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescritptions{};

	attributeDescritptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
	attributeDescritptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
	attributeDescritptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	attributeDescritptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

	return attributeDescritptions;
}

void B3DModel::Builder::loadModels(const std::string& filePath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	vertices.clear();
	indices.clear();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};

			if (index.vertex_index >= 0)
			{
				vertex.position = { attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2] };

				vertex.color = { attrib.colors[3 * index.vertex_index + 0], attrib.colors[3 * index.vertex_index + 1], attrib.colors[3 * index.vertex_index + 2] };
			}

			if (index.normal_index >= 0)
			{
				vertex.normal = { attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2] };
			}

			if (index.texcoord_index >= 0)
			{
				vertex.uv = { attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1]};
			}

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}
			
			indices.push_back(uniqueVertices[vertex]);
		}
	}
}
