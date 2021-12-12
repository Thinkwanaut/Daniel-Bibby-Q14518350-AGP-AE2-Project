Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float4 added_colour : ADDED_COLOUR;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer CBuffer0
{
	matrix WVPMatrix;
	float4 tint_colour;
	float4 added_colour;
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
	float4 point_light_position;
	float4 point_light_colour;
};

VOut VShader(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;
	float directional_diffuse = saturate(dot(directional_light_vector, normal));

	float4 point_vector = point_light_position - position;
	float point_diffuse = saturate(dot(normalize(point_vector), normal)) / pow(length(point_vector), 3);

	output.position = mul(WVPMatrix, position);
	output.colour = tint_colour + ambient_light_colour + 
		directional_light_colour * directional_diffuse + 
		point_light_colour * point_diffuse;
	output.added_colour = added_colour;
	output.texcoord = texcoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 colour : COLOUR, float4 added_colour : ADDED_COLOUR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	added_colour[3] = texture0.Sample(sampler0, texcoord)[3];
	return colour * texture0.Sample(sampler0, texcoord) + added_colour;
}