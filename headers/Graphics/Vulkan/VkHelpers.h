#ifndef VULKAN_HELPERS_H
#define VULKAN_HELPERS_H
//
#include "VkCore.h"   // IWYU pragma: keep
#include "VkObject.h" // IWYU pragma: keep
//
namespace VulkanContext {
// * Validation layers
bool isLayerSupported(const char *layer);
bool hasUnsupportedLayers(const std::vector<const char *> layers);
void findUnsupportedLayers(const std::vector<const char *> layers,
                           std::vector<const char *> unsup);

}; // namespace VulkanContext
#endif // VULKAN_HELPERS_H