#include "VkInterface.h"
#include <iostream>
#include <stdexcept>
#include <vector>

#include "File.h"

// #define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

#ifdef _DEBUG
VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  std::cerr << "[Vulkan Debug] " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}
#endif

void VulkanContext::CreateInstance(void) {
  vk::ApplicationInfo appInfo("Vulkan Application", VK_MAKE_VERSION(1, 0, 0),
                              "No Engine", VK_MAKE_VERSION(1, 0, 0),
                              VK_API_VERSION_1_2);
  std::vector<const char *> layers = {};
#ifdef _DEBUG
  layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
  std::vector<const char *> extensions = {
      VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_get_physical_device_properties2"};
#ifdef WINDOWS
  extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef _DEBUG
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
  vk::InstanceCreateInfo createInfo{{},
                                    &appInfo,
                                    static_cast<uint32_t>(layers.size()),
                                    layers.data(),
                                    static_cast<uint32_t>(extensions.size()),
                                    extensions.data()};
  instance = vk::createInstance(createInfo);
  dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
}

void VulkanContext::SetupDebugMessenger(void) {
#ifdef _DEBUG
  vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo(
      {},
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      debugCallback);
  if (vk::Result result = instance.createDebugUtilsMessengerEXT(
          &debugCreateInfo, nullptr, &debugMessenger, dldi);
      result != vk::Result::eSuccess)
    throw vk::SystemError(result, "Failed to set up debug messenger");
#endif
}

void VulkanContext::CreateSurface(void) {
#ifdef WINDOWS
  vk::Win32SurfaceCreateInfoKHR createInfo{
      {}, window->GetWindowInstance(), window->GetWindowHandle()};
  surface = instance.createWin32SurfaceKHR(createInfo, nullptr, dldi);
#elif LINUX
  // Linux surface creation logic here...
#endif
}

void VulkanContext::PickPhysicalDevice(void) {
  auto devices = instance.enumeratePhysicalDevices(dldi);
  if (devices.empty())
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");
  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      if (msaaSamples != vk::SampleCountFlagBits::e1 &&
          msaaSamples > GetMaxUsableSampleCount())
        throw std::runtime_error(
            "Sample count is bigger than physical device maximum");
      break;
    }
  }
  if (!physicalDevice)
    throw std::runtime_error("Failed to find a suitable GPU!");
}

void VulkanContext::CreateLogicalDevice() {
  QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags{}, queueFamily, 1,
                                  &queuePriority);
  }

  //  Feature setup + validation
  vk::PhysicalDeviceFeatures deviceFeatures{};
  if (anisotropy) {
    vk::PhysicalDeviceFeatures supportedFeatures = physicalDevice.getFeatures();
    if (!supportedFeatures.samplerAnisotropy) {
      throw std::runtime_error(
          "Anisotropy not supported by the physical device!");
    }

    vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
    if (properties.limits.maxSamplerAnisotropy < 1.0f) {
      throw std::runtime_error("Invalid anisotropy limits on physical device!");
    }

    deviceFeatures.samplerAnisotropy = VK_TRUE;
  }

  //  Clean initialization (all fields zeroed)
  vk::DeviceCreateInfo createInfo{};
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  //  CRITICAL: device layers MUST be disabled
  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;

  //  Create logical device
  device = physicalDevice.createDevice(createInfo);

  //  Retrieve queues
  graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
  presentQueue = device.getQueue(indices.presentFamily.value(), 0);
}

void VulkanContext::CreateSwapChain(void) {
  SwapChainSupportDetails swapChainSupport =
      QuerySwapChainSupport(physicalDevice);

  vk::SurfaceFormatKHR surfaceFormat =
      ChooseSwapSurfaceFormat(swapChainSupport.formats);
  vk::PresentModeKHR presentMode =
      ChooseSwapPresentMode(swapChainSupport.presentModes);
  vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount)
    imageCount = swapChainSupport.capabilities.maxImageCount;
  vk::SwapchainCreateInfoKHR createInfo(
      vk::SwapchainCreateFlagsKHR(), surface, imageCount, surfaceFormat.format,
      surfaceFormat.colorSpace, extent,
      1, // imageArrayLayers
      vk::ImageUsageFlagBits::eColorAttachment);
  QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = vk::SharingMode::eExclusive;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

  swapChain = device.createSwapchainKHR(createInfo);

  swapChainImages = device.getSwapchainImagesKHR(swapChain);

  swapChainImageFormat = surfaceFormat.format;
  swapChainExtent = extent;
}

