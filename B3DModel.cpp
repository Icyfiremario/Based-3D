#include "B3DModel.h"

B3DModel::B3DModel(B3DDevice& device, const std::vector<Vertex>& verticies) : modelDevice{device}
{
	createVertexBuffers(verticies);
}

B3DModel::~B3DModel()
{
	vkDestroyBuffer(modelDevice.device(), vertexBuffer, nullptr);
	vkFreeMemory(modelDevice.device(), vertexBufferMemory, nullptr);
}

void B3DModel::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void B3DModel::draw(VkCommandBuffer commandBuffer)
{
	vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void B3DModel::createVertexBuffers(const std::vector<Vertex>& verticies)
{
	vertexCount = static_cast<uint32_t>(verticies.size());

	assert(vertexCount >= 3 && "Vertex count must be at least 3");

	VkDeviceSize bufferSize = sizeof(verticies[0]) * vertexCount;

	modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);

	void* data;
	vkMapMemory(modelDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verticies.data(), static_cast<uint32_t>(bufferSize));
	vkUnmapMemory(modelDevice.device(), vertexBufferMemory);
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
