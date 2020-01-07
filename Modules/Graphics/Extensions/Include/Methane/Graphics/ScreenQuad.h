/******************************************************************************

Copyright 2019 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************

FILE: Methane/Graphics/ScreenQuad.h
ScreenQuad rendering primitive.

******************************************************************************/

#pragma once

#include <Methane/Graphics/Context.h>
#include <Methane/Graphics/Texture.h>
#include <Methane/Graphics/Buffer.h>
#include <Methane/Graphics/RenderState.h>
#include <Methane/Graphics/Program.h>
#include <Methane/Graphics/Sampler.h>
#include <Methane/Graphics/MathTypes.h>
#include <Methane/Graphics/Types.h>

#include <memory>

namespace Methane::Graphics
{

class RenderCommandList;

class ScreenQuad
{
public:
    using Ptr = std::shared_ptr<ScreenQuad>;

    struct Settings
    {
        const std::string  name;
        FrameRect   screen_rect;
        Color4f     blend_color = Color4f(1.f, 1.f, 1.f, 1.f);
    };

    ScreenQuad(Context& context, Texture::Ptr sp_texture, Settings settings);

    void SetBlendColor(const Color4f& blend_color);
    void SetScreenRect(const FrameRect& screen_rect);

    void Draw(RenderCommandList& cmd_list) const;

private:
    struct SHADER_STRUCT_ALIGN Constants
    {
        SHADER_FIELD_ALIGN Color4f blend_color;
    };

    Settings                       m_settings;
    Constants                      m_constants;
    const std::string              m_debug_region_name;
    RenderState::Ptr               m_sp_state;
    Buffer::Ptr                    m_sp_vertex_buffer;
    Buffer::Ptr                    m_sp_index_buffer;
    Buffer::Ptr                    m_sp_const_buffer;
    Texture::Ptr                   m_sp_texture;
    Sampler::Ptr                   m_sp_texture_sampler;
    Program::ResourceBindings::Ptr m_sp_const_resource_bindings;
};

} // namespace Methane::Graphics
