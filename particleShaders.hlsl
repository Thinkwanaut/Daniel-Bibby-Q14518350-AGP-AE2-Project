TextureCube cube0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 texcoord : TEXCOORD;
};

cbuffer CBuffer0
{
	matrix WVPMatrix;
	float4 colour;
};

VOut VShader(float4 position : POSITION)
{
	VOut output;

	output.texcoord = position.xy;
	output.position = mul(WVPMatrix, position);
	output.colour = float4 (1, 1, 1, 1);

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	float distSq = texcoord.x * texcoord.x + texcoord.y * texcoord.y;
clip(1.0f - distSq);
	return colour;
}