void VulkanContext::CreateImageViews(void) {
  swapChainImageViews.resize(swapChainImages.size());

  for (size_t i = 0; i < swapChainImages.size(); i++) {
    swapChainImageViews[i] =
        CreateImageView(swapChainImages[i], swapChainImageFormat,
                        vk::ImageAspectFlagBits::eColor, 1);
  }
}

void VulkanContext::CreateRenderPass(void) {
  vk::AttachmentDescription colorAttachment{};
  colorAttachment.setFormat(swapChainImageFormat)
      .setSamples(msaaSamples)
      .setLoadOp(vk::AttachmentLoadOp::eClear)
      .setStoreOp(vk::AttachmentStoreOp::eStore)
      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setInitialLayout(vk::ImageLayout::eUndefined);
  if (msaaSamples != vk::SampleCountFlagBits::e1)
    colorAttachment.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
  else
    colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentDescription depthAttachment{};
  depthAttachment.setFormat(FindDepthFormat())
      .setSamples(msaaSamples)
      .setLoadOp(vk::AttachmentLoadOp::eClear)
      .setStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setInitialLayout(vk::ImageLayout::eUndefined)
      .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  vk::AttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = swapChainImageFormat;
  colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
  colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
  colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
  colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
  colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference colorAttachmentRef{};
  colorAttachmentRef.setAttachment(0).setLayout(
      vk::ImageLayout::eColorAttachmentOptimal);

  vk::AttachmentReference depthAttachmentRef{};
  depthAttachmentRef.setAttachment(1).setLayout(
      vk::ImageLayout::eDepthStencilAttachmentOptimal);

  vk::AttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

  vk::SubpassDescription subpass{};
  subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
      .setColorAttachmentCount(1)
      .setPColorAttachments(&colorAttachmentRef)
      .setPDepthStencilAttachment(&depthAttachmentRef);
  if (msaaSamples != vk::SampleCountFlagBits::e1)
    subpass.pResolveAttachments = &colorAttachmentResolveRef;
  else
    subpass.pResolveAttachments = nullptr;

  vk::SubpassDependency dependency{};
  dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
      .setDstSubpass(0)
      .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                       vk::PipelineStageFlagBits::eEarlyFragmentTests)
      .setSrcAccessMask({})
      .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                       vk::PipelineStageFlagBits::eEarlyFragmentTests)
      .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                        vk::AccessFlagBits::eDepthStencilAttachmentWrite);

  std::vector<vk::AttachmentDescription> attachments = {
      colorAttachment, depthAttachment /*,colorAttachmentResolve*/};

  if (msaaSamples != vk::SampleCountFlagBits::e1) {
    attachments.push_back(colorAttachmentResolve);
  }

  vk::RenderPassCreateInfo renderPassInfo{};
  renderPassInfo.setAttachmentCount(static_cast<uint32_t>(attachments.size()))
      .setPAttachments(attachments.data())
      .setSubpassCount(1)
      .setPSubpasses(&subpass)
      .setDependencyCount(1)
      .setPDependencies(&dependency);

  renderPass = device.createRenderPass(renderPassInfo);
}

void VulkanContext::CreateDescriptorSetLayout(void) {
  vk::DescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

  vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 0;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      vk::DescriptorType::eCombinedImageSampler;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

  vk::DescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.bindingCount = 1;

  // Create the uniform descriptor set layout
  layoutInfo.pBindings = &uboLayoutBinding;
  uniformDescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

  // Create the texture descriptor set layout
  layoutInfo.pBindings = &samplerLayoutBinding;
  textureDescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
}

