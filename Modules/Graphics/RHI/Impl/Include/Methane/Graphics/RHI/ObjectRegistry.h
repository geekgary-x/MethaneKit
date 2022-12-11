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

FILE: Methane/Graphics/RHI/ObjectRegistry.h
Methane ObjectRegistry PIMPL wrappers for direct calls to final implementation.

******************************************************************************/

#pragma once

#include "Pimpl.h"

#include <Methane/Graphics/RHI/ICommandList.h>

namespace Methane::Graphics::Rhi
{

class ObjectRegistry
{
public:
    META_PIMPL_DEFAULT_CONSTRUCT_METHODS_DECLARE(ObjectRegistry);

    ObjectRegistry(const Ptr<IObjectRegistry>& interface_ptr);
    ObjectRegistry(IObjectRegistry& interface);
    ObjectRegistry(const Refs<ICommandList>& command_list_refs, Opt<Data::Index> frame_index_opt);

    void Init(const Refs<ICommandList>& command_list_refs, Opt<Data::Index> frame_index_opt);
    void Release();

    bool IsInitialized() const META_PIMPL_NOEXCEPT;
    IObjectRegistry& GetInterface() const META_PIMPL_NOEXCEPT;

private:
    class Impl;

    UniquePtr<Impl> m_impl_ptr;
};

} // namespace Methane::Graphics::Rhi
