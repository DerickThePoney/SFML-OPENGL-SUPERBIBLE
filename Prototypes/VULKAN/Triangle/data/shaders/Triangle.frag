#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout (location = 3) in vec4 fragShadowCoord;

layout(binding = 1) uniform sampler2D shadowMap;
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

void main() {
	float shadow = filterPCF(fragShadowCoord / fragShadowCoord.w);//, vec2(0.0));
	//float shadow = 1.0;
	//if ( texture( shadowMap, fragShadowCoord.xy ).r  <  fragShadowCoord.z){
	//	shadow = 0.5;
	//}
    outColor = vec4(.1,.1,.1,1.0) * vec4(fragColor,1.0) + shadow * 1.0 * vec4(fragColor,1.0) * clamp(dot(normalize(fragNormal), normalize(vec3(-1,-1,-1))),0.0,1.0);//vec4(fragColor,1.0);//texture(texSampler, fragTexCoord*2);
   //outColor = vec4(normalize(fragNormal),1.0);
}