void VulkanContext::CreatePipeline(void) {
  auto vertShaderCode =
      FileUtils::ReadFile("D:/Projects/Vulkan/Shaders/vert.spv");
  auto fragShaderCode =
      FileUtils::ReadFile("D:/Projects/Vulkan/Shaders/frag.spv");

  auto vertShaderModule = CreateShaderModule(vertShaderCode);
  auto fragShaderModule = CreateShaderModule(fragShaderCode);

  vk::PipelineShaderStageCreateInfo shaderStages[] = {
      {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex,
       *vertShaderModule, "main"},
      {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment,
       *fragShaderModule, "main"}};

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;

  auto bindingDescription = Vertex::getBindingDescription();
  auto attributeDescriptions = Vertex::getAttributeDescriptions();

  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  vk::PipelineViewportStateCreateInfo viewportState = {};
  viewportState.viewportCount = 1;
  viewportState.pViewports = nullptr;
  // viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = nullptr;
  // viewportState.pScissors = &scissor;

  vk::PipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = vk::PolygonMode::eFill;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = vk::CullModeFlagBits::eBack;
  // rasterizer.frontFace = vk::FrontFace::eClockwise;
  // rasterizer.cullMode = vk::CullModeFlagBits::eNone;
  rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
  rasterizer.depthBiasEnable = VK_FALSE;

  vk::PipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = msaaSamples;

  vk::PipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = vk::CompareOp::eLess;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask =
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
  colorBlendAttachment.blendEnable = VK_FALSE;

  vk::PipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = vk::LogicOp::eCopy;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  vk::PushConstantRange psRange;
  psRange.offset = 0;
  psRange.size = sizeof(glm::mat4);
  psRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

  std::array<vk::DescriptorSetLayout, 2> SetLayouts = {
      uniformDescriptorSetLayout, textureDescriptorSetLayout};

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(SetLayouts.size());
  ;
  pipelineLayoutInfo.pSetLayouts = SetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &psRange;

  pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

  std::vector<vk::DynamicState> dynamicStates = {
      vk::DynamicState::eViewport,
      vk::DynamicState::eScissor,
  };

  vk::PipelineDynamicStateCreateInfo dynamicStateInfo = {};
  dynamicStateInfo.dynamicStateCount =
      static_cast<uint32_t>(dynamicStates.size());
  dynamicStateInfo.pDynamicStates = dynamicStates.data();

  vk::GraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = nullptr;
  pipelineInfo.pDynamicState = &dynamicStateInfo;

  // auto result = device.createGraphicsPipeline(nullptr, pipelineInfo);
  // graphicsPipeline = result.value;
  graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;
}

void VulkanContext::CreateFramebuffers(void) {
  swapChainFramebuffers.resize(swapChainImageViews.size());
  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    std::vector<vk::ImageView> attachments = {
        msaaSamples != vk::SampleCountFlagBits::e1 ? colorImageView
                                                   : swapChainImageViews[i],
        depthImageView};
    if (msaaSamples != vk::SampleCountFlagBits::e1) {
      attachments.push_back(swapChainImageViews[i]);
    }

    vk::FramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
  }
}

void VulkanContext::CreateCommandPool(void) {
  QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);
  vk::CommandPoolCreateInfo poolInfo = {};
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
  poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  commandPool = device.createCommandPool(poolInfo);
}

void VulkanContext::CreateDepthResources(void) {
  vk::Format depthFormat = FindDepthFormat();

  CreateImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples,
              depthFormat, vk::ImageTiling::eOptimal,
              vk::ImageUsageFlagBits::eDepthStencilAttachment,
              vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage,
              depthImageMemory);
  depthImageView = CreateImageView(depthImage, depthFormat,
                                   vk::ImageAspectFlagBits::eDepth, 1);
}

