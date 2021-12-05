Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : color;
};

VOut TextVS(float4 position : POSITION,  float2 texcoord : TEXCOORD, float4 color : COLOR)
{
	VOut output;

	output.position = position;

	output.texcoord = texcoord;

	output.color = color;

	return output;
}


 float4 TextPS(float4 position : SV_POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR) : SV_TARGET 
 {
	return color * texture0.Sample(sampler0, texcoord);
 }