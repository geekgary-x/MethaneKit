/******************************************************************************

Copyright 2022 Evgeny Gorodetskiy

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

FILE: Methane/Graphics/RHI/RenderCommandList.cpp
Methane RenderCommandList PIMPL wrappers for direct calls to final implementation.

******************************************************************************/

#include <Methane/Graphics/RHI/RenderCommandList.h>
#include <Methane/Graphics/RHI/CommandQueue.h>
#include <Methane/Graphics/RHI/CommandListDebugGroup.h>
#include <Methane/Graphics/RHI/ResourceBarriers.h>
#include <Methane/Graphics/RHI/Buffer.h>
#include <Methane/Graphics/RHI/RenderPass.h>
#include <Methane/Graphics/RHI/RenderState.h>
#include <Methane/Graphics/RHI/ViewState.h>
#include <Methane/Graphics/RHI/ProgramBindings.h>

#if defined METHANE_GFX_DIRECTX

#include <Methane/Graphics/DirectX/RenderCommandList.h>
using RenderCommandListImpl = Methane::Graphics::DirectX::RenderCommandList;

#elif defined METHANE_GFX_VULKAN

#include <Methane/Graphics/Vulkan/RenderCommandList.h>
using RenderCommandListImpl = Methane::Graphics::Vulkan::RenderCommandList;

#elif defined METHANE_GFX_METAL

#include <Methane/Graphics/Metal/RenderCommandList.hh>
using RenderCommandListImpl = Methane::Graphics::Metal::RenderCommandList;

#else // METHAN_GFX_[API] is undefined

static_assert(false, "Static graphics API macro-definition is missing.");

#endif

#include "ImplWrapper.hpp"

#include <Methane/Instrumentation.h>