void VulkanContext::CreateTextureImage(void) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load("Textures/asuka.jpg", &texWidth, &texHeight,
                              &texChannels, STBI_rgb_alpha);
  vk::DeviceSize imageSize = texWidth * texHeight * 4;
  mipLevels = static_cast<uint32_t>(
                  std::floor(std::log2(std::max(texWidth, texHeight)))) +
              1;

  if (!pixels)
    throw std::runtime_error("Failed to load texture image!");

  // Create staging buffer
  vk::Buffer stagingBuffer;
  vk::DeviceMemory stagingBufferMemory;
  CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
               vk::MemoryPropertyFlagBits::eHostVisible |
                   vk::MemoryPropertyFlagBits::eHostCoherent,
               stagingBuffer, stagingBufferMemory);

  // Map memory and copy texture data
  void *data = device.mapMemory(stagingBufferMemory, 0, imageSize);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  device.unmapMemory(stagingBufferMemory);

  stbi_image_free(pixels);

  // Create texture image
  CreateImage(texWidth, texHeight, mipLevels, vk::SampleCountFlagBits::e1,
              vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
              vk::ImageUsageFlagBits::eTransferDst |
                  vk::ImageUsageFlagBits::eSampled,
              vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage,
              textureImageMemory);

  // Transition image layout and copy data
  TransitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb,
                        vk::ImageLayout::eUndefined,
                        vk::ImageLayout::eTransferDstOptimal);

  CopyBufferToImage(stagingBuffer, textureImage,
                    static_cast<uint32_t>(texWidth),
                    static_cast<uint32_t>(texHeight));

  TransitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb,
                        vk::ImageLayout::eTransferDstOptimal,
                        vk::ImageLayout::eShaderReadOnlyOptimal);

  // Cleanup staging buffer
  device.destroyBuffer(stagingBuffer);
  device.freeMemory(stagingBufferMemory);
}

void VulkanContext::CreateTextureImageView(void) {
  textureImageView =
      CreateImageView(textureImage, vk::Format::eR8G8B8A8Srgb,
                      vk::ImageAspectFlagBits::eColor, mipLevels);
}

void VulkanContext::CreateTextureSampler(void) {
  vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

  vk::SamplerCreateInfo samplerInfo{};
  samplerInfo.setMagFilter(vk::Filter::eLinear)
      .setMinFilter(vk::Filter::eLinear)
      .setAddressModeU(vk::SamplerAddressMode::eRepeat)
      .setAddressModeV(vk::SamplerAddressMode::eRepeat)
      .setAddressModeW(vk::SamplerAddressMode::eRepeat)
      .setAnisotropyEnable(anisotropy)
      .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
      .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
      .setUnnormalizedCoordinates(false)
      .setCompareEnable(false)
      .setCompareOp(vk::CompareOp::eAlways)
      .setMipmapMode(vk::SamplerMipmapMode::eLinear);

  // if (anisotropy = true && properties.limits.maxSamplerAnisotropy > 1.0f)
  //     samplerInfo.setMaxAnisotropy(properties.limits.maxSamplerAnisotropy);

  textureSampler = device.createSampler(samplerInfo);
}

void VulkanContext::CreateColorResources(void) {
  vk::Format colorFormat = swapChainImageFormat;

  CreateImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples,
              colorFormat, vk::ImageTiling::eOptimal,
              vk::ImageUsageFlagBits::eTransientAttachment |
                  vk::ImageUsageFlagBits::eColorAttachment,
              vk::MemoryPropertyFlagBits::eDeviceLocal, colorImage,
              colorImageMemory);

  colorImageView = CreateImageView(colorImage, colorFormat,
                                   vk::ImageAspectFlagBits::eColor, 1);
}

void VulkanContext::CreateVertexBuffer() {
  vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  vk::Buffer stagingBuffer;
  vk::DeviceMemory stagingBufferMemory;
  CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
               vk::MemoryPropertyFlagBits::eHostVisible |
                   vk::MemoryPropertyFlagBits::eHostCoherent,
               stagingBuffer, stagingBufferMemory);

  void *data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
  memcpy(data, vertices.data(), (size_t)bufferSize);
  device.unmapMemory(stagingBufferMemory);

  CreateBuffer(bufferSize,
               vk::BufferUsageFlagBits::eTransferDst |
                   vk::BufferUsageFlagBits::eVertexBuffer,
               vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer,
               vertexBufferMemory);

  CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

  device.destroyBuffer(stagingBuffer);
  device.freeMemory(stagingBufferMemory);
}

