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

float4 frag(VertexOutput input) : SV_TARGET
{
    float2 board = floor(input.uv * 8);

    float2 res = fmod(board, 2.0f);

    float3 col = res.x == res.y ? float3(0.39607843137, 0.0, 0.0156862745) : float3(0.94509803921, 0.76470588235, 0.55686274509);

    return float4(col, 1.0);
}