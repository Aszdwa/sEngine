#include "Error/ErrorEnums.h"
#include "VkCore.h"          // IWYU pragma: keep
#include "VkObject.h"        // IWYU pragma: keep
#include "VkHelpers.h"       // IWYU pragma: keep
#include "Error/ErrorCore.h" // IWYU pragma: keep
#include <string>
using namespace ErrorContext;
//
void VulkanContext::VulkanCore::CreateInstance() {
  vk::Instance instance = VK_NULL_HANDLE;
  vk::ApplicationInfo appInfo{};
  appInfo.setPApplicationName("Hello Triangle")
      .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
      .setPEngineName("No Engine")
      .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
      .setApiVersion(VK_MAKE_API_VERSION(0, 1, 4, 0));

  // Get the required layers
  std::vector<char const *> requiredLayers =
      VkObject::sInstance().getValidationLayers();

  std::vector<const char *> requiredExtensions =
      VkObject::sInstance().getRequiredExtensions();

  vk::InstanceCreateInfo createInfo{};
  createInfo.setPApplicationInfo(&appInfo)
      .setEnabledLayerCount(static_cast<uint32_t>(requiredLayers.size()))
      .setPpEnabledLayerNames(requiredLayers.data())
      .setEnabledExtensionCount(
          static_cast<uint32_t>(requiredExtensions.size()))
      .setPpEnabledExtensionNames(requiredExtensions.data());

  try {
    instance = vk::createInstance(createInfo);
  } catch (const vk::SystemError &vkerr) {
    Error err = MAKE_ERROR(vkerr.code().value(), vkerr.what(), "Vulkan",
                           ErrorSeverity::Fatal);

    if (vkerr.code() == vk::Result::eErrorLayerNotPresent) {
      std::vector<const char *> unsupported;
      VkHelpers::findUnsupportedLayers(requiredLayers, &unsupported);
      std::string unsup_layers;
      for (const char *layer : unsupported) {
        unsup_layers += layer + (std::string) "\n";
        Error info = MAKE_ERROR(vkerr.code().value(), unsup_layers, "Vulkan",
                                ErrorSeverity::Info);
        REPORT_ERROR(info);
      }
    } else if (vkerr.code() == vk::Result::eErrorExtensionNotPresent) {
      std::vector<const char *> unsupported;
      VkHelpers::findUnsupportedExtensions(requiredExtensions, &unsupported);
      std::string unsup_extensions;
      for (const char *ext : unsupported) {
        unsup_extensions += ext + (std::string) "\n";
        Error info = MAKE_ERROR(vkerr.code().value(), unsup_extensions,
                                "Vulkan", ErrorSeverity::Info);
        REPORT_ERROR(info);
      }
    }
    REPORT_ERROR(err);
  }

  VkObject::sInstance().setInstance(instance);
}