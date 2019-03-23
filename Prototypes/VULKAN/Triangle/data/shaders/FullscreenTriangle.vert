#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 4) out vec2 outTexCoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main(void)
{
	float x = -1.0 + float((gl_VertexIndex & 1) << 2);
    float y = 1.0 - float((gl_VertexIndex & 2) << 1);
    outTexCoord.x = (x+1.0)*0.5;
    outTexCoord.y = (y+1.0)*0.5;
    gl_Position = vec4(x, y, 0, 1);
}