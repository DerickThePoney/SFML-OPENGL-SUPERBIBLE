#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout (location = 3) in vec4 fragShadowCoord;
layout (location = 4) in mat3 TBN;

layout(binding = 1) uniform sampler2D shadowMap;
layout(binding = 2) uniform sampler2D diffuseMap;
layout(binding = 3) uniform sampler2D ambientMap;
layout(binding = 4) uniform sampler2D opacityMap;
layout(binding = 4) uniform sampler2D normalMap;


#define ambient 0.1
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

void main() 
{
	float shadow = 1.0;//filterPCF(fragShadowCoord / fragShadowCoord.w);

	vec4 diffuseColor = texture ( diffuseMap, fragTexCoord);
	vec4 ambientColor = texture ( ambientMap, fragTexCoord);
	//float opacity = texture ( opacityMap, fragTexCoord).r;
    outColor = vec4(.1,.1,.1,1.0) * ambientColor + shadow * 1.0 * diffuseColor * clamp(dot(normalize(fragNormal), normalize(vec3(-1,-1,-1))),0.0,1.0);
	outColor.a = diffuseColor.a;
}