#include "B3DModel.h"

//Defines

B3DModel::B3DModel(B3DDevice& device, const B3DModel::Builder& builder) : modelDevice{device}
{
	createVertexBuffers(builder.vertices);
	createIndexBuffers(builder.indices);
}

B3DModel::~B3DModel()
{
	vkDestroyBuffer(modelDevice.device(), vertexBuffer, nullptr);
	vkFreeMemory(modelDevice.device(), vertexBufferMemory, nullptr);

	if (hasIndexBuffer)
	{
		vkDestroyBuffer(modelDevice.device(), indexBuffer, nullptr);
		vkFreeMemory(modelDevice.device(), indexBufferMemory, nullptr);
	}
}

void B3DModel::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (hasIndexBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
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

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(modelDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verticies.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(modelDevice.device(), stagingBufferMemory);

	modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	modelDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(modelDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(modelDevice.device(), stagingBufferMemory, nullptr);
}

void B3DModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
	indexCount = static_cast<uint32_t>(indices.size());

	hasIndexBuffer = indexCount > 0;

	if (!hasIndexBuffer) return;

	VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(modelDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(modelDevice.device(), stagingBufferMemory);

	modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	modelDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(modelDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(modelDevice.device(), stagingBufferMemory, nullptr);
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
	std::vector<VkVertexInputAttributeDescription> attributeDescritptions(2);

	attributeDescritptions[0].binding = 0;
	attributeDescritptions[0].location = 0;
	attributeDescritptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescritptions[0].offset = offsetof(Vertex, position);

	attributeDescritptions[1].binding = 0;
	attributeDescritptions[1].location = 1;
	attributeDescritptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescritptions[1].offset = offsetof(Vertex, color);

	return attributeDescritptions;
}
