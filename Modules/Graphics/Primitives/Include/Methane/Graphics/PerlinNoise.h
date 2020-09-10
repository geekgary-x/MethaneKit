/******************************************************************************

Copyright 2019-2020 Evgeny Gorodetskiy

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

FILE: Methane/Graphics/Noise.h
Multi-octave simplex noise generator in range [0, 1]

******************************************************************************/

#pragma once

#include <Methane/Graphics/Types.h>

#include <vector>

namespace Methane::Graphics
{

class PerlinNoise
{
public:
    PerlinNoise(float persistence = 0.5f, size_t octaves_count = 4);
    
    float operator()(Vector2f pos) const;
    float operator()(Vector3f pos) const;
    float operator()(Vector4f pos) const;

private:
    using Weights = std::vector<float>;

    static Weights GetWeights(float persistence, size_t octaves_count);
    static float GetWeightsSum(const PerlinNoise::Weights& weights);

    const Weights m_weights;
    const float   m_norm_multiplier;
};

} // namespace Methane::Graphics