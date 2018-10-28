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

layout (location = 0)out VertexData
{
	vec3 fragColor;
	vec2 fragTexCoord;
	vec3 fragNormal;
	vec4 fragShadowCoord;
	vec4 fragViewDir;
	mat3 TBN;
} outVertexData;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inTexCoord;


//layout(location = 0) out vec3 fragColor;
//layout(location = 1) out vec2 fragTexCoord;
//layout(location = 2) out vec3 fragNormal;
//layout (location = 3) out vec4 fragShadowCoord;
//layout (location = 4) out vec4 fragViewDir;
//layout (location = 5) out mat3 TBN;


const float PI = 3.1415926535897932384626433832795;
const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

const vec4 WaveA = vec4(1,0,.5,10); // dir, steepness, Wavelength
const vec4 WaveB = vec4(0,1,.25,20); // dir, steepness, Wavelength
const vec4 WaveC = vec4(1,1,.15,10); // dir, steepness, Wavelength
const vec4 WaveD = vec4(-1,1,.15,10); // dir, steepness, Wavelength
const vec4 WaveE = vec4(-1,0,.1,1); // dir, steepness, Wavelength
const vec4 WaveF = vec4(0,1,.1,1); // dir, steepness, Wavelength

vec3 GertznerWave(vec4 wave, vec3 p, inout vec3 tangent, inout vec3 binormal)
{
	//Compute quantities
	float steepness = wave.z;
	float wavelength = wave.w;
	float k = 2 * PI / wavelength;
	float c = sqrt(9.8/k);
	vec2 d = normalize(wave.xy);
	float f = k * (dot(d,p.xz) - c * ubo.time/5);
	float a = steepness/k;

	float dist = length(p);
	a = clamp(mix(0, a, dist / 200), 0, a);

	//compute the normal
	float cosf = cos(f); float sinf = sin(f);
	float dxx = - d.x * d.x * (steepness * sinf);
	float dyy = -d.y * d.y * (steepness * sinf);
	float dxy = -d.x * d.y * (steepness * sinf);
	tangent += vec3(dxx, d.x * steepness * cosf, dxy);
	binormal += vec3(dxy, d.y * steepness * cosf, dyy);
	return vec3(d.x *(a*cosf), a*sinf, d.y *(a*cosf));
}

void main() 
{
	//get world position
	vec3 worldPosition = vec3(ubo.model * vec4(inPosition, 1.0));
	vec3 tangent = vec3(1,0,0);
	vec3 binormal = vec3(0,0,1);
	worldPosition += GertznerWave(WaveA, worldPosition, tangent, binormal);
	worldPosition += GertznerWave(WaveB, worldPosition, tangent, binormal);
	worldPosition += GertznerWave(WaveC, worldPosition, tangent, binormal);
	worldPosition += GertznerWave(WaveD, worldPosition, tangent, binormal);
	worldPosition += GertznerWave(WaveE, worldPosition, tangent, binormal);
	worldPosition += GertznerWave(WaveF, worldPosition, tangent, binormal);
	vec3 worldNormal = normalize(cross(binormal, tangent));

	//standard suff
    gl_Position = ubo.proj * ubo.view * vec4(worldPosition, 1.0);
    outVertexData.fragColor = inColor;
    outVertexData.fragTexCoord = inTexCoord;
	outVertexData.fragNormal = normalize(ubo.proj * ubo.view * vec4(worldNormal,0.0)).xyz;

	outVertexData.fragShadowCoord = (biasMat * ubo.lightSpace) * vec4(inPosition, 1.0);

	vec3 bitangent = cross(inNormal, inTangent);
	outVertexData.TBN = mat3(inTangent, bitangent, inNormal);

	//vompute the view dir
	outVertexData.fragViewDir = vec4(worldPosition,1) - vec4(213,120,-8,1);
	outVertexData.fragViewDir = ubo.proj * ubo.view * outVertexData.fragViewDir;
}