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

FILE: Methane/Graphics/DirectX/ICommandList.h
DirectX 12 command list interface and debug group implementation.

******************************************************************************/

#pragma once

#include <Methane/Graphics/RHI/IResourceBarriers.h>

#include <directx/d3d12.h>

namespace Methane::Graphics::DirectX
{

class CommandQueue;
class CommandListDebugGroup;

struct ICommandList
{
    using DebugGroup = CommandListDebugGroup;

    virtual CommandQueue& GetDirectCommandQueue() = 0;
    virtual ID3D12GraphicsCommandList& GetNativeCommandList() const = 0;
    virtual ID3D12GraphicsCommandList4* GetNativeCommandList4() const = 0;
    virtual void SetResourceBarriers(const Rhi::IResourceBarriers& resource_barriers) = 0;

    virtual ~ICommandList() = default;
};

} // namespace Methane::Graphics::DirectX
