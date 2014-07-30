cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

cbuffer InfoBuffer
{
	float1 time;
	float1 g1;
	float1 g2;
	float1 g3;
}

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

PixelInputType ProceduralTerrainVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	input.position.w = 1.0f;

	output.position = input.position;

	// THIS HAS TO BE THE SAME AS THE GAMELOGIC IN PROCEDURALTERRAIN.CPP OR IT WILL BE WIGGITY WHACK
	output.position.y =  5.0*sin((6.28*output.position.x / 100.0) + (3.14 * 5 * time)) + 10.0*sin(((3.14*output.position.x) / 100.0) + (2 * 3.14 * time));
    
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
    
    return output;
}