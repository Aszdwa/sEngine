#include "VkHelpers.h"
//
namespace VulkanContext {
//
namespace VkHelpers {
//
bool isLayerSupported(const char *layer) {
  const auto availableLayers = vk::enumerateInstanceLayerProperties();

  return std::ranges::any_of(
      availableLayers, [layer](const vk::LayerProperties &availableLayer) {
        return std::string_view(availableLayer.layerName) == layer;
      });
}

bool hasUnsupportedLayers(const std::vector<const char *> layers) {
  return std::ranges::any_of(
      layers, [](const char *layer) { return !isLayerSupported(layer); });
}

bool findUnsupportedLayers(const std::vector<const char *> layers,
                           std::vector<const char *> *unsup) {
  if (!unsup)
    return 1;
  unsup->clear();
  for (const char *layer : layers) {
    if (isLayerSupported(layer) == false) {
      unsup->push_back(layer);
    }
  }
  return 0;
}

bool isExtensionSupported(const char *extension) {
  const auto availableExtensions = vk::enumerateInstanceExtensionProperties();

  return std::ranges::any_of(
      availableExtensions,
      [extension](const vk::ExtensionProperties &available) {
        return std::string_view(available.extensionName) == extension;
      });
}

bool hasUnsupportedExtensions(const std::vector<const char *> extensions) {
  return std::ranges::any_of(extensions, [](const char *extension) {
    return !isExtensionSupported(extension);
  });
}

bool findUnsupportedExtensions(const std::vector<const char *> extensions,
                               std::vector<const char *> *unsup) {

  if (!unsup) {
    return false;
  }

  unsup->clear();

  for (const char *extension : extensions) {
    if (!isExtensionSupported(extension)) {
      unsup->push_back(extension);
    }
  }

  return !unsup->empty();
}

}; // namespace VkHelpers
}; // namespace VulkanContext