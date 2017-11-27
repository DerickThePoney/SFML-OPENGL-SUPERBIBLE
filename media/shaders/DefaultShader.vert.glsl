#version 430 core

//vertex attributes
layout (location = 0) in vec4 position;


//uniforms
uniform layout(location = 0) mat4 modelToWorldMatrix;

layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;

void main(void)
{
	gl_Position = projData.projectionMatrix * projData.worldViewMatrix * modelToWorldMatrix * position;	
}