void VulkanContext::CreateIndexBuffer(void) {
  vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

  vk::Buffer stagingBuffer;
  vk::DeviceMemory stagingBufferMemory;
  CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
               vk::MemoryPropertyFlagBits::eHostVisible |
                   vk::MemoryPropertyFlagBits::eHostCoherent,
               stagingBuffer, stagingBufferMemory);

  void *data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
  memcpy(data, indices.data(), (size_t)bufferSize);
  device.unmapMemory(stagingBufferMemory);

  CreateBuffer(bufferSize,
               vk::BufferUsageFlagBits::eTransferDst |
                   vk::BufferUsageFlagBits::eIndexBuffer,
               vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer,
               indexBufferMemory);

  CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

  device.destroyBuffer(stagingBuffer);
  device.freeMemory(stagingBufferMemory);
}

void VulkanContext::CreateUniformBuffers() {
  vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

  uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                 vk::MemoryPropertyFlagBits::eHostVisible |
                     vk::MemoryPropertyFlagBits::eHostCoherent,
                 uniformBuffers[i], uniformBuffersMemory[i]);
  }
}

void VulkanContext::CreateDescriptorPool(void) {
  std::array<vk::DescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
  poolSizes[0].setDescriptorCount(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
  poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);
  poolSizes[1].setDescriptorCount(3);

  vk::DescriptorPoolCreateInfo poolInfo{};
  poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
      .setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()))
      .setPPoolSizes(poolSizes.data())
      .setMaxSets(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) + 3);

  descriptorPool = device.createDescriptorPool(poolInfo);
}

void VulkanContext::CreateDescriptorSets(void) {
  std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                               uniformDescriptorSetLayout);
  vk::DescriptorSetAllocateInfo allocInfo{};
  allocInfo.setDescriptorPool(descriptorPool)
      .setDescriptorSetCount(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT))
      .setPSetLayouts(layouts.data());

  descriptorSets = device.allocateDescriptorSets(allocInfo);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.setBuffer(uniformBuffers[i])
        .setOffset(0)
        .setRange(sizeof(UniformBufferObject));

    vk::WriteDescriptorSet descriptorWrite{};
    descriptorWrite.setDstSet(descriptorSets[i])
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1)
        .setPBufferInfo(&bufferInfo);

    device.updateDescriptorSets(descriptorWrite, nullptr);
  }
}

void VulkanContext::CreateCommandBuffers() {
  commandBuffers.resize(swapChainFramebuffers.size());

  vk::CommandBufferAllocateInfo allocInfo = {};
  allocInfo.commandPool = commandPool;
  allocInfo.level = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  commandBuffers = device.allocateCommandBuffers(allocInfo);
}

void VulkanContext::CreateSyncObjects() {
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    imageAvailableSemaphores[i] = device.createSemaphore({});
    renderFinishedSemaphores[i] = device.createSemaphore({});
    inFlightFences[i] =
        device.createFence({vk::FenceCreateFlagBits::eSignaled});
  }
}

void VulkanContext::CalculateTime() {
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration<float, std::chrono::seconds::period>(
             currentTime - startTime)
             .count();
}

void VulkanContext::UpdateUniformBuffer(uint32_t currentImage) {
  UniformBufferObject ubo{};
  // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f),
  // glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f),
                              static_cast<float>(swapChainExtent.width) /
                                  static_cast<float>(swapChainExtent.height),
                              0.1f, 10.0f);
  ubo.proj[1][1] *= -1;

  void *data =
      device.mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo));
  std::memcpy(data, &ubo, sizeof(ubo));
  device.unmapMemory(uniformBuffersMemory[currentImage]);
}

