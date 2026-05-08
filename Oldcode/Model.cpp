#include "VkInterface.h"

#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

void ModelLoader::LoadModel(const char* Path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, Path)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void ModelLoader::CreateVertexBuffer() {
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // Create staging buffer
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    context->CreateBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data = context->device.mapMemory(stagingBufferMemory, 0, bufferSize);
    std::memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    context->device.unmapMemory(stagingBufferMemory);

    // Create vertex buffer
    context->CreateBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vertexBuffer,
        vertexBufferMemory
    );

    context->CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    context->device.destroyBuffer(stagingBuffer);
    context->device.freeMemory(stagingBufferMemory);
}

void ModelLoader::CreateIndexBuffer() {
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    // Create staging buffer
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    context->CreateBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data = context->device.mapMemory(stagingBufferMemory, 0, bufferSize);
    std::memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    context->device.unmapMemory(stagingBufferMemory);

    // Create index buffer
    context->CreateBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        indexBuffer,
        indexBufferMemory
    );

    context->CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

    context->device.destroyBuffer(stagingBuffer);
    context->device.freeMemory(stagingBufferMemory);
}