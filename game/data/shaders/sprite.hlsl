#pragma pack_matrix(row_major)

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

cbuffer vp : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer m : register(b1)
{
    float4x4 model;
}

PSInput VSMain(float2 position : POSITION, float2 texcoord : TEXCOORD0)
{
    PSInput result;

	float4x4 mvp = mul(model, mul(projection, view));

    result.position = mul(float4(position, 0.0f, 1.0f), mvp);
    result.texcoord = texcoord;

    return result;
};

Texture2D sprite : register(t0);
SamplerState smp : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
    return sprite.Sample(smp, input.texcoord);
}
