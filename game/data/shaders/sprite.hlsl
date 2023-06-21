struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

PSInput VSMain(float2 position : POSITION, float2 texcoord : TEXCOORD0)
{
    PSInput result;

    result.position = float4(position, 0.0, 0.0);
    result.texcoord = texcoord;

    return result;
};

Texture2D sprite : register(t0);
SamplerState smp : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
    return sprite.Sample(smp, input.texcoord);
}
