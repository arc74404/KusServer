#include "swap_chain.hpp"

#include <iostream>

#include "engine/render_manager/device/device.hpp"
#include "engine/render_manager/instance/instance.hpp"
#include "engine/render_manager/render_pass/render_pass.hpp"
#include "engine/window/window.hpp"

namespace kusengine
{

namespace render
{
const vk::SurfaceKHR&
SwapChain::surface() const noexcept
{
    return m_surface.get();
}

vk::Extent2D
SwapChain::extent() const noexcept
{
    return m_extent;
}

vk::Format
SwapChain::format() const noexcept
{
    return m_format;
}

const vk::SwapchainKHR&
SwapChain::swapchain() const noexcept
{
    return m_swapchain.get();
}

void
SwapChain::createSurface(const kusengine::Window& window,
                         const Instance& instance) noexcept(false)
{
    VkSurfaceKHR c_style_surface{};

    if (!window.createWindowSurface(instance.get(), c_style_surface))
    {
        throw std::exception("Failed to create window surface\n");
    }

    m_surface.get() = c_style_surface;
}

vk::SurfaceFormatKHR
SwapChain::chooseSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& available_formats) const
{
    for (vk::SurfaceFormatKHR format : available_formats)
    {
        if (format.format == vk::Format::eB8G8R8A8Unorm &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return format;
        }
    }

    return available_formats[0];
}

vk::PresentModeKHR
SwapChain::choosePresentMode(
    const std::vector<vk::PresentModeKHR>& available_present_modes) const
{
    for (vk::PresentModeKHR presentMode : available_present_modes)
    {
        // if (presentMode == vk::PresentModeKHR::eImmediate)
        // {
        //     return presentMode;
        // }
        if (presentMode == vk::PresentModeKHR::eMailbox)
        {
            return presentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D
SwapChain::chooseExtent(uint32_t width,
                        uint32_t height,
                        vk::SurfaceCapabilitiesKHR capabilities) const
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        vk::Extent2D extent = {width, height};

        extent.width =
            std::min(capabilities.maxImageExtent.width,
                     std::max(capabilities.minImageExtent.width, extent.width));

        extent.height = std::min(
            capabilities.maxImageExtent.height,
            std::max(capabilities.minImageExtent.height, extent.height));

        return extent;
    }
}

uint32_t
SwapChain::chooseImageCount(
    const vk::PresentModeKHR& present_mode,
    const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept
{
    uint32_t res = 2;
    if (present_mode == vk::PresentModeKHR::eMailbox) res = 3;
    return std::min(capabilities.maxImageCount, res);
}

// bool
// SwapChain::recreate(const Window& window, const Instance& instance)
// {
//     // createSurface(window, instance);
//     // if (!create(window.getExtent().width, window.getExtent().height))
//     //     return false;
//     // createSwapChainFrames();
//     return true;
// }

void
SwapChain::create(uint32_t width, uint32_t height)
{
    auto support = DEVICE_INSTANCE.getSurfaceSupportDetails(surface());

    vk::SurfaceFormatKHR format = chooseSurfaceFormat(support.formats);

    vk::PresentModeKHR present_mode = choosePresentMode(support.presentModes);

    vk::Extent2D extent = chooseExtent(width, height, support.capabilities);

    int image_count = chooseImageCount(present_mode, support.capabilities);

    vk::SwapchainCreateInfoKHR create_info = vk::SwapchainCreateInfoKHR(
        vk::SwapchainCreateFlagsKHR(), surface(), image_count, format.format,
        format.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = DEVICE_INSTANCE.getQueueFamilyIndices();

    uint32_t queue_family_indices[] = {indices.graphics_family.value(),
                                       indices.present_family.value()};

    if (indices.graphics_family != indices.present_family)
    {
        create_info.imageSharingMode      = vk::SharingMode::eConcurrent;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = vk::SharingMode::eExclusive;
    }

    create_info.preTransform   = support.capabilities.currentTransform;
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    create_info.presentMode    = present_mode;
    create_info.clipped        = VK_TRUE;

    create_info.oldSwapchain = vk::SwapchainKHR(nullptr);

    m_swapchain = LOGICAL_DEVICE_INSTANCE.createSwapchainKHRUnique(create_info);

    m_format = format.format;
    m_extent = extent;
}

size_t
SwapChain::createSwapChainFrames()
{
    auto images =
        LOGICAL_DEVICE_INSTANCE.getSwapchainImagesKHR(m_swapchain.get());

    m_frames.resize(images.size());
    for (int i = 0; i < images.size(); ++i)
    {
        m_frames[i].createImage(images[i], m_format);
        // m_frames[i].createFrameBuffer(
        //     render_way_storage.getRenderWay(RenderWayType::UNIVERSAL)
        //         ->renderPass(),
        // m_extent);
        m_frames[i].createCommandBuffer();
        m_frames[i].createSynchronization();
        // m_frames[i].createDescriptorSet(
        //     render_way_storage.getRenderWay(RenderWayType::UNIVERSAL)
        //         ->descManager());
    }
    return images.size();
}

bool
SwapChain::present(uint32_t index, const vk::Semaphore* wait_sems)
{
    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = wait_sems;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains    = &m_swapchain.get();
    presentInfo.pImageIndices  = &index;

    vk::Result present;
    try
    {
        present = DEVICE_INSTANCE.getQueue("present").presentKHR(presentInfo);
    }
    catch (vk::OutOfDateKHRError error)
    {
        present = vk::Result::eErrorOutOfDateKHR;
    }

    if (present == vk::Result::eErrorOutOfDateKHR ||
        present == vk::Result::eSuboptimalKHR)
    {
        // recreate();
        return false;
    }
    return true;
}

// void
// SwapChain::recordCommandBuffer(SwapChainFrame& frame,
//                                const RenderWay& render_way)
// {
//     const vk::CommandBuffer& command_buffer_ref = frame.commandBuffer();

//     command_buffer_ref.reset();

//     vk::CommandBufferBeginInfo beginInfo = {};

//     command_buffer_ref.begin(beginInfo);

//     vk::RenderPassBeginInfo renderPassInfo = {};
//     renderPassInfo.renderPass              = render_way.renderPass();
//     renderPassInfo.framebuffer             = frame.framebuffer();

//     renderPassInfo.renderArea.offset.x = 0;
//     renderPassInfo.renderArea.offset.y = 0;
//     renderPassInfo.renderArea.extent   = extent();

//     renderPassInfo.clearValueCount = 1;
//     vk::ClearValue clear_value{vk::ClearColorValue(1.f, 1.f, 1.f, 1.f)};

//     renderPassInfo.pClearValues = &clear_value;

//     command_buffer_ref.beginRenderPass(&renderPassInfo,
//                                        vk::SubpassContents::eInline);

//     render_way.bind(command_buffer_ref, frame.descriptorSets());

//     command_buffer_ref.endRenderPass();

//     command_buffer_ref.end();
// }

}; // namespace render
}; // namespace kusengine
