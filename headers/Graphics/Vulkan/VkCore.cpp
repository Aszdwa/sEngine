#include "VkCore.h"
#include "VkObject.h"
using namespace VulkanContext;
//
int VulkanCore::Initialize() {
  CreateInstance();

  return 0;
}
//
void VulkanCore::Cleanup() { VkObject::sInstance().Cleanup(); }