/******************************************************************************

Copyright 2021 Evgeny Gorodetskiy

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

FILE: MethaneKit/Apps/Tutorials/02-HelloCube/Shaders/Cube.hlsl
Shaders for colored cube rendering

******************************************************************************/

struct VSInput
{
    float3 position : POSITION;
    float3 color    : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

PSInput CubeVS(VSInput input)
{
    PSInput output;
    output.position = float4(input.position, 1.F);
    output.color    = float4(input.color, 1.F);
    return output;
}

float4 CubePS(PSInput input) : SV_TARGET
{
    return input.color;
}
