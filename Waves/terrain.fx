Texture2D flatTexture;
Texture2D slopedTexture;

SamplerState SampleType;

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightBuffer
{
	float4 diffuseColor;
	float4 fillColor;
	float3 lightDirection;
	float padding;
};

struct VertexIn
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelIn
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

PixelIn VS(VertexIn vin)
{
	PixelIn vout;

	vin.position.w = 1.0f;
	vout.position = mul(vin.position, worldMatrix);
	vout.position = mul(vout.position, viewMatrix);
	vout.position = mul(vout.position, projectionMatrix);

	vout.tex = vin.tex;

	vout.normal = mul(vin.normal, (float3x3)worldMatrix);
	vout.normal = normalize(vout.normal);

	return vout;
}

float4 PS(PixelIn pin) : SV_Target
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float flatness;

	flatness = dot(pin.normal, float3(0, 2, 0));

	flatness = flatness / 2;

	if (flatness > 1.0f)
	{
		flatness = 1.0f;
	}

	color = fillColor;

	textureColor = flatTexture.Sample(SampleType, pin.tex)*flatness + slopedTexture.Sample(SampleType, pin.tex)*(1-flatness);

	lightDir = -lightDirection;
	lightIntensity = saturate(dot(pin.normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		color += (diffuseColor * lightIntensity);
	}

	color = saturate(color);
	color = color*textureColor;

	return color;
}