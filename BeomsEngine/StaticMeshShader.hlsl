// StaticMeshShader.hlsl
// Basic Static Mesh Vertex and Pixel Shader
// Coordinate System: Z-Up, Left-Handed

// Constant Buffer for transformation matrices
cbuffer ConstantBuffer : register(b0)
{
    row_major matrix World;
    row_major matrix View;
    row_major matrix Projection;
};

// Vertex Shader Input (matches FVertex structure)
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

// Vertex Shader Output / Pixel Shader Input
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD1;
};

// Vertex Shader
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // Transform to world space
    float4 worldPos = mul(float4(input.Position, 1.0f), World);
    output.WorldPos = worldPos.xyz;

    // Transform to view space
    float4 viewPos = mul(worldPos, View);

    // Transform to projection space
    output.Position = mul(viewPos, Projection);

    // Transform normal to world space (assume no non-uniform scaling)
    output.Normal = mul(input.Normal, (float3x3)World);
    output.Normal = normalize(output.Normal);

    // Pass through texture coordinates
    output.TexCoord = input.TexCoord;

    return output;
}

// Pixel Shader
float4 PS(PS_INPUT input) : SV_Target
{
    // Simple directional lighting
    float3 lightDir = normalize(float3(1.0f, 1.0f, 1.0f)); // Light from top-right-front
    float3 normal = normalize(input.Normal);

    // Ambient + Diffuse lighting
    float3 ambient = float3(0.2f, 0.2f, 0.2f);
    float diffuse = max(dot(normal, lightDir), 0.0f);

    // Base color (light gray)
    float3 baseColor = float3(0.7f, 0.7f, 0.7f);

    float3 finalColor = baseColor * (ambient + diffuse * 0.8f);

    return float4(finalColor, 1.0f);
}
