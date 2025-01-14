/******************************************************************************

Copyright 2019-2020 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License"),
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************

FILE: Methane/Graphics/Vulkan/RenderState.cpp
Vulkan implementation of the render state interface.

******************************************************************************/

#include <Methane/Graphics/Vulkan/RenderState.h>
#include <Methane/Graphics/Vulkan/RenderPattern.h>
#include <Methane/Graphics/Vulkan/IContext.h>
#include <Methane/Graphics/Vulkan/Device.h>
#include <Methane/Graphics/Vulkan/RenderCommandList.h>
#include <Methane/Graphics/Vulkan/Program.h>
#include <Methane/Graphics/Vulkan/Shader.h>
#include <Methane/Graphics/Vulkan/Types.h>
#include <Methane/Graphics/Vulkan/Utils.hpp>

#include <Methane/Graphics/Base/RenderContext.h>
#include <Methane/Instrumentation.h>
#include <Methane/Checks.hpp>

#include <algorithm>

namespace Methane::Graphics::Vulkan
{

[[nodiscard]]
static vk::PolygonMode RasterizerFillModeToVulkan(Rhi::IRenderState::Rasterizer::FillMode fill_mode)
{
    META_FUNCTION_TASK();
    using FillMode = Rhi::IRenderState::Rasterizer::FillMode;
    switch(fill_mode)
    {
    case FillMode::Solid:     return vk::PolygonMode::eFill;
    case FillMode::Wireframe: return vk::PolygonMode::eLine;
    default:
        META_UNEXPECTED_ARG_RETURN(fill_mode, vk::PolygonMode::eFill);
    }
}

[[nodiscard]]
static vk::CullModeFlags RasterizerCullModeToVulkan(Rhi::IRenderState::Rasterizer::CullMode cull_mode)
{
    META_FUNCTION_TASK();
    using CullMode = Rhi::IRenderState::Rasterizer::CullMode;
    switch(cull_mode)
    {
    case CullMode::None:  return vk::CullModeFlagBits::eNone;
    case CullMode::Back:  return vk::CullModeFlagBits::eBack;
    case CullMode::Front: return vk::CullModeFlagBits::eFront;
    default:
        META_UNEXPECTED_ARG_RETURN(cull_mode, vk::CullModeFlagBits::eNone);
    }
}

[[nodiscard]]
static vk::SampleCountFlagBits RasterizerSampleCountToVulkan(uint32_t sample_count)
{
    META_FUNCTION_TASK();
    switch(sample_count)
    {
    case 1:  return vk::SampleCountFlagBits::e1;
    case 2:  return vk::SampleCountFlagBits::e2;
    case 4:  return vk::SampleCountFlagBits::e4;
    case 8:  return vk::SampleCountFlagBits::e8;
    case 16: return vk::SampleCountFlagBits::e16;
    case 32: return vk::SampleCountFlagBits::e32;
    case 61: return vk::SampleCountFlagBits::e64;
    default:
        META_UNEXPECTED_ARG_DESCR_RETURN(sample_count, vk::SampleCountFlagBits::e1, "Vulkan rasterizer sample count should be a power of 2 from 1 to 64.");
    }
}

[[nodiscard]]
static vk::StencilOp StencilOperationToVulkan(Rhi::FaceOperation stencil_operation)
{
    META_FUNCTION_TASK();
    switch(stencil_operation)
    {
    case Rhi::FaceOperation::Keep:            return vk::StencilOp::eKeep;
    case Rhi::FaceOperation::Zero:            return vk::StencilOp::eZero;
    case Rhi::FaceOperation::Replace:         return vk::StencilOp::eReplace;
    case Rhi::FaceOperation::Invert:          return vk::StencilOp::eInvert;
    case Rhi::FaceOperation::IncrementClamp:  return vk::StencilOp::eIncrementAndClamp;
    case Rhi::FaceOperation::DecrementClamp:  return vk::StencilOp::eDecrementAndClamp;
    case Rhi::FaceOperation::IncrementWrap:   return vk::StencilOp::eIncrementAndWrap;
    case Rhi::FaceOperation::DecrementWrap:   return vk::StencilOp::eDecrementAndWrap;
    default:
        META_UNEXPECTED_ARG_RETURN(stencil_operation, vk::StencilOp::eKeep);
    }
}

[[nodiscard]]
static vk::BlendFactor BlendingFactorToVulkan(Rhi::IRenderState::Blending::Factor blend_factor)
{
    META_FUNCTION_TASK();
    using BlendFactor = Rhi::IRenderState::Blending::Factor;
    switch(blend_factor)
    {
    case BlendFactor::Zero:                     return vk::BlendFactor::eZero;
    case BlendFactor::One:                      return vk::BlendFactor::eOne;
    case BlendFactor::SourceColor:              return vk::BlendFactor::eSrcColor;
    case BlendFactor::OneMinusSourceColor:      return vk::BlendFactor::eOneMinusSrcColor;
    case BlendFactor::SourceAlpha:              return vk::BlendFactor::eSrcAlpha;
    case BlendFactor::OneMinusSourceAlpha:      return vk::BlendFactor::eOneMinusSrcAlpha;
    case BlendFactor::DestinationColor:         return vk::BlendFactor::eDstColor;
    case BlendFactor::OneMinusDestinationColor: return vk::BlendFactor::eOneMinusDstColor;
    case BlendFactor::DestinationAlpha:         return vk::BlendFactor::eDstAlpha;
    case BlendFactor::OneMinusDestinationAlpha: return vk::BlendFactor::eOneMinusDstAlpha;
    case BlendFactor::SourceAlphaSaturated:     return vk::BlendFactor::eSrcAlphaSaturate;
    case BlendFactor::BlendColor:               return vk::BlendFactor::eConstantColor;
    case BlendFactor::OneMinusBlendColor:       return vk::BlendFactor::eOneMinusConstantColor;
    case BlendFactor::BlendAlpha:               return vk::BlendFactor::eConstantAlpha;
    case BlendFactor::OneMinusBlendAlpha:       return vk::BlendFactor::eOneMinusConstantAlpha;
    case BlendFactor::Source1Color:             return vk::BlendFactor::eSrc1Color;
    case BlendFactor::OneMinusSource1Color:     return vk::BlendFactor::eOneMinusSrc1Color;
    case BlendFactor::Source1Alpha:             return vk::BlendFactor::eSrc1Alpha;
    case BlendFactor::OneMinusSource1Alpha:     return vk::BlendFactor::eOneMinusSrc1Alpha;
    default: META_UNEXPECTED_ARG_RETURN(blend_factor, vk::BlendFactor::eZero);
    }
}

[[nodiscard]]
static vk::BlendOp BlendingOperationToVulkan(Rhi::IRenderState::Blending::Operation blend_operation)
{
    META_FUNCTION_TASK();
    using BlendOperation = Rhi::IRenderState::Blending::Operation;
    switch(blend_operation)
    {
    case BlendOperation::Add:               return vk::BlendOp::eAdd;
    case BlendOperation::Subtract:          return vk::BlendOp::eSubtract;
    case BlendOperation::ReverseSubtract:   return vk::BlendOp::eReverseSubtract;
    case BlendOperation::Minimum:           return vk::BlendOp::eMin;
    case BlendOperation::Maximum:           return vk::BlendOp::eMax;
    default:
        META_UNEXPECTED_ARG_RETURN(blend_operation, vk::BlendOp::eAdd);
    }
}

[[nodiscard]]
static vk::ColorComponentFlags BlendingColorChannelsToVulkan(Rhi::BlendingColorChannelMask color_channels)
{
    META_FUNCTION_TASK();
    vk::ColorComponentFlags color_component_flags{};

    if (color_channels.HasAnyBit(Rhi::BlendingColorChannel::Red))
        color_component_flags |= vk::ColorComponentFlagBits::eR;

    if (color_channels.HasAnyBit(Rhi::BlendingColorChannel::Green))
        color_component_flags |= vk::ColorComponentFlagBits::eG;

    if (color_channels.HasAnyBit(Rhi::BlendingColorChannel::Blue))
        color_component_flags |= vk::ColorComponentFlagBits::eB;

    if (color_channels.HasAnyBit(Rhi::BlendingColorChannel::Alpha))
        color_component_flags |= vk::ColorComponentFlagBits::eA;

    return color_component_flags;
}

RenderState::RenderState(const Base::RenderContext& context, const Settings& settings)
    : Base::RenderState(context, settings)
    , m_vk_context(dynamic_cast<const IContext&>(GetRenderContext()))
{
    META_FUNCTION_TASK();
    Reset(settings);
}

void RenderState::Reset(const Settings& settings)
{
    META_FUNCTION_TASK();
    Base::RenderState::Reset(settings);

    vk::PipelineRasterizationStateCreateInfo rasterizer_info(
        vk::PipelineRasterizationStateCreateFlags{},
        false, // depthClampEnable
        false, // rasterizerDiscardEnable
        RasterizerFillModeToVulkan(settings.rasterizer.fill_mode),
        RasterizerCullModeToVulkan(settings.rasterizer.cull_mode),
        settings.rasterizer.is_front_counter_clockwise ? vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise,
        false, // depthBiasEnable
        0.F,   // depthBiasConstantFactor
        0.F,   // depthBiasClamp
        0.F,   // depthBiasSlopeFactor
        1.F    // lineWidth
    );

    vk::PipelineMultisampleStateCreateInfo multisample_info(
        vk::PipelineMultisampleStateCreateFlags{},
        RasterizerSampleCountToVulkan(settings.rasterizer.sample_count),
        false,   // sampleShadingEnable
        1.F,     // minSampleShading
        nullptr, // pSampleMask
        settings.rasterizer.alpha_to_coverage_enabled,
        false    // alphaToOneEnable
    );

    vk::PipelineDepthStencilStateCreateInfo depth_stencil_info(
        vk::PipelineDepthStencilStateCreateFlags{},
        settings.depth.enabled,
        settings.depth.write_enabled,
        TypeConverter::CompareFunctionToVulkan(settings.depth.compare),
        false, // depthBoundsTestEnable
        settings.stencil.enabled,
        vk::StencilOpState( // front face
            StencilOperationToVulkan(settings.stencil.front_face.stencil_failure),
            StencilOperationToVulkan(settings.stencil.front_face.stencil_pass),
            StencilOperationToVulkan(settings.stencil.front_face.depth_failure),
            TypeConverter::CompareFunctionToVulkan(settings.stencil.front_face.compare),
            0U, // compareMask
            0U, // writeMask
            0U  // reference
        ),
        vk::StencilOpState( // back face
            StencilOperationToVulkan(settings.stencil.back_face.stencil_failure),
            StencilOperationToVulkan(settings.stencil.back_face.stencil_pass),
            StencilOperationToVulkan(settings.stencil.back_face.depth_failure),
            TypeConverter::CompareFunctionToVulkan(settings.stencil.back_face.compare),
            0U, // compareMask
            0U, // writeMask
            0U  // reference
        ),
        0.F, // minDepthBounds
        0.F  // maxDepthBounds
    );

    const size_t blend_attachments_count = settings.blending.is_independent
                                         ? settings.program_ptr->GetSettings().attachment_formats.colors.size()
                                         : 1;
    std::vector<vk::PipelineColorBlendAttachmentState> attachment_blend_states;
    std::transform(settings.blending.render_targets.begin(),
                   settings.blending.render_targets.begin() + blend_attachments_count,
                   std::back_inserter(attachment_blend_states),
        [](const Blending::RenderTarget& rt_blending)
        {
            return vk::PipelineColorBlendAttachmentState(
                rt_blending.blend_enabled,
                BlendingFactorToVulkan(rt_blending.source_rgb_blend_factor),
                BlendingFactorToVulkan(rt_blending.dest_rgb_blend_factor),
                BlendingOperationToVulkan(rt_blending.rgb_blend_op),
                BlendingFactorToVulkan(rt_blending.source_alpha_blend_factor),
                BlendingFactorToVulkan(rt_blending.dest_alpha_blend_factor),
                BlendingOperationToVulkan(rt_blending.alpha_blend_op),
                BlendingColorChannelsToVulkan(rt_blending.color_write)
            );
        }
    );

    vk::PipelineColorBlendStateCreateInfo blending_info(
        vk::PipelineColorBlendStateCreateFlags{},
        false, // logicOpEnable
        vk::LogicOp::eCopy,
        attachment_blend_states,
        settings.blending_color.AsArray()
    );

    // Fake state, actual PrimitiveTopology is set dynamically
    vk::PipelineInputAssemblyStateCreateInfo assembly_info(
        vk::PipelineInputAssemblyStateCreateFlags{},
        vk::PrimitiveTopology::eTriangleList,
        false
    );

    // Fake viewport state, actual state is set dynamically
    vk::PipelineViewportStateCreateInfo viewport_info(
        vk::PipelineViewportStateCreateFlags{},
        0, nullptr, 0, nullptr
    );

    const std::vector<vk::DynamicState> dynamic_states = {
        vk::DynamicState::eViewportWithCountEXT,
        vk::DynamicState::eScissorWithCountEXT,
        vk::DynamicState::ePrimitiveTopologyEXT,
    };
    vk::PipelineDynamicStateCreateInfo dynamic_info(
        vk::PipelineDynamicStateCreateFlags{},
        dynamic_states
    );

    auto& program = static_cast<Program&>(*GetSettings().program_ptr);
    const auto& render_pattern = static_cast<RenderPattern&>(*GetSettings().render_pattern_ptr);

    const vk::PipelineVertexInputStateCreateInfo vk_vertex_input_state_info = program.GetNativeVertexInputStateCreateInfo();
    const std::vector<vk::PipelineShaderStageCreateInfo> vk_stages_info = program.GetNativeShaderStageCreateInfos();

    const vk::GraphicsPipelineCreateInfo vk_pipeline_create_info(
        vk::PipelineCreateFlags(),
        vk_stages_info,
        &vk_vertex_input_state_info,
        &assembly_info,
        nullptr, // no tesselation support yet
        &viewport_info,
        &rasterizer_info,
        &multisample_info,
        &depth_stencil_info,
        &blending_info,
        &dynamic_info,
        program.GetNativePipelineLayout(),
        render_pattern.GetNativeRenderPass()
    );

    auto pipe = m_vk_context.GetVulkanDevice().GetNativeDevice().createGraphicsPipelineUnique(nullptr, vk_pipeline_create_info);
    META_CHECK_ARG_EQUAL_DESCR(pipe.result, vk::Result::eSuccess, "Vulkan pipeline creation has failed");
    m_vk_unique_pipeline = std::move(pipe.value);
}

void RenderState::Apply(Base::RenderCommandList& render_command_list, Groups /*state_groups*/)
{
    META_FUNCTION_TASK();
    const auto& vulkan_render_command_list = static_cast<RenderCommandList&>(render_command_list);
    vulkan_render_command_list.GetNativeCommandBufferDefault().bindPipeline(vk::PipelineBindPoint::eGraphics, GetNativePipeline());
}

bool RenderState::SetName(std::string_view name)
{
    META_FUNCTION_TASK();
    if (!Base::RenderState::SetName(name))
        return false;

    SetVulkanObjectName(m_vk_context.GetVulkanDevice().GetNativeDevice(), m_vk_unique_pipeline.get(), name);
    return true;
}

} // namespace Methane::Graphics::Vulkan
