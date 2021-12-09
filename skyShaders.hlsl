TextureCube cube0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float3 texcoord : TEXCOORD;
};

cbuffer CBuffer0
{
	matrix WVPMatrix;
};

VOut VShader(float4 position : POSITION, float3 texcoord : TEXCOORD)
{
	VOut output;

	output.texcoord = position.xyz;
	output.position = mul(WVPMatrix, position);
	output.colour = float4 (1, 1, 1, 1);

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 colour : COLOUR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return colour * cube0.Sample(sampler0, texcoord);
}