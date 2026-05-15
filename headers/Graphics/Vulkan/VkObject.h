#ifndef VULKAN_OBJECT_H
#define VULKAN_OBJECT_H
//
#include "VkInclude.h"                   // IWYU pragma: keep
#include "Compatibility/Compatibility.h" // IWYU pragma: keep
#include <vector>
//
namespace VulkanContext {
class VkObject {
public:
  VkObject() = default;
  static VkObject &sInstance();
  VkObject(const VkObject &) = delete;
  void operator=(const VkObject &) = delete;

public:
  void Cleanup();
  ~VkObject();

public: // ? Setters
  void addValidationLayer(const char *c) noexcept;
  void setEnableValidationLayers(const bool &b);
  void setInstance(const vk::Instance &instance) noexcept;

public: // ? Getters
  bool getEnableValidationLayers() const;
  const std::vector<const char *> getValidationLayers();
  const std::vector<const char *> getRequiredExtensions();
  vk::Instance getInstance() const;

private:
  bool enableValidationLayers = false;
  std::vector<const char *> validationLayers = {};
  std::vector<const char *> instanceExtensions = {};
  vk::Instance instance = VK_NULL_HANDLE;
};
}; // namespace VulkanContext
#endif // VULKAN_OBJECT_H