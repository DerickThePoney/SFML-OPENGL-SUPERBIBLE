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

//layout(location = 0) in vec3 fragColor;
//layout(location = 1) in vec2 fragTexCoord;
//layout(location = 2) in vec3 fragNormal;
//layout (location = 3) in vec4 fragShadowCoord;
//layout (location = 4) in vec4 fragViewDir;
//layout (location = 5) in mat3 TBN;*/

layout(binding = 1) uniform sampler2D shadowMap;
layout(binding = 2) uniform sampler2D diffuseMap;
layout(binding = 3) uniform sampler2D ambientMap;
layout(binding = 4) uniform sampler2D opacityMap;
layout(binding = 5) uniform sampler2D normalMap;


#define ambient 0.5
const vec3 lightDir = vec3(0,-1,-1);
float textureProj(vec4 P, vec2 off)
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( shadowMap, shadowCoord.xy + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z - 0.005 ) 
		{
			shadow = ambient;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

vec4 ComputeDiffuseComponent(vec3 lightDir, vec3 fragNormal, vec4 diffuseColor)
{
	return diffuseColor;
}


vec4 ComputeDiffuseLighting(vec3 lightDir, vec3 fragNormal, vec4 diffuseColor)
{
	return  1.0 * ComputeDiffuseComponent(lightDir, fragNormal, diffuseColor) * clamp(dot(fragNormal, lightDir),0.0,1.0);
}

vec4 ComputeSpecularComponent(vec3 lightDir, vec3 fragNormal, vec3 viewDir, vec4 specularColor)
{
	vec3 halfVector = normalize(viewDir + lightDir);
	float ndoth = clamp(dot(fragNormal, halfVector), 0.0,1.0);
	return specularColor * pow(ndoth, 128);
}

vec4 ComputeSpecularLighting(vec3 lightDir, vec3 fragNormal, vec3 viewDir, vec4 specularColor)
{
	vec3 halfVector = normalize(viewDir + lightDir);
	float ndoth = clamp(dot(fragNormal, halfVector), 0.0,1.0);
	return ComputeSpecularComponent(lightDir, fragNormal, viewDir, specularColor) * clamp(dot(fragNormal, lightDir),0.0,1.0);
}

vec4 ComputeLightingSpecAndDiff(vec3 lightDir, vec3 fragNormal,vec3 viewDir, vec4 diffuseColor, vec4 specularColor, vec4 ambientColor, float shadowFactor)
{
	vec4 brdf = ComputeDiffuseComponent(lightDir, fragNormal, diffuseColor) + ComputeSpecularComponent(lightDir, fragNormal, viewDir, specularColor);

	return brdf* clamp(dot(fragNormal, lightDir),0.0,1.0);
}

vec4 ComputeLighting(vec3 lightDir, vec3 fragNormal,vec3 viewDir, vec4 diffuseColor, vec4 specularColor, vec4 ambientColor, float shadowFactor)
{
	return ambient * ambientColor + shadowFactor * ComputeLightingSpecAndDiff(lightDir, fragNormal, viewDir, diffuseColor, specularColor, ambientColor, shadowFactor);
}


void main() 
{
	float shadow = 1.0;//filterPCF(fragShadowCoord / fragShadowCoord.w);

	vec4 diffuseColor = texture ( diffuseMap, outVertexData.fragTexCoord);
	vec4 ambientColor = texture ( ambientMap, outVertexData.fragTexCoord);
	//float opacity = texture ( opacityMap, fragTexCoord).r;
	outColor = ComputeLighting(normalize(lightDir), normalize(outVertexData.fragNormal), vec3(normalize(outVertexData.fragViewDir)), diffuseColor, vec4(1.0,1.0,1.0,1.0), ambientColor, shadow);
	outColor.a = diffuseColor.a;

	//outColor = vec4(outVertexData.fragNormal,1.0);
}