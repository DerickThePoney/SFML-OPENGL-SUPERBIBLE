#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;


layout (location = 0)in VertexData
{
	vec3 fragColor;
	vec2 fragTexCoord;
	vec3 fragNormal;
	vec4 fragShadowCoord;
	vec4 fragViewDir;
	mat3 TBN;
} outVertexData;


layout(binding = 1) uniform sampler2D shadowMap;
layout(binding = 2) uniform sampler2D diffuseMap;
layout(binding = 3) uniform sampler2D ambientMap;
layout(binding = 4) uniform sampler2D opacityMap;
layout(binding = 5) uniform sampler2D normalMap;




void main() 
{
	outColor = vec4(1.0,0,0,1);
}