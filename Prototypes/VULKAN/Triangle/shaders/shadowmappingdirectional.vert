#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(binding = 0) uniform UniformBufferObject {
    mat4 MVP;
} uboShadows;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	gl_Position = uboShadows.MVP * vec4(inPosition, 1.0);
}