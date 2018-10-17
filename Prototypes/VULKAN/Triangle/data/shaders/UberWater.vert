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


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout (location = 3) out vec4 fragShadowCoord;
layout (location = 4) out mat3 TBN;

const float PI = 3.1415926535897932384626433832795;
const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

const float Wavelength = 10;
const float Steepness = .5;
const vec2 Direction = vec2(1,1);
const vec4 WaveA = vec4(1,1,.5,20); // dir, steepness, Wavelength
const vec4 WaveB = vec4(0,1,.25,40); // dir, steepness, Wavelength

vec3 GertznerWave(vec4 wave, vec3 p, inout vec3 tangent, inout vec3 binormal)
{
	float steepness = wave.z;
	float wavelength = wave.w;
	float k = 2 * PI / wavelength;
	float c = sqrt(9.8/k);
	vec2 d = normalize(wave.xy);
	float f = k * (dot(d,p.xz) - c * ubo.time);
	float a = steepness/k;
	//worldPosition.x += d.x *(a*cos(f));
	//worldPosition.y = a*sin(f);
	//worldPosition.z += d.y *(a*cos(f));

	//compute the normal
	tangent += vec3(- d.x * d.x * (steepness * sin(f)), d.x * steepness * cos(f), -d.x * d.y * (steepness * sin(f)));
	binormal += vec3(d.x * d.y * (steepness * sin(f)), d.y * steepness * cos(f), -d.y * d.y * (steepness * sin(f)));
	return vec3(d.x *(a*cos(f)), a*sin(f), d.y *(a*cos(f)));
}

void main() 
{
	//get world position
	vec3 worldPosition = vec3(ubo.model * vec4(inPosition, 1.0));
	vec3 tangent = vec3(1,0,0);
	vec3 binormal = vec3(0,0,1);
	worldPosition += GertznerWave(WaveA, worldPosition, tangent, binormal);
	worldPosition += GertznerWave(WaveB, worldPosition, tangent, binormal);
	vec3 worldNormal = normalize(cross(binormal, tangent));

	//compute the position as a gertzner wave
	//float k = 2 * PI / Wavelength;
	//float c = sqrt(9.8/k);
	//vec2 d = normalize(Direction);
	//float f = k * (dot(d,worldPosition.xz) - c * ubo.time);
	//float a = Steepness/k;
	//worldPosition.x += d.x *(a*cos(f));
	//worldPosition.y = a*sin(f);
	//worldPosition.z += d.y *(a*cos(f));

	//compute the normal
	//vec3 tangent = vec3(1 - d.x * d.x * (Steepness * sin(f)), d.x * Steepness * cos(f), -d.x * d.y * (Steepness * sin(f)));
	//vec3 binormal = vec3(d.x * d.y * (Steepness * sin(f)), d.y * Steepness * cos(f), 1-d.y * d.y * (Steepness * sin(f)));
	//vec3 worldNormal = normalize(cross(binormal, tangent));

	//standard suff
    gl_Position = ubo.proj * ubo.view * vec4(worldPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
	fragNormal = normalize(ubo.proj * ubo.view * vec4(worldNormal,0.0)).xyz;

	fragShadowCoord = (biasMat * ubo.lightSpace) * vec4(inPosition, 1.0);

	vec3 bitangent = cross(inNormal, inTangent);
	TBN = mat3(inTangent, bitangent, inNormal);
}