void VulkanContext::CleanupSwapChain() {
  device.destroyImageView(colorImageView);
  device.destroyImage(colorImage);
  device.freeMemory(colorImageMemory);
  device.destroyImageView(depthImageView);
  device.destroyImage(depthImage);
  device.freeMemory(depthImageMemory);
  for (auto framebuffer : swapChainFramebuffers)
    device.destroyFramebuffer(framebuffer);
  device.freeCommandBuffers(commandPool, commandBuffers);
  device.destroyPipeline(graphicsPipeline);
  device.destroyPipelineLayout(pipelineLayout);
  device.destroyRenderPass(renderPass);
  for (auto imageView : swapChainImageViews)
    device.destroyImageView(imageView);
  device.destroySwapchainKHR(swapChain);
}

void VulkanContext::RecordCommandBuffer(vk::CommandBuffer &commandBuffer,
                                        uint32_t imageIndex) {
  // Begin recording the command buffer
  vk::CommandBufferBeginInfo beginInfo{};
  beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
  commandBuffer.begin(
      beginInfo); // No explicit result check required in Vulkan.hpp

  // Set up the render pass begin info
  vk::RenderPassBeginInfo renderPassInfo{};
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
  renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
  renderPassInfo.renderArea.extent = swapChainExtent;

  // Define a clear color value (e.g., black with no transparency)
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  ;
  renderPassInfo.pClearValues = clearValues.data();

  // Begin the render pass
  commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

  // Bind the graphics pipeline
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             graphicsPipeline);

  // Set the viewport dynamically
  vk::Viewport viewport{
      0.0f,
      0.0f,                                       // x, y
      static_cast<float>(swapChainExtent.width),  // width
      static_cast<float>(swapChainExtent.height), // height
      0.0f,
      1.0f // minDepth, maxDepth
  };
  commandBuffer.setViewport(0, 1, &viewport);

  // Set the scissor dynamically
  vk::Rect2D scissor{
      vk::Offset2D{0, 0}, // Offset
      swapChainExtent     // Extent matching the swapchain
  };
  commandBuffer.setScissor(0, 1, &scissor);

  commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   pipelineLayout, 0,
                                   {descriptorSets[currentFrame]}, {});
  commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   pipelineLayout, 1,
                                   {textureLoader->descriptorSet}, {});

  glm::mat4 transform;
  transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));
  // transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f,
  // 0.0f, 0.0f)); transform = glm::rotate(transform, glm::radians(90.0f),
  // glm::vec3(1.0f, 0.0f, 0.0f));
  transform = glm::scale(transform, glm::vec3(1.0f));

  // TAGS: ROTATION, ROTATE, SPIN, TRANSLATE
  // Make the model spin
  transform = glm::rotate(transform, time * glm::radians(45.0f),
                          glm::vec3(1.0f, 1.0f, 1.0f));
  //

  commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex,
                              0, sizeof(glm::mat4), &transform);
  modelLoader->Render(commandBuffer);

  commandBuffer.endRenderPass();
  commandBuffer.end();
}

