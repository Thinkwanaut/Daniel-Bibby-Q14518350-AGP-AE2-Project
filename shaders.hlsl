Texture2D texture0;
Texture2D texture1;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer CBuffer0
{
	matrix WVPMatrix;
	float4 base_colour;
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
	float4 point_light_position;
	float4 point_light_colour;
};

VOut VShader(float4 position : POSITION, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;
	float directional_diffuse = saturate(dot(directional_light_vector, normal));

	float4 point_vector = point_light_position - position;
	float point_diffuse = saturate(dot(normalize(point_vector), normal)) / pow(length(point_vector), 3);

	output.position = mul(WVPMatrix, position);
	output.color = base_colour + ambient_light_colour + 
		directional_light_colour * directional_diffuse + 
		point_light_colour * point_diffuse;
	output.texcoord = texcoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}