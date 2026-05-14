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
  void setInstance(const vk::Instance &instance) noexcept;

public: // ? Getters
  vk::Instance getInstance() const;

private:
  std::vector<const char *> validationLayers = {};
  vk::Instance instance = VK_NULL_HANDLE;
};
}; // namespace VulkanContext