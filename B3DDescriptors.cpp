#include "B3DDescriptors.h"

B3DDescriptorSetLayout::Builder& B3DDescriptorSetLayout::Builder::addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count)
{
    assert(bindings.count(binding) == 0 && "Binding already in use!");

    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;

    return *this;
}

B3DDescriptorPool::Builder& B3DDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
    poolSizes.push_back({ descriptorType, count });
    return *this;
}

B3DDescriptorPool::Builder& B3DDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
{
    poolFlags = flags;
    return *this;
}

B3DDescriptorPool::Builder& B3DDescriptorPool::Builder::setMaxSets(uint32_t count)
{
    maxSets = count;
    return *this;
}

std::unique_ptr<B3DDescriptorPool> B3DDescriptorPool::Builder::build() const
{
    return std::make_unique<B3DDescriptorPool>(builderDevice, maxSets, poolFlags, poolSizes);
}

std::unique_ptr<B3DDescriptorSetLayout> B3DDescriptorSetLayout::Builder::build() const
{
    return std::make_unique<B3DDescriptorSetLayout>(builderDevice, bindings);
}

B3DDescriptorSetLayout::B3DDescriptorSetLayout(B3DDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings) : desSetDevice{device}, bindings{bindings}
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};

    for (auto kv : bindings)
    {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(desSetDevice.device(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

B3DDescriptorSetLayout::~B3DDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(desSetDevice.device(), descriptorSetLayout, nullptr);
}

B3DDescriptorPool::B3DDescriptorPool(B3DDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes) : desPoolDevice{device}
{
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};

    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(desPoolDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

B3DDescriptorPool::~B3DDescriptorPool()
{
    vkDestroyDescriptorPool(desPoolDevice.device(), descriptorPool, nullptr);
}

bool B3DDescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    if (vkAllocateDescriptorSets(desPoolDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void B3DDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
{
    vkFreeDescriptorSets(desPoolDevice.device(), descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void B3DDescriptorPool::resetPool()
{
    vkResetDescriptorPool(desPoolDevice.device(), descriptorPool, 0);
}

B3DDescriptorWriter::B3DDescriptorWriter(B3DDescriptorSetLayout& setLayout, B3DDescriptorPool& pool) : setLayout{setLayout}, pool{pool}
{

}

B3DDescriptorWriter& B3DDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding!");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info when binding expect multiple!");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

B3DDescriptorWriter& B3DDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding!");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info when binding expect multiple!");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

bool B3DDescriptorWriter::build(VkDescriptorSet& set)
{
    bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
    if (!success)
    {
        return false;
    }
    overwrite(set);
    return true;
}

void B3DDescriptorWriter::overwrite(VkDescriptorSet& set)
{
    for (auto& write : writes)
    {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(pool.desPoolDevice.device(), writes.size(), writes.data(), 0, nullptr);
}
