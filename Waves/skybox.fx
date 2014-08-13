TextureCube gCubeMap;

SamplerState SampleType;

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

struct VertexIn
{
    float3 posL : POSITION;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
	float3 posL : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	vout.posH = mul(float4(vin.posL, 1.0f), worldMatrix);
	vout.posH = mul(vout.posH, viewMatrix);
	vout.posH = mul(vout.posH, projectionMatrix).xyww;

	// Use local vertex position as cubemap lookup vector.
	vout.posL = vin.posL;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gCubeMap.Sample(SampleType, pin.posL);
}