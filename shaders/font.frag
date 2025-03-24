#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2  inTexCoord;
layout(location = 1) in float inImageIndex;

layout(location = 0) out vec4 outColor;

layout(binding = 1)uniform sampler2DArray inTex;

void main() {
	vec4 texColor = texture(inTex, vec3(inTexCoord, inImageIndex));
	if(texColor.r == 0 && texColor.g == 0 && texColor.b == 0)
		discard;
	texColor.y = texColor.x;
	texColor.z = texColor.x;
	texColor.a = texColor.x;
	outColor = texColor;
}
