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
	output.position.y =  2.0*sin(6.28*output.position.x / 100.0 + 3.14*5*time);
    
	output.position = mul(output.position, worldMatrix);

    output.tex = input.tex;

	//These ALSO HAVE TO MATCH, DERIVATIVES AND WHATNOT
	output.normal.y = 0.5;
	output.normal.x = 6.28/100*2.0*cos((6.28*output.position.x / 100.0) + (3.14 * 5 * time));
	output.normal.z = 0.0;

	output.tex.x = output.tex.x+time;

	// Do this after normal so our view doesn't change it
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.normal = normalize(output.normal);
    
    return output;
}