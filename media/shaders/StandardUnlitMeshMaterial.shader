#sh VERTEX 
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 UV;

out VS_OUT
{
	vec3 N;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} vs_out;

//uniforms
uniform layout(location = 0) mat4 modelToWorldMatrix;

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
	
	vs_out.UV = UV;
	vs_out.color = color;
	gl_Position = projData.projectionMatrix * P;
	
}

#sh FRAGMENT 
#version 430 core

in VS_OUT
{
	vec3 N;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} fs_in;

uniform layout(location = 1) vec3 diffuse_color = vec3(0.5);
layout (location = 0) out vec4 color;

void main(void)
{	
	color = clamp(vec4(diffuse_color,1), 0 , 1);
}
