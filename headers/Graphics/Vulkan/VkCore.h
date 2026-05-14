#ifndef VULKAN_CORE_H
#define VULKAN_CORE_H
//
namespace VulkanContext {
//
class VulkanCore {
public:
  VulkanCore() = default;
  ~VulkanCore() = default;

public:
  void CreateInstance();

public:
  int Initialize();
  void Cleanup();
}; // class VulkanCore

}; // namespace VulkanContext
#endif // VULKAN_CORE_H