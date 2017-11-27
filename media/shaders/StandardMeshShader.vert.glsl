#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 UV;

out VS_OUT
{
	//light stuff
	vec3 N;
	vec3 L;
	vec3 V;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} vs_out;

//uniforms
uniform layout(location = 0) mat4 modelToWorldMatrix;
uniform layout(location = 1) vec3 lightPosition = vec3(0,0,10);

layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;



void main(void)
{
	mat4 mv_matrix = projData.worldViewMatrix * modelToWorldMatrix;
	vec4 P = mv_matrix * position;
	vs_out.N = mat3(mv_matrix) * normal;
	vs_out.L = lightPosition - P.xyz;
	vs_out.V = -P.xyz;

	vs_out.UV = UV;
	vs_out.color = color;
	gl_Position = projData.projectionMatrix * P;
	
}
