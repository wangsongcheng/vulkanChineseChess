#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1)uniform sampler2D inTex;

void main() {
	vec4 texColor = texture(inTex, inTexCoord);
	if(texColor.r == 0 && texColor.g == 0 && texColor.b == 0)
		discard;
//	if(texColor.r == 1 && texColor.g == 0 && texColor.b == 0)
//		discard;
//	outColor = vec4(.25, .25, .25, 1);
//	outColor = vec4(.7, .7, .7, 1);
//	outColor = vec4(texColor.r, .25, .5, 1);
	outColor = texColor;
//	outColor = vec4(texColor.r, texColor.g, texColor.b, 1);
}
