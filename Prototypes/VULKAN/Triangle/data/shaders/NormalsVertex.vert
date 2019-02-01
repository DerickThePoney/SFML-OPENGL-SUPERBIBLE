#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout(location = 4) in vec2 inTexCoord;

layout (location = 0) out vec3 outNormal;
layout(location = 4) out vec2 outTexCoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main(void)
{
	outNormal = inNormal;
	outTexCoord = inTexCoord;
	gl_Position = vec4(inPos.xyz, 1.0);
}