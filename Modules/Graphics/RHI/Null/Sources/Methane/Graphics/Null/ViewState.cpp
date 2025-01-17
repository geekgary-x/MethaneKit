/******************************************************************************

Copyright 2023 Evgeny Gorodetskiy

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

FILE: Methane/Graphics/Null/ViewState.cpp
Null implementation of the view state interface.

******************************************************************************/

#include <Methane/Graphics/Null/ViewState.h>

namespace Methane::Graphics::Rhi
{

Ptr<IViewState> Rhi::IViewState::Create(const Rhi::IViewState::Settings& state_settings)
{
    META_FUNCTION_TASK();
    return std::make_shared<Null::ViewState>(state_settings);
}

} // namespace Methane::Graphics::Rhi

