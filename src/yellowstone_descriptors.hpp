#pragma once

#include "yellowstone_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace yellowstone {

class YellowstoneDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(YellowstoneDevice &yellowstoneDevice) : yellowstoneDevice{yellowstoneDevice} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<YellowstoneDescriptorSetLayout> build() const;

   private:
    YellowstoneDevice &yellowstoneDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  YellowstoneDescriptorSetLayout(
      YellowstoneDevice &yellowstoneDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~YellowstoneDescriptorSetLayout();
  YellowstoneDescriptorSetLayout(const YellowstoneDescriptorSetLayout &) = delete;
  YellowstoneDescriptorSetLayout &operator=(const YellowstoneDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  YellowstoneDevice &yellowstoneDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class YellowstoneDescriptorWriter;
};

class YellowstoneDescriptorPool {
 public:
  class Builder {
   public:
    Builder(YellowstoneDevice &yellowstoneDevice) : yellowstoneDevice{yellowstoneDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<YellowstoneDescriptorPool> build() const;

   private:
    YellowstoneDevice &yellowstoneDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  YellowstoneDescriptorPool(
      YellowstoneDevice &yellowstoneDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~YellowstoneDescriptorPool();
  YellowstoneDescriptorPool(const YellowstoneDescriptorPool &) = delete;
  YellowstoneDescriptorPool &operator=(const YellowstoneDescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  YellowstoneDevice &yellowstoneDevice;
  VkDescriptorPool descriptorPool;

  friend class YellowstoneDescriptorWriter;
};

class YellowstoneDescriptorWriter {
 public:
  YellowstoneDescriptorWriter(YellowstoneDescriptorSetLayout &setLayout, YellowstoneDescriptorPool &pool);

  YellowstoneDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  YellowstoneDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  YellowstoneDescriptorSetLayout &setLayout;
  YellowstoneDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}