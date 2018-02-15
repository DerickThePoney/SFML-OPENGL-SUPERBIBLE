#version 430 core
#sh INTERPOLATOR 

VS_OUT
{
	vec2 TexCoord;
	vec4 WorldPos;
	vec3 WorldNormal;
} interpolator;

#sh UNIFORMS 
#define NUM_LIGHTS 10
uniform layout(location = 0) mat4 modelToWorldMatrix;
uniform layout(location = 1) mat4 invModelToWorldMatrix;

uniform layout(location = 2) sampler2D heightMap;
uniform layout(location = 3) float displacementStrength = 10.0;

//Lighting data
struct Light
{
	vec4 m_kPosition;
	vec4 m_kDirection;
	vec4 m_kLightColor;
	int m_eLight;
	float m_fRange;
	float m_fConeSize;
	float m_fLightStrength;
};

layout(std140, binding = 1) uniform LightArray
{
	Light lights[NUM_LIGHTS];
} lightData;

//projection data
layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;

#sh VERTEX 
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 UV;


void main()
{
	interpolator.TexCoord = UV;
    interpolator.WorldPos = modelToWorldMatrix * position;
    interpolator.WorldNormal = mat3(modelToWorldMatrix) * normal; 
	
	float height = texture(heightMap, UV).r * displacementStrength;
	interpolator.WorldPos.y = height;
    gl_Position =  projData.projectionMatrix * projData.worldViewMatrix * interpolator.WorldPos;
}

#sh FRAGMENT 
out vec4 FragColor;

void main()
{
	FragColor = texture(heightMap,interpolator.TexCoord) * vec4(1);
}