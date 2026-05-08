#include "VkInterface.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void TextureImage::CreateTextureImage(const char* path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    if (!pixels)
        throw std::runtime_error("Failed to load texture image!");

    // Create staging buffer
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    context->CreateBuffer(imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);

    // Map memory and copy texture data
    void* data = context->device.mapMemory(stagingBufferMemory, 0, imageSize);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    context->device.unmapMemory(stagingBufferMemory);

    stbi_image_free(pixels);

    // Create texture image
    context->CreateImage(texWidth,
        texHeight,
        mipLevels,
        vk::SampleCountFlagBits::e1,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        textureImage,
        textureImageMemory);

    // Transition image layout and copy data
    context->TransitionImageLayout(textureImage,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal);

    context->CopyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    context->TransitionImageLayout(textureImage,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal);

    // Cleanup staging buffer
    context->device.destroyBuffer(stagingBuffer);
    context->device.freeMemory(stagingBufferMemory);
}

void TextureImage::CreateTextureImageView(void) {
    textureImageView = context->CreateImageView(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, mipLevels);
}

void TextureImage::CreateTextureSampler(void) {
    vk::PhysicalDeviceProperties properties = context->physicalDevice.getProperties();

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setAddressModeU(vk::SamplerAddressMode::eRepeat)
        .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        .setAnisotropyEnable(context->anisotropy)
        .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(false)
        .setCompareEnable(false)
        .setCompareOp(vk::CompareOp::eAlways)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear);

    //if (anisotropy = true && properties.limits.maxSamplerAnisotropy > 1.0f)
    //    samplerInfo.setMaxAnisotropy(properties.limits.maxSamplerAnisotropy);

    textureSampler = context->device.createSampler(samplerInfo);
}

void TextureImage::CreateTextureDescriptorSet() {
    vk::DescriptorSetAllocateInfo allocInfo = {};
    allocInfo.setDescriptorPool(context->descriptorPool);
    allocInfo.setDescriptorSetCount(1);
    allocInfo.setPSetLayouts(&context->textureDescriptorSetLayout);

    descriptorSet = context->device.allocateDescriptorSets(allocInfo).front();

    vk::DescriptorImageInfo imageInfo = {};
    imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    imageInfo.setImageView(textureImageView);
    imageInfo.setSampler(textureSampler);

    vk::WriteDescriptorSet descriptorWrite = {};
    descriptorWrite.setDstSet(descriptorSet);
    descriptorWrite.setDstBinding(0);
    descriptorWrite.setDstArrayElement(0);
    descriptorWrite.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    descriptorWrite.setDescriptorCount(1);
    descriptorWrite.setPImageInfo(&imageInfo);

    context->device.updateDescriptorSets(descriptorWrite, nullptr);
}