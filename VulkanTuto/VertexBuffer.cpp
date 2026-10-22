#include "Header/VertexBuffer.h"

void VertexBuffer::CreateVertexBuffer()
{
    vk::BufferCreateInfo bufferInfo{.size        = sizeof(vertices[0]) * vertices.size(),
                                .usage       = vk::BufferUsageFlagBits::eVertexBuffer,
                                .sharingMode = vk::SharingMode::eExclusive};
    
}

uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags property_flags,Device& device)
{
    vk::PhysicalDeviceMemoryProperties memProperties = device.GetphysiacalDevice()->getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)))
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    
    
}
