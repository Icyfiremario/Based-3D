#include "B3DBuffer.h"

B3DBuffer::B3DBuffer(B3DDevice& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment) : bufferDevice{ device }, instanceSize{ instanceSize }, instanceCount{ instanceCount }, usageFlags{ usageFlags }, memoryPropertyFlags{ memoryPropertyFlags }
{
	alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
	bufferSize = alignmentSize * instanceCount;
	device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

B3DBuffer::~B3DBuffer()
{
	unmap();
	vkDestroyBuffer(bufferDevice.device(), buffer, nullptr);
	vkFreeMemory(bufferDevice.device(), memory, nullptr);
}

VkResult B3DBuffer::map(VkDeviceSize size, VkDeviceSize offset)
{
	assert(buffer && memory && "Called map on buffer before is was created!");
	return vkMapMemory(bufferDevice.device(), memory, offset, size, 0, &mapped);
}

void B3DBuffer::unmap()
{
	if (mapped)
	{
		vkUnmapMemory(bufferDevice.device(), memory);
		mapped = nullptr;
	}
}

void B3DBuffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
{
	assert(mapped && "Cannot copy into an unmapped buffer!");

	if (size == VK_WHOLE_SIZE)
	{
		memcpy(mapped, data, bufferSize);
	}
	else
	{
		char* memOffset = (char*)mapped;
		memOffset += offset;
		memcpy(memOffset, data, size);
	}
}

VkResult B3DBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	return vkFlushMappedMemoryRanges(bufferDevice.device(), 1, &mappedRange);
}

VkDescriptorBufferInfo B3DBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
{
	return VkDescriptorBufferInfo{buffer, offset, size};
}

VkResult B3DBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	return vkInvalidateMappedMemoryRanges(bufferDevice.device(), 1, &mappedRange);
}

void B3DBuffer::writeToIndex(void* data, int index)
{
	writeToBuffer(data, instanceSize, index * alignmentSize);
}

VkResult B3DBuffer::flushIndex(void* data, int index)
{
	return flush(alignmentSize, index * alignmentSize);
}

VkDescriptorBufferInfo B3DBuffer::descriptorInfoForIndex(int index)
{
	return descriptorInfo(alignmentSize, index * alignmentSize);
}

VkResult B3DBuffer::invalidateIndex(int index)
{
	return invalidate(alignmentSize, index * alignmentSize);
}

VkDeviceSize B3DBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
{
	if (minOffsetAlignment > 0)
	{
		return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
	}
	else
	{
		return instanceSize;
	}
}
