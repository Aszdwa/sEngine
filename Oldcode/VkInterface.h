#pragma once
#include "Defines.h"
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif // COMPILER SPECIFIC
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
// #define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#endif
#include "Window.h"
// #define VK_NO_PROTOTYPES
// #define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
// VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
// Default headers
#include <array>
#include <chrono>
#include <optional>
#include <set>
#include <string>

// Forward declarations
class ModelLoader;
class TextureImage;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static vk::VertexInputBindingDescription getBindingDescription() {
    return vk::VertexInputBindingDescription()
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
  }

  static std::array<vk::VertexInputAttributeDescription, 3>
  getAttributeDescriptions() {
    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0]
        .setBinding(0)
        .setLocation(0)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, pos));

    attributeDescriptions[1]
        .setBinding(0)
        .setLocation(1)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, color));

    attributeDescriptions[2]
        .setBinding(0)
        .setLocation(2)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(Vertex, texCoord));

    return attributeDescriptions;
  }

  bool operator==(const Vertex &other) const {
    return pos == other.pos && color == other.color &&
           texCoord == other.texCoord;
  }
};

namespace std {
template <> struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    return ((hash<glm::vec3>()(vertex.pos) ^
             (hash<glm::vec3>()(vertex.color) << 1)) >>
            1) ^
           (hash<glm::vec2>()(vertex.texCoord) << 1);
  }
};
} // namespace std

class VulkanContext {
private:
  // Window related
  Window *window;
  // Vulkan variables
  vk::Instance instance;
  vk::DispatchLoaderDynamic dldi;
  vk::DebugUtilsMessengerEXT debugMessenger;
  vk::SurfaceKHR surface;
  vk::PhysicalDevice physicalDevice;
  vk::Device device;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;
  vk::SwapchainKHR swapChain;
  std::vector<vk::Image> swapChainImages;
  vk::Format swapChainImageFormat;
  vk::Extent2D swapChainExtent;
  std::vector<vk::ImageView> swapChainImageViews;
  std::vector<vk::Framebuffer> swapChainFramebuffers;
  vk::RenderPass renderPass;
  vk::PipelineLayout pipelineLayout;
  vk::Pipeline graphicsPipeline;
  vk::CommandPool commandPool;

  uint32_t mipLevels;
  vk::Image textureImage;
  vk::DeviceMemory textureImageMemory;
  vk::ImageView textureImageView;
  vk::Sampler textureSampler;
  vk::Buffer vertexBuffer;
  vk::DeviceMemory vertexBufferMemory;
  vk::Buffer indexBuffer;
  vk::DeviceMemory indexBufferMemory;

  vk::Image depthImage;
  vk::DeviceMemory depthImageMemory;
  vk::ImageView depthImageView;

  float time = 0.0f;
  std::vector<vk::Buffer> uniformBuffers;
  std::vector<vk::DeviceMemory> uniformBuffersMemory;
  // std::vector<void*> uniformBuffersMapped;

  bool anisotropy = true;
  vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e16;
  vk::Image colorImage;
  vk::DeviceMemory colorImageMemory;
  vk::ImageView colorImageView;

  vk::DescriptorPool descriptorPool;
  std::vector<vk::DescriptorSet> descriptorSets;
  std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>
      commandBuffers;
  std::vector<vk::Semaphore> imageAvailableSemaphores;
  std::vector<vk::Semaphore> renderFinishedSemaphores;
  std::vector<vk::Fence> inFlightFences;

  std::array<vk::ClearValue, 2> clearValues = {
      {vk::ClearValue(
           vk::ClearColorValue(std::array<float, 4>{0.0f, 0.7f, 0.25f, 1.0f})),
       vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0))}};

  const int MAX_FRAMES_IN_FLIGHT = 2;
  uint32_t currentFrame = 0;
  bool framebufferResized = false;
  // Second phase variables

  // vk::DescriptorSetLayout descriptorSetLayout;

  vk::DescriptorSetLayout textureDescriptorSetLayout;
  vk::DescriptorSetLayout uniformDescriptorSetLayout;

  // Classes pointers
  ModelLoader *modelLoader;
  TextureImage *textureLoader;

  // Private function prototypes
  void CreateInstance(void);
  void SetupDebugMessenger(void);
  void CreateSurface(void);
  void PickPhysicalDevice(void);
  void CreateLogicalDevice(void);
  void CreateSwapChain(void);
  void CreateImageViews(void);
  void CreateRenderPass(void);
  void CreateDescriptorSetLayout(void);
  void CreatePipeline(void);
  void CreateFramebuffers(void);
  void CreateCommandPool(void);
  void CreateDepthResources(void);
  void CreateColorResources(void);
  void CreateTextureImage(void);
  void CreateTextureImageView(void);
  void CreateTextureSampler(void);
  void CreateVertexBuffer(void);
  void CreateIndexBuffer(void);
  void CreateUniformBuffers(void);
  void CreateDescriptorPool(void);
  void CreateDescriptorSets(void);
  void CreateCommandBuffers(void);
  void CreateSyncObjects(void);
  void CleanupSwapChain(void);
  void RecordCommandBuffer(vk::CommandBuffer &commandBuffer,
                           uint32_t imageIndex);
  // Friend classes
  friend class TextureImage;
  friend class ModelLoader;