namespace Methane::Graphics::Rhi
{

class RenderCommandList::Impl
    : public ImplWrapper<IRenderCommandList, RenderCommandListImpl>
{
public:
    using ImplWrapper::ImplWrapper;
};

META_PIMPL_DEFAULT_CONSTRUCT_METHODS_IMPLEMENT(RenderCommandList);

RenderCommandList::RenderCommandList(UniquePtr<Impl>&& impl_ptr)
    : Transmitter<Rhi::ICommandListCallback>(impl_ptr->GetInterface())
    , Transmitter<Rhi::IObjectCallback>(impl_ptr->GetInterface())
    , m_impl_ptr(std::move(impl_ptr))
{
}

RenderCommandList::RenderCommandList(const Ptr<IRenderCommandList>& interface_ptr)
    : RenderCommandList(std::make_unique<Impl>(interface_ptr))
{
}

RenderCommandList::RenderCommandList(IRenderCommandList& interface_ref)
    : RenderCommandList(interface_ref.GetDerivedPtr<IRenderCommandList>())
{
}

RenderCommandList::RenderCommandList(const CommandQueue& command_queue, const RenderPass& render_pass)
    : RenderCommandList(IRenderCommandList::Create(command_queue.GetInterface(), render_pass.GetInterface()))
{
}

void RenderCommandList::Init(const CommandQueue& command_queue, const RenderPass& render_pass)
{
    m_impl_ptr = std::make_unique<Impl>(IRenderCommandList::Create(command_queue.GetInterface(), render_pass.GetInterface()));
    Transmitter<Rhi::ICommandListCallback>::Reset(&m_impl_ptr->GetInterface());
    Transmitter<Rhi::IObjectCallback>::Reset(&m_impl_ptr->GetInterface());
}

void RenderCommandList::Release()
{
    Transmitter<Rhi::ICommandListCallback>::Reset();
    Transmitter<Rhi::IObjectCallback>::Reset();
    m_impl_ptr.release();
}

bool RenderCommandList::IsInitialized() const META_PIMPL_NOEXCEPT
{
    return static_cast<bool>(m_impl_ptr);
}

IRenderCommandList& RenderCommandList::GetInterface() const META_PIMPL_NOEXCEPT
{
    return GetPublicInterface(m_impl_ptr);
}

bool RenderCommandList::SetName(std::string_view name) const
{
    return GetPrivateImpl(m_impl_ptr).SetName(name);
}

std::string_view RenderCommandList::GetName() const META_PIMPL_NOEXCEPT
{
    return GetPrivateImpl(m_impl_ptr).GetName();
}

void RenderCommandList::PushDebugGroup(const DebugGroup& debug_group) const
{
    GetPrivateImpl(m_impl_ptr).PushDebugGroup(debug_group.GetInterface());
}

void RenderCommandList::PopDebugGroup() const
{
    GetPrivateImpl(m_impl_ptr).PopDebugGroup();
}

void RenderCommandList::Reset(const DebugGroup* debug_group_ptr) const
{
    GetPrivateImpl(m_impl_ptr).Reset(debug_group_ptr ? &debug_group_ptr->GetInterface() : nullptr);
}

void RenderCommandList::ResetOnce(const DebugGroup* debug_group_ptr) const
{
    GetPrivateImpl(m_impl_ptr).ResetOnce(debug_group_ptr ? &debug_group_ptr->GetInterface() : nullptr);
}

void RenderCommandList::SetProgramBindings(const ProgramBindings& program_bindings, ProgramBindingsApplyBehaviorMask apply_behavior) const
{
    GetPrivateImpl(m_impl_ptr).SetProgramBindings(program_bindings.GetInterface(), apply_behavior);
}

void RenderCommandList::SetResourceBarriers(const ResourceBarriers& resource_barriers) const
{
    GetPrivateImpl(m_impl_ptr).SetResourceBarriers(resource_barriers.GetInterface());
}

void RenderCommandList::Commit() const
{
    GetPrivateImpl(m_impl_ptr).Commit();
}

void RenderCommandList::WaitUntilCompleted(uint32_t timeout_ms) const
{
    GetPrivateImpl(m_impl_ptr).WaitUntilCompleted(timeout_ms);
}

Data::TimeRange RenderCommandList::GetGpuTimeRange(bool in_cpu_nanoseconds) const
{
    return GetPrivateImpl(m_impl_ptr).GetGpuTimeRange(in_cpu_nanoseconds);
}

CommandListState RenderCommandList::GetState() const META_PIMPL_NOEXCEPT
{
    return GetPrivateImpl(m_impl_ptr).GetState();
}

CommandQueue RenderCommandList::GetCommandQueue() const
{
    return GetPrivateImpl(m_impl_ptr).GetCommandQueue();
}

bool RenderCommandList::IsValidationEnabled() const META_PIMPL_NOEXCEPT
{
    return GetPrivateImpl(m_impl_ptr).IsValidationEnabled();
}

void RenderCommandList::SetValidationEnabled(bool is_validation_enabled) const
{
    GetPrivateImpl(m_impl_ptr).SetValidationEnabled(is_validation_enabled);
}

RenderPass RenderCommandList::GetRenderPass() const
{
    return RenderPass(GetPrivateImpl(m_impl_ptr).GetRenderPass());
}

void RenderCommandList::ResetWithState(const RenderState& render_state, const DebugGroup* debug_group_ptr) const
{
    GetPrivateImpl(m_impl_ptr).ResetWithState(render_state.GetInterface(), debug_group_ptr ? &debug_group_ptr->GetInterface() : nullptr);
}

void RenderCommandList::ResetWithStateOnce(const RenderState& render_state, const DebugGroup* debug_group_ptr) const
{
    GetPrivateImpl(m_impl_ptr).ResetWithStateOnce(render_state.GetInterface(), debug_group_ptr ? &debug_group_ptr->GetInterface() : nullptr);
}

void RenderCommandList::SetRenderState(const RenderState& render_state, RenderStateGroupMask state_groups) const
{
    GetPrivateImpl(m_impl_ptr).SetRenderState(render_state.GetInterface(), state_groups);
}

void RenderCommandList::SetViewState(const ViewState& view_state) const
{
    GetPrivateImpl(m_impl_ptr).SetViewState(view_state.GetInterface());
}

bool RenderCommandList::SetVertexBuffers(const BufferSet& vertex_buffers, bool set_resource_barriers) const
{
    return GetPrivateImpl(m_impl_ptr).SetVertexBuffers(vertex_buffers.GetInterface(), set_resource_barriers);
}

bool RenderCommandList::SetIndexBuffer(const Buffer& index_buffer, bool set_resource_barriers) const
{
    return GetPrivateImpl(m_impl_ptr).SetIndexBuffer(index_buffer.GetInterface(), set_resource_barriers);
}

void RenderCommandList::DrawIndexed(Primitive primitive, uint32_t index_count,
                                    uint32_t start_index, uint32_t start_vertex,
                                    uint32_t instance_count, uint32_t start_instance) const
{
    GetPrivateImpl(m_impl_ptr).DrawIndexed(primitive, index_count, start_index, start_vertex, instance_count, start_instance);
}

void RenderCommandList::Draw(Primitive primitive,
                             uint32_t vertex_count, uint32_t start_vertex,
                             uint32_t instance_count, uint32_t start_instance) const
{
    GetPrivateImpl(m_impl_ptr).Draw(primitive, vertex_count, start_vertex, instance_count, start_instance);
}

} // namespace Methane::Graphics::Rhi
