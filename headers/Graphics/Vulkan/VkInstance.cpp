#include "VkCore.h"   // IWYU pragma: keep
#include "VkObject.h" // IWYU pragma: keep
//
void VulkanContext::VulkanCore::CreateInstance() {
  vk::Instance instance;

  VkObject::sInstance().setInstance(instance);
}