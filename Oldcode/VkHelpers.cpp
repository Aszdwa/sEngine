#include "VkInterface.h"

void VulkanContext::SetWindowPtr(Window* window) {
	this->window = window;
}
void VulkanContext::SetModelPtr(ModelLoader* model) {
    this->modelLoader = model;
}
void VulkanContext::SetTexturePtr(TextureImage* texture) {
    this->textureLoader = texture;
}

bool VulkanContext::isDeviceSuitable(const vk::PhysicalDevice& device) {
    QueueFamilyIndices indices = FindQueueFamilies(device);
    bool extensionsSupported = CheckDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

QueueFamilyIndices VulkanContext::FindQueueFamilies(vk::PhysicalDevice device) {
    QueueFamilyIndices indices;
    auto queueFamilies = device.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;
        if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, surface)) 
            indices.presentFamily = i;
        if (indices.isComplete())
            break;
        i++;
    }
    return indices;
}

bool VulkanContext::CheckDeviceExtensionSupport(const vk::PhysicalDevice& device) {
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : device.enumerateDeviceExtensionProperties())
        requiredExtensions.erase(extension.extensionName);
    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanContext::QuerySwapChainSupport(const vk::PhysicalDevice& device) {
    SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);
    return details;
}

vk::SurfaceFormatKHR VulkanContext::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return availableFormat;
    }
    return availableFormats[0];
}

vk::PresentModeKHR VulkanContext::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) {
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            return availablePresentMode;
        else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
            bestMode = availablePresentMode;
    }
    return bestMode;
}

vk::Extent2D VulkanContext::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    else {
        //int width = window->GetWidth(), height = window->GetHeight();
        //vk::Extent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        vk::Extent2D actualExtent = { static_cast<uint32_t>(window->GetWidth() ), static_cast<uint32_t>(window->GetHeight() ) };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }
}

vk::UniqueShaderModule VulkanContext::CreateShaderModule(const std::vector<char>& code) {
    return device.createShaderModuleUnique({
        vk::ShaderModuleCreateFlags(),
        code.size(),
        reinterpret_cast<const uint32_t*>(code.data())
        });
}

void VulkanContext::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer = device.createBuffer(bufferInfo);

    vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = device.allocateMemory(allocInfo);

    device.bindBufferMemory(buffer, bufferMemory, 0);
}

uint32_t VulkanContext::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanContext::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();
    vk::BufferCopy copyRegion{};
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
    EndSingleTimeCommands(commandBuffer);
}

void VulkanContext::RecreateSwapChain(void) {
    if (window->GetWidth() == 0 || window->GetHeight() == 0) {
        window->WindowEvents();
        return;
    }

    device.waitIdle();

    CleanupSwapChain();

    CreateSwapChain();
    CreateImageViews();
    CreateColorResources();
    CreateDepthResources();
    CreateRenderPass();
    CreatePipeline();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void VulkanContext::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format,
    vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory) {

    vk::ImageCreateInfo imageInfo{};
    imageInfo.setImageType(vk::ImageType::e2D)
        .setExtent({ width, height, 1 })
        .setMipLevels(1)
        .setArrayLayers(1)
        .setFormat(format)
        .setTiling(tiling)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(usage)
        .setSamples(numSamples)
        .setSharingMode(vk::SharingMode::eExclusive);

    image = device.createImage(imageInfo);

    vk::MemoryRequirements memRequirements = device.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memRequirements.size)
        .setMemoryTypeIndex(FindMemoryType(memRequirements.memoryTypeBits, properties));

    imageMemory = device.allocateMemory(allocInfo);

    device.bindImageMemory(image, imageMemory, 0);
}

void VulkanContext::TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier{};
    barrier.setOldLayout(oldLayout)
        .setNewLayout(newLayout)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(image)
        .setSubresourceRange({
            vk::ImageAspectFlagBits::eColor,  // aspectMask
            0,                               // baseMipLevel
            1,                               // levelCount
            0,                               // baseArrayLayer
            1                                // layerCount
            });

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.setSrcAccessMask({})
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(
        sourceStage, destinationStage,
        {},                // dependencyFlags
        nullptr,           // memoryBarriers
        nullptr,           // bufferMemoryBarriers
        barrier            // imageMemoryBarriers
    );

    EndSingleTimeCommands(commandBuffer);
}

void VulkanContext::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

    vk::BufferImageCopy region{};
    region.setBufferOffset(0)
        .setBufferRowLength(0)
        .setBufferImageHeight(0)
        .setImageSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 })
        .setImageOffset({ 0, 0, 0 })
        .setImageExtent({ width, height, 1 });

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, { region });

    EndSingleTimeCommands(commandBuffer);
}

vk::CommandBuffer VulkanContext::BeginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo).front();

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void VulkanContext::EndSingleTimeCommands(vk::CommandBuffer commandBuffer) {
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    graphicsQueue.submit(submitInfo, nullptr);
    graphicsQueue.waitIdle();

    device.freeCommandBuffers(commandPool, commandBuffer);
}

vk::ImageView VulkanContext::CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipLevels) {
    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.setImage(image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format)
        .setSubresourceRange({
            aspectFlags,
            0, // baseMipLevel
            1, // levelCount
            0, // baseArrayLayer
            1  // layerCount
            });

    vk::ImageView imageView;

    imageView = device.createImageView(viewInfo);

    return imageView;
}

vk::Format VulkanContext::FindDepthFormat() {
    return FindSupportedFormat(
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

vk::Format VulkanContext::FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props = physicalDevice.getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            return format;
    }
    throw std::runtime_error("Failed to find supported format!");
}

bool VulkanContext::hasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

vk::SampleCountFlagBits VulkanContext::GetMaxUsableSampleCount(void) {
    vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();

    vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
        physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
    if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
    if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
    if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
    if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
    if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

    return vk::SampleCountFlagBits::e1;
}