void VulkanContext::DrawFrame(void) {
  // Wait for the previous frame to finish
  if (device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE,
                           std::numeric_limits<uint64_t>::max()) !=
      vk::Result::eSuccess) {
    throw std::runtime_error("Failed to wait for fences");
  }

  // Acquire the next image from the swap chain
  uint32_t imageIndex;
  try {
    vk::ResultValue result = device.acquireNextImageKHR(
        swapChain, std::numeric_limits<uint64_t>::max(),
        imageAvailableSemaphores[currentFrame], nullptr);
    imageIndex = result.value;
  } catch (vk::OutOfDateKHRError &) {
    RecreateSwapChain();
    return;
  } catch (vk::SystemError &) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  CalculateTime();
  UpdateUniformBuffer(currentFrame);

  // Reset the fence to unsignaled state for the current frame
  if (device.resetFences(1, &inFlightFences[currentFrame]) !=
      vk::Result::eSuccess) {
    throw std::runtime_error("Failed to reset fences");
  }

  // Record command buffer for this frame
  commandBuffers[imageIndex].reset({}); // Reset the command buffer
  RecordCommandBuffer(commandBuffers[imageIndex], imageIndex);

  // Submit the command buffer
  vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  vk::PipelineStageFlags waitStages[] = {
      vk::PipelineStageFlagBits::eColorAttachmentOutput};

  vk::SubmitInfo submitInfo =
      vk::SubmitInfo()
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(waitSemaphores)
          .setPWaitDstStageMask(waitStages)
          .setCommandBufferCount(1)
          .setPCommandBuffers(&commandBuffers[imageIndex])
          .setSignalSemaphoreCount(1)
          .setPSignalSemaphores(&renderFinishedSemaphores[currentFrame]);

  try {
    graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
  } catch (vk::SystemError &) {
    throw std::runtime_error("Failed to submit draw command buffer!");
  }

  // Present the swap chain image
  vk::SwapchainKHR swapChains[] = {swapChain};

  vk::PresentInfoKHR presentInfo =
      vk::PresentInfoKHR()
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(&renderFinishedSemaphores[currentFrame])
          .setSwapchainCount(1)
          .setPSwapchains(swapChains)
          .setPImageIndices(&imageIndex);

  try {
    vk::Result result = presentQueue.presentKHR(presentInfo);
    if (result == vk::Result::eSuboptimalKHR || framebufferResized) {
      framebufferResized = false;
      RecreateSwapChain();
    }
  } catch (vk::OutOfDateKHRError &) {
    RecreateSwapChain();
  } catch (vk::SystemError &) {
    throw std::runtime_error("Failed to present swap chain image!");
  }

  // Advance to the next frame
  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

int VulkanContext::Initialize(void) {
  try {
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreatePipeline();
    CreateColorResources();
    CreateDepthResources();
    CreateFramebuffers();
    CreateCommandPool();
    // CreateTextureImage();
    // CreateTextureImageView();
    // CreateTextureSampler();
    // CreateVertexBuffer();
    // CreateIndexBuffer();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateCommandBuffers();
    CreateSyncObjects();
  } catch (vk::SystemError &err) {
    std::cout << "vk::SystemError: " << err.what() << std::endl;
    return -1;
  } catch (std::exception &err) {
    std::cout << "std::exception: " << err.what() << std::endl;
    return -1;
  }
  return 0;
}

VulkanContext::~VulkanContext() {
  device.waitIdle();

  modelLoader->Destroy();
  textureLoader->Destroy();

  device.destroyImageView(colorImageView);
  device.destroyImage(colorImage);
  device.freeMemory(colorImageMemory);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    device.destroySemaphore(renderFinishedSemaphores[i]);
    device.destroySemaphore(imageAvailableSemaphores[i]);
    device.destroyFence(inFlightFences[i]);
  }
  device.destroyImageView(depthImageView);
  device.destroyImage(depthImage);
  device.freeMemory(depthImageMemory);
  device.freeCommandBuffers(commandPool, commandBuffers);
  device.destroyDescriptorSetLayout(uniformDescriptorSetLayout);
  device.destroyDescriptorSetLayout(textureDescriptorSetLayout);

  // device.destroyImage(textureImage);
  // device.freeMemory(textureImageMemory);
  // device.destroyImageView(textureImageView);
  // device.destroySampler(textureSampler);
  // device.destroyBuffer(indexBuffer);
  // device.freeMemory(indexBufferMemory);
  // device.destroyBuffer(vertexBuffer);
  // device.freeMemory(vertexBufferMemory);

  device.destroyCommandPool(commandPool);
  for (auto framebuffer : swapChainFramebuffers)
    device.destroyFramebuffer(framebuffer);
  device.destroyPipeline(graphicsPipeline);
  device.destroyPipelineLayout(pipelineLayout);
  device.destroyRenderPass(renderPass);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroyBuffer(device, uniformBuffers[i], nullptr);
    vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
  }
  device.freeDescriptorSets(descriptorPool, descriptorSets);
  device.destroyDescriptorPool(descriptorPool);
  for (auto imageView : swapChainImageViews)
    device.destroyImageView(imageView);
  device.destroySwapchainKHR(swapChain);
  device.destroy();
  instance.destroySurfaceKHR(surface);
  instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
  instance.destroy();
}