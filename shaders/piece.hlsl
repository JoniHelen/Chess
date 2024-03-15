cbuffer cb : register(b0)
{
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMartix;
}

struct VertexInput
{
    float2 positionOS : POSITION;
    float2 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 positionHCS : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOutput vert(VertexInput input)
{
    VertexOutput output;
    output.positionHCS = mul(projectionMartix, mul(viewMatrix, mul(modelMatrix, float4(input.positionOS, 0.0, 1.0))));
    output.uv = input.uv;
    return output;
}

Texture2D mainTex : register(t0);
SamplerState mainTexSampler : register(s0);

float4 frag(VertexOutput input) : SV_TARGET
{
    return mainTex.Sample(mainTexSampler, input.uv);
}