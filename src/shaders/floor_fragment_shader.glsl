#version 430

in vec2 f_texcoord;
out vec4 outcolor;
uniform sampler2D tex;

void main(void) {
	outcolor = texture(tex, f_texcoord);
}
