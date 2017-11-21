#version 450 core

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

uniform mat4 projectionMatrix;
uniform mat4 moveMatrix;
uniform vec3 lightPosition = vec3(100,100,100);

void main(void)
{
	
	vec4 P = moveMatrix * position;
	vs_out.N = mat3(moveMatrix) * normal;
	vs_out.L = lightPosition - P.xyz;
	vs_out.V = -P.xyz;

	vs_out.UV = UV;
	vs_out.color = color;
	gl_Position = projectionMatrix * P;
	
}
