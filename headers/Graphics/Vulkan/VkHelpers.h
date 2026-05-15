#ifndef VULKAN_HELPERS_H
#define VULKAN_HELPERS_H
//
#include "VkCore.h"   // IWYU pragma: keep
#include "VkObject.h" // IWYU pragma: keep
//
namespace VulkanContext {
//
namespace VkHelpers {
// * Validation layers
bool isLayerSupported(const char *layer);
bool hasUnsupportedLayers(const std::vector<const char *> layers);
bool findUnsupportedLayers(const std::vector<const char *> layers,
                           std::vector<const char *> *unsup);
// * Extensions
bool isExtensionSupported(const char *extension);
bool hasUnsupportedExtensions(const std::vector<const char *> extensions);
bool findUnsupportedExtensions(const std::vector<const char *> extensions,
                               std::vector<const char *> *unsup);
}; // namespace VkHelpers
}; // namespace VulkanContext
#endif // VULKAN_HELPERS_H