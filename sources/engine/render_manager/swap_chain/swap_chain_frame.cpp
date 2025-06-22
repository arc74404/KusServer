#include "swap_chain_frame.hpp"

#include <iostream>

#include "swap_chain.hpp"
#include "synchronization_control.hpp"

namespace kusengine
{

namespace render
{
// void
// SwapChainFrame::fillDescriptorSets(std::vector<vk::DescriptorSet>& d_sets)
// const
// {
//     for (int i = 0; i < m_descriptor_sets.size(); ++i)
//     {
//         d_sets.emplace_back(m_descriptor_sets[i].get());
//     }
// }

const std::vector<vk::DescriptorSet>&
SwapChainFrame::descriptorSets() const noexcept
{
    return m_descriptor_sets;
}

const SynchronizationControl&
SwapChainFrame::synControl() const noexcept
{
    return m_sync_control;
}

const vk::CommandBuffer&
SwapChainFrame::commandBuffer() const noexcept
{
    return m_command_buffer.commandBuffer();
}

const vk::Framebuffer&
SwapChainFrame::framebuffer() const noexcept
{
    return m_framebuffer.get();
}

void
SwapChainFrame::createImage(const vk::Image& image, const vk::Format& format)
{
    vk::ImageViewCreateInfo create_image_view_info = {};
    create_image_view_info.image                   = image;
    create_image_view_info.viewType                = vk::ImageViewType::e2D;
    create_image_view_info.format                  = format;
    create_image_view_info.components.r = vk::ComponentSwizzle::eIdentity;
    create_image_view_info.components.g = vk::ComponentSwizzle::eIdentity;
    create_image_view_info.components.b = vk::ComponentSwizzle::eIdentity;
    create_image_view_info.components.a = vk::ComponentSwizzle::eIdentity;
    create_image_view_info.subresourceRange.aspectMask =
        vk::ImageAspectFlagBits::eColor;
    create_image_view_info.subresourceRange.baseMipLevel   = 0;
    create_image_view_info.subresourceRange.levelCount     = 1;
    create_image_view_info.subresourceRange.baseArrayLayer = 0;
    create_image_view_info.subresourceRange.layerCount     = 1;

    m_view =
        LOGICAL_DEVICE_INSTANCE.createImageViewUnique(create_image_view_info);
}

void
SwapChainFrame::createFrameBuffer(const vk::RenderPass& renderpass,
                                  const vk::Extent2D& extent)
{
    std::vector<vk::ImageView> attachments = {m_view.get()};

    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.flags           = vk::FramebufferCreateFlags();
    framebufferInfo.renderPass      = renderpass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments    = attachments.data();
    framebufferInfo.width           = extent.width;
    framebufferInfo.height          = extent.height;
    framebufferInfo.layers          = 1;

    m_framebuffer =
        LOGICAL_DEVICE_INSTANCE.createFramebufferUnique(framebufferInfo);
}

void
SwapChainFrame::writeDescriptorSetUBO()
{
    vk::DescriptorBufferInfo ubo_buffer_info{};
    ubo_buffer_info.buffer = m_uniform_buffer.buffer();
    ubo_buffer_info.offset = 0;
    ubo_buffer_info.range  = m_uniform_buffer.byteSize();

    vk::WriteDescriptorSet descriptor_write;
    descriptor_write.dstSet          = m_descriptor_sets[0];
    descriptor_write.dstBinding      = 0;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType  = vk::DescriptorType::eUniformBuffer;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo     = &ubo_buffer_info;

    LOGICAL_DEVICE_INSTANCE.updateDescriptorSets(1, &descriptor_write, 0,
                                                 nullptr);
}

void
SwapChainFrame::writeDescriptorSetMBDD()
{
    vk::DescriptorBufferInfo dob_buffer_info{};
    dob_buffer_info.buffer = m_storage_buffer.buffer();
    dob_buffer_info.offset = 0;
    dob_buffer_info.range  = m_storage_buffer.byteSize();

    vk::WriteDescriptorSet descriptor_write;
    descriptor_write.dstSet          = m_descriptor_sets[0];
    descriptor_write.dstBinding      = 1;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType  = vk::DescriptorType::eStorageBuffer;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo     = &dob_buffer_info;

    LOGICAL_DEVICE_INSTANCE.updateDescriptorSets(1, &descriptor_write, 0,
                                                 nullptr);
}

void
SwapChainFrame::createDescriptorSet(const DescriptorManager& descriptor_manager)
{

    vk::DescriptorSetAllocateInfo allocationInfo;

    allocationInfo.descriptorPool =
        descriptor_manager.descriptorConstructs()[0].pool.descriptorPool();
    allocationInfo.descriptorSetCount = 1;
    allocationInfo.pSetLayouts =
        &(descriptor_manager.descriptorSetLayoutVector()[0]);

    m_descriptor_sets =
        LOGICAL_DEVICE_INSTANCE.allocateDescriptorSets(allocationInfo);
}

void
SwapChainFrame::createCommandBuffer()
{
    m_command_buffer.create();
}

void
SwapChainFrame::createSynchronization()
{
    m_sync_control.create();
}

void
SwapChainFrame::waitForFence()
{
    LOGICAL_DEVICE_INSTANCE.waitForFences(1, &(m_sync_control.inFlightFence()),
                                          VK_TRUE, UINT64_MAX);

    LOGICAL_DEVICE_INSTANCE.resetFences(1, &(m_sync_control.inFlightFence()));
}

void
SwapChainFrame::submitCommandBuffer()
{
    vk::SubmitInfo submitInfo = {};

    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores    = m_sync_control.waitSemaphores();
    submitInfo.pWaitDstStageMask  = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &(m_command_buffer.commandBuffer());

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = m_sync_control.signalSemaphores();

    DEVICE_INSTANCE.getQueue("graphics")
        .submit(submitInfo, m_sync_control.inFlightFence());
}
}; // namespace render
}; // namespace kusengine
