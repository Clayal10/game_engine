#version 430

in vec3 in_vertex;
in vec2 in_texcoord;
uniform mat4 mvp;
out vec4 fcolor;
//out vec4 gl_Position;
out vec2 f_texcoord;

void main(void) {	
	int x_offset = 2 * (gl_InstanceID / 100) - 100;
	int z_offset = 2 * (gl_InstanceID % 100) - 100;
	vec4 instance_point = vec4(in_vertex.x + float(x_offset), in_vertex.y, in_vertex.z + float(z_offset), 1.0);
	instance_point.xz *= 10;
	gl_Position = mvp * instance_point;
	f_texcoord = in_vertex.xz;
}
