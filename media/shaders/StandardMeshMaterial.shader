#sh VERTEX 
#version 430 core

#define NUM_LIGHTS 10

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 UV;

out VS_OUT
{
	//light stuff
	vec3 NWorld;
	vec4 WoldPos;
	vec4 ViewPos;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} vs_out;

//uniforms
uniform layout(location = 0) mat4 modelToWorldMatrix;
uniform layout(location = 1) mat4 invModelToWorldMatrix;

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

layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;


void main(void)
{
	vs_out.WoldPos = modelToWorldMatrix * position;
	vs_out.ViewPos = projData.worldViewMatrix * vs_out.WoldPos;
	vs_out.NWorld = mat3(transpose(invModelToWorldMatrix)) * normal;
	vs_out.UV = UV;
	vs_out.color = color;
	gl_Position = projData.projectionMatrix * vs_out.ViewPos;
	
}

#sh FRAGMENT 
#version 430 core
#define NUM_LIGHTS 10
in VS_OUT
{
	//light stuff
	vec3 NWorld;
	vec4 WoldPos;
	vec4 ViewPos;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} fs_in;

layout (location = 0) out vec4 color;

uniform layout(location = 0) mat4 modelToWorldMatrix;


uniform layout(location = 3) vec3 ambiant_light = vec3(0.2,0.2,0.2);
uniform layout(location = 4) vec3 diffuse_albedo = vec3(0.8,0.8,0.8);
uniform layout(location = 5) vec3 specular_albedo = vec3(0.7);
uniform layout(location = 6) float specular_power = 8.0;

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

layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;

vec3 ComputePointLight(int i, vec3 N, vec4 V)
{
	vec4 LDist  = lightData.lights[i].m_kPosition - fs_in.WoldPos;
	vec3 L = normalize(LDist).xyz;
	
	float attenuation = clamp((lightData.lights[i].m_fRange - abs(length(LDist))) / lightData.lights[i].m_fRange, 0, 1);
	
	vec3 R = mat3(projData.worldViewMatrix) * reflect(-L, N);
	vec3 diffuse = max(dot(N,L), 0.2) * lightData.lights[i].m_kLightColor.rgb * lightData.lights[i].m_fLightStrength;
	vec3 specular = pow(max(dot(R,V.xyz),0.0), specular_power) * lightData.lights[i].m_kLightColor.rgb * lightData.lights[i].m_fLightStrength;
		
	return (diffuse + specular) * attenuation;
}

vec4 ComputeLightColors()
{
	vec4 totalLight = vec4(0.1);
	
	vec3 N = normalize(fs_in.NWorld);
	vec4 V = normalize(fs_in.ViewPos);
	
	for(int i = 0; i < NUM_LIGHTS;++i)
	{
		if(lightData.lights[i].m_eLight == 0) continue;
		
		if(lightData.lights[i].m_eLight == 1)
		{
			totalLight.rgb += vec3(1);
		}
		else if(lightData.lights[i].m_eLight == 2)
		{
			totalLight.rgb += ComputePointLight(i, N, V);
		}
		else if(lightData.lights[i].m_eLight == 3)
		{
			totalLight.rgb += vec3(1);
		}
		
	}
	
	totalLight.rgb = clamp(totalLight.rgb,0,1);
	//totalLight = totalLight/NUM_LIGHTS;
	totalLight.a = 1;
	return totalLight;
}

void main(void)
{
	//normalise input
	/*vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L[0]);//vec3(1);//normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	
	//reflected quantity
	vec3 R = reflect(-L, N);
	
	//diffuse and specular quantity
	vec3 colorTexture = diffuse_albedo;//vec3(1.0,1.0,1.0);//texture(tex_object, fs_in.UV).rgb;  
	vec3 diffuse = max(dot(N,L), 0.2) * colorTexture;
	vec3 specular = pow(max(dot(R,V),0.0), specular_power) * specular_albedo;
	
	vec3 light = clamp((ambiant_light + diffuse + specular) * lightData.lights[0].m_kLightColor.xyz,0,1);*/
	
	//color = vec4(fs_in.tc, 1.0, 1.0);//texture(tex_object, fs_in.tc * vec2(3.0, 1.0));
	color = fs_in.color * ComputeLightColors();
}
