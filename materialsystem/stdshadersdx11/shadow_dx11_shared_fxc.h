#ifndef SHADOW_DX11_SHARED_FXC_H_
#define SHADOW_DX11_SHARED_FXC_H_

struct Shadow_t
{
    float4 BaseTexCoordTransform[2];
    float4 TextureJitter[2];
    float4 Color;
};

#endif