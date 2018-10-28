#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 lightSpace;
	float time;
} ubo;

out gl_PerVertex {
    vec4 gl_Position;
};


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inTexCoord;


layout (location = 0)out VertexData
{
	vec3 fragColor;
	vec2 fragTexCoord;
	vec3 fragNormal;
	vec4 fragShadowCoord;
	vec4 fragViewDir;
	mat3 TBN;
} outVertexData;


const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);
void main() 
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    outVertexData.fragColor = inColor;
    outVertexData.fragTexCoord = inTexCoord;
	outVertexData.fragTexCoord.y = 1 - outVertexData.fragTexCoord.y;
	outVertexData.fragNormal = normalize(ubo.proj * ubo.view * ubo.model * vec4(inNormal,0.0)).xyz;
	outVertexData.fragShadowCoord = (biasMat * ubo.lightSpace) * vec4(inPosition, 1.0);

	vec3 bitangent = cross(inNormal, inTangent);
	outVertexData.TBN = mat3(inTangent, bitangent, inNormal);

	//vompute the view dir
	outVertexData.fragViewDir = ubo.model * vec4(inPosition, 1.0) - vec4(213,120,-8,1);
	outVertexData.fragViewDir = ubo.proj * ubo.view * outVertexData.fragViewDir;
}