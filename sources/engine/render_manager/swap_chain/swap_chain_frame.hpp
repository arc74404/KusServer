#ifndef SWAP_CHAIN_FRAME_HPP
#define SWAP_CHAIN_FRAME_HPP

#include <vulkan/vulkan.hpp>

#include <unordered_map>

#include "engine/render_manager/buffers/storage_buffer.hpp"
#include "engine/render_manager/buffers/uniform_buffer.hpp"
#include "engine/render_manager/commands/command_buffer.hpp"

#include "synchronization_control.hpp"

namespace kusengine::render
{
class SwapChain;
class DescriptorAllocator;

class SwapChainFrame
{
public:
    const vk::CommandBuffer& commandBuffer() const& noexcept;

    const SynchronizationControl& synControl() const& noexcept;

    const std::vector<vk::DescriptorSet>& descriptorSets() const& noexcept;

    void addFrameBuffer(std::string_view key,
                        const vk::RenderPass& render_pass,
                        const vk::Extent2D& extent);

    const vk::Framebuffer& getBuffer(const std::string& key) const&;

    void createImage(const vk::Image& image, const vk::Format& format);

    void createSynchronization();

    void createCommandBuffer();

    void createDescriptorSet(const DescriptorAllocator& desc_alloc);
    // Resourcers

    template <typename UBOType>
    void updateUBO(const UBOType& ubo)
    {
        m_uniform_buffer.setData(&ubo, sizeof(UBOType));
        writeDescriptorSetUBO();
    }
    template <typename MBDDType>
    void updateMBDD(const std::vector<MBDDType>& data)
    {
        m_storage_buffer.setData(data.data(), data.size() * sizeof(MBDDType));
        writeDescriptorSetMBDD();
    }

    void writeDescriptorSetUBO();
    void writeDescriptorSetMBDD();

    //
    void waitForFence();

    void submitCommandBuffer();

private:
    SynchronizationControl m_sync_control;
    CommandBuffer m_command_buffer;

    //
    std::unordered_map<std::string, vk::UniqueFramebuffer> m_framebuffers;

    vk::UniqueImageView m_view;

    // Resources

    UniformBuffer m_uniform_buffer;

    StorageBuffer m_storage_buffer;
    std::vector<vk::DescriptorSet> m_descriptor_sets;
};

}; // namespace kusengine::render
#endif // SWAP_CHAIN_FRAME_HPP
