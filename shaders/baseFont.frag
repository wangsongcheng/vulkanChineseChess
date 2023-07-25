#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1)uniform sampler2D inTex;

void main() {
	vec4 texColor = texture(inTex, inTexCoord);
	if(texColor.r == 0 && texColor.g == 0 && texColor.b == 0)
		discard;
	outColor = texColor;
}
