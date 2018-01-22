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

// uniform vec4 camWorldPos; -- inutile directement extractible de viewProj

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

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

    gl_Position =  projData.projectionMatrix * projData.worldViewMatrix * interpolator.WorldPos;
}

#sh FRAGMENT 
out vec4 FragColor;

struct PBRLight
{
	vec3 L;
	vec3 H;
	vec3 radiance;
};

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 BRDF(PBRLight l, float roughness, vec3 N, vec3 V, vec3 F0)
{
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, l.H, roughness);   
	float G   = GeometrySmith(N, V, l.L, roughness);      
	vec3 F    = fresnelSchlick(clamp(dot(l.H, V), 0.0, 1.0), F0);
	   
	vec3 nominator    = NDF * G * F; 
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, l.L), 0.0);
	vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
	
	// kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - metallic;	  

	// scale light by NdotL
	float NdotL = max(dot(N, l.L), 0.0);        

	// add to outgoing radiance Lo
	return (kD * albedo / PI + specular) * l.radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}


vec3 ComputePBRLight(vec4 camPos)
{
	vec3 Lo = vec3(0);
	
	vec3 N = normalize(interpolator.WorldNormal);
	vec3 V = vec3(normalize(camPos - interpolator.WorldPos));
	
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	
	for(int i = 0; i < NUM_LIGHTS;++i)
	{
		if(lightData.lights[i].m_eLight == 0) continue;
		
		PBRLight thisLight;
		
		if(lightData.lights[i].m_eLight == 1)
		{
			thisLight.L = vec3(lightData.lights[i].m_kDirection);
			thisLight.radiance = vec3(lightData.lights[i].m_kLightColor) * lightData.lights[i].m_fLightStrength;
		}
		else if(lightData.lights[i].m_eLight == 2)
		{
			thisLight.L = vec3(lightData.lights[i].m_kPosition - interpolator.WorldPos);
			float oneOnRange = 1 / lightData.lights[i].m_fRange;
			float dist = length(thisLight.L);
			float attenuation = 1 / (1 + oneOnRange * dist * dist);
			thisLight.radiance = vec3(lightData.lights[i].m_kLightColor) * lightData.lights[i].m_fLightStrength * attenuation;
		}
		else if(lightData.lights[i].m_eLight == 3)
		{
			thisLight.L = vec3(lightData.lights[i].m_kDirection);
			thisLight.radiance = vec3(lightData.lights[i].m_kLightColor)  * lightData.lights[i].m_fLightStrength; // TODO attenuation
		}
		
		thisLight.L = normalize(thisLight.L);
		thisLight.H = normalize(V+thisLight.L);
		
		Lo += BRDF(thisLight, roughness, N, V, F0);
	}
	
	vec3 ambient = vec3(0.03) * albedo * ao;
	
	vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 
	return color;
}

void main()
{
	vec4 camPos = -projData.worldViewMatrix[3];
	
	FragColor = vec4(ComputePBRLight(camPos), 1);
}