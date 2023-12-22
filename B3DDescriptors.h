#pragma once

//STD
#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>

//Local
#include "B3DDevice.h"

class B3DDescriptorSetLayout
{
	public:

		class Builder
		{
			public:

				Builder(B3DDevice& device) : builderDevice{ device } {}

				Builder& addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);

				std::unique_ptr<B3DDescriptorSetLayout> build() const;

			private:
				B3DDevice& builderDevice;
				std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		B3DDescriptorSetLayout(B3DDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~B3DDescriptorSetLayout();

		B3DDescriptorSetLayout(const B3DDescriptorSetLayout&) = delete;
		B3DDescriptorSetLayout& operator=(const B3DDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		
		B3DDevice& desSetDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class B3DDescriptorWriter;
};

class B3DDescriptorPool
{
	public:

		class Builder
		{
			public:

				Builder(B3DDevice& device) : builderDevice{ device } {}

				Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
				Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
				Builder& setMaxSets(uint32_t count);
				std::unique_ptr<B3DDescriptorPool> build() const;

			private:

				B3DDevice& builderDevice;
				std::vector<VkDescriptorPoolSize> poolSizes{};
				uint32_t maxSets = 1000;
				VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		B3DDescriptorPool(B3DDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
		~B3DDescriptorPool();

		B3DDescriptorPool(const B3DDescriptorPool&) = delete;
		B3DDescriptorPool& operator=(const B3DDescriptorPool&) = delete;

		bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:

		B3DDevice& desPoolDevice;
		VkDescriptorPool descriptorPool;

		friend class B3DDescriptorWriter;
};

class B3DDescriptorWriter
{
	public:

		B3DDescriptorWriter(B3DDescriptorSetLayout& setLayout, B3DDescriptorPool &pool);

		B3DDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		B3DDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:

		B3DDescriptorSetLayout& setLayout;
		B3DDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;

};