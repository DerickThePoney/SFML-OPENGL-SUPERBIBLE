#sh VERTEX 
#version 430 core

layout (location = 0) in vec4 position;

out VS_OUT
{
	//color stuff
	vec4 color;
} vs_out;

//uniforms
layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;

void main(void)
{
	vs_out.color = vec4(1.0 , 0.0 , 1.0 , 1.0);
	gl_Position = projData.projectionMatrix * projData.worldViewMatrix * position;///position.w;	
}

#sh FRAGMENT 
#version 430 core

in VS_OUT
{
	vec4 color;
} fs_in;

layout (location = 0) out vec4 color;

void main(void)
{
	color = fs_in.color;
}