public:
  // Function calls
  int Initialize(void);
  void SetWindowPtr(Window *window);
  void SetModelPtr(ModelLoader *model);
  void SetTexturePtr(TextureImage *texture);
  void DrawFrame(void);
  bool GetFbResize(void) const { return framebufferResized; };
  VulkanContext() = default;
  ~VulkanContext();

protected:
  // Helper functions
  bool isDeviceSuitable(const vk::PhysicalDevice &device);
  QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
  bool CheckDeviceExtensionSupport(const vk::PhysicalDevice &device);
  SwapChainSupportDetails
  QuerySwapChainSupport(const vk::PhysicalDevice &device);
  vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);
  vk::PresentModeKHR ChooseSwapPresentMode(
      const std::vector<vk::PresentModeKHR> availablePresentModes);
  vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR> &availableFormats);
  vk::UniqueShaderModule CreateShaderModule(const std::vector<char> &code);
  void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                    vk::MemoryPropertyFlags properties, vk::Buffer &buffer,
                    vk::DeviceMemory &bufferMemory);
  uint32_t FindMemoryType(uint32_t typeFilter,
                          vk::MemoryPropertyFlags properties);
  void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void RecreateSwapChain(void);
  void CalculateTime(void);
  void UpdateUniformBuffer(uint32_t currentImage);
  void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                   vk::SampleCountFlagBits numSamples, vk::Format format,
                   vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                   vk::MemoryPropertyFlags properties, vk::Image &image,
                   vk::DeviceMemory &imageMemory);
  void TransitionImageLayout(vk::Image image, vk::Format format,
                             vk::ImageLayout oldLayout,
                             vk::ImageLayout newLayout);
  void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width,
                         uint32_t height);
  vk::ImageView CreateImageView(vk::Image image, vk::Format format,
                                vk::ImageAspectFlags aspectFlags,
                                uint32_t mipLevels);
  vk::CommandBuffer BeginSingleTimeCommands();
  void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
  vk::Format FindDepthFormat(void);
  vk::Format FindSupportedFormat(const std::vector<vk::Format> &candidates,
                                 vk::ImageTiling tiling,
                                 vk::FormatFeatureFlags features);
  bool hasStencilComponent(vk::Format format);
  vk::SampleCountFlagBits GetMaxUsableSampleCount(void);

  // Extensions and layers vectors
  std::vector<const char *> validationLayers =
      { // Change later : Pass as a reference
          "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> deviceExtensions =
      { // Change later : Pass as a reference
          VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

class ModelLoader {
public:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  vk::Buffer vertexBuffer;
  vk::DeviceMemory vertexBufferMemory;
  vk::Buffer indexBuffer;
  vk::DeviceMemory indexBufferMemory;

  VulkanContext *context;

  void Load(const char *Path) {
    LoadModel(Path);
    CreateVertexBuffer();
    CreateIndexBuffer();
  }

  void Destroy() {
    // context->device.waitIdle();

    context->device.destroyBuffer(indexBuffer);
    context->device.freeMemory(indexBufferMemory);
    context->device.destroyBuffer(vertexBuffer);
    context->device.freeMemory(vertexBufferMemory);
  }

  void Render(vk::CommandBuffer commandBuffer) {
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, {vertexBuffer}, offsets);
    commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
    commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0,
                              0);
  }

  /*~ModelLoader() { Destroy(); }*/

private:
  void LoadModel(const char *Path);
  void CreateVertexBuffer();
  void CreateIndexBuffer();

  friend class VulkanContext;
};

class TextureImage {
public:
  vk::DescriptorSet descriptorSet;
  VulkanContext *context;

  void Load(const char *path) {
    CreateTextureImage(path);
    CreateTextureImageView();
    CreateTextureSampler();
    CreateTextureDescriptorSet();
  }

  void Free() {
    context->device.freeDescriptorSets(context->descriptorPool, descriptorSet);
  }

  void Destroy() {
    // context->device.waitIdle();

    context->device.destroy(textureSampler);
    context->device.destroy(textureImageView);
    context->device.destroy(textureImage);
    context->device.freeMemory(textureImageMemory);
  }

  /*~TextureImage() { Destroy(); }*/

private:
  uint32_t mipLevels;
  vk::Image textureImage;
  vk::DeviceMemory textureImageMemory;
  vk::ImageView textureImageView;
  vk::Sampler textureSampler;

  void CreateTextureImage(const char *path);
  void CreateTextureImageView();
  void CreateTextureSampler();
  void CreateTextureDescriptorSet();

  friend class VulkanContext;
};