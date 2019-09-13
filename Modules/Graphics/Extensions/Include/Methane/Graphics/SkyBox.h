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

FILE: SkyBox.h
SkyBox rendering primitive

******************************************************************************/

#pragma once

#include "ImageLoader.h"
#include "MeshBuffers.hpp"

#include <Methane/Graphics/Context.h>
#include <Methane/Graphics/MathTypes.h>

#include <memory>
#include <array>
#include <string>

namespace Methane::Graphics
{

class SkyBox
{
public:
    using Ptr = std::shared_ptr<SkyBox>;

    enum class Face : uint32_t
    {
        PositiveX = 0u,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ,

        Count
    };

    using FaceResources = std::array<std::string, static_cast<uint32_t>(Face::Count)>;

    struct Settings
    {
        FaceResources face_resources;
    };

    SkyBox(Context& context, ImageLoader& image_loader, const Settings& settings);

private:
    struct SHADER_STRUCT_ALIGN BoxMeshUniforms
    {
        SHADER_FIELD_ALIGN Matrix44f mvp_matrix;
    };

    using TexturedMeshBuffers = TexturedMeshBuffers<BoxMeshUniforms>;

    Settings            m_settings;
    Context&            m_context;
    TexturedMeshBuffers m_mesh_buffers;
};

} // namespace Methane::Graphics
