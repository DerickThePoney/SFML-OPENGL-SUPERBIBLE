#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(quads, equal_spacing, cw) in;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inNormal[];
layout(location = 4) in vec2 inTexCoord[];

layout(location = 0) out vec3 outNormal;

layout(binding = 6) uniform sampler2D displacementField;
layout(binding = 7) uniform sampler2D worldNormal;

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 lightSpace;
	float time;
} ubo;

void main() 
{
	// Interpolate positions
	vec4 pos1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 pos2 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
	gl_Position = ubo.model * mix(pos1, pos2, gl_TessCoord.y);
	
	// Interpolate normal
	/*vec3 n1 = mix(inNormal[0], inNormal[1], gl_TessCoord.x);
	vec3 n2 = mix(inNormal[3], inNormal[2], gl_TessCoord.x);
	outNormal = mix(n1, n2, gl_TessCoord.y);*/

	// Interpolate uv
	vec2 uv1 = mix(inTexCoord[0], inTexCoord[1], gl_TessCoord.x);
	vec2 uv2 = mix(inTexCoord[3], inTexCoord[2], gl_TessCoord.x);
	vec2 uv = mix(uv1, uv2, gl_TessCoord.y);

	gl_Position = gl_Position + texture(displacementField, uv).xyzw;
	outNormal = normalize(texture(worldNormal, uv).xyz);
}