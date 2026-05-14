#include "VkObject.h"
#include "Error/ErrorCore.h" // IWYU pragma: keep
//
// * Core functions
//
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
//
// * Setters && Getters
//
void VkObject::setInstance(const vk::Instance &arg_instance) noexcept {
  this->instance = arg_instance;
}

vk::Instance VkObject::getInstance() const { return this->instance; }