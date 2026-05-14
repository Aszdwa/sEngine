// ? Bridge between Vulkan core class and Vulkan object singleton
#include "VkCore.h"          // IWYU pragma: keep
#include "VkObject.h"        // IWYU pragma: keep
#include "Error/ErrorCore.h" // IWYU pragma: keep
// *
// * VkCore related code:
// *
namespace VulkanContext {
int VulkanCore::Initialize() {
  CreateInstance();

  return 0;
}

void VulkanCore::Cleanup() { VkObject::sInstance().Cleanup(); }

void VulkanCore::addValidationLayer(const char *c) {
  VkObject::sInstance().addValidationLayer(c);
}
}; // namespace VulkanContext
// *
// * VkObject related code:
// *
namespace VulkanContext {
VkObject::~VkObject() { this->Cleanup(); }

void VkObject::Cleanup() {

  if (this->instance != VK_NULL_HANDLE) {
    vkDestroyInstance(instance, nullptr); // Second param is allocator callback
    this->instance = VK_NULL_HANDLE;
  }
  ErrorContext::Error err =
      ErrorContext::MAKE_ERROR(0, "Finished vulkan cleanup", "Vulkan",
                               ErrorContext::ErrorSeverity::Info);
  REPORT_ERROR(err);
}

VkObject &VkObject::sInstance() {
  static VkObject s_instance;
  return s_instance;
}

// ? Setters && Getters
void VkObject::addValidationLayer(const char *c) noexcept {
  this->validationLayers.push_back(c);
}

void VkObject::setInstance(const vk::Instance &arg_instance) noexcept {
  this->instance = arg_instance;
}

vk::Instance VkObject::getInstance() const { return this->instance; }
}; // namespace VulkanContext