#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(push_constant) uniform uPushConstant {
	mat4 projection;
} pc;

layout(binding = 0)uniform UniformBlock {
	mat4 model;
}mubo;

void main() {
	outColor = inColor;
	gl_Position = pc.projection * mubo.model * vec4(inPos, 1.0);
}
