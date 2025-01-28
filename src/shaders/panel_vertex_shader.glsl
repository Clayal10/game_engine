#version 460

layout(packed, binding=0) buffer model_list {
	mat4 models[];
};
layout(packed, binding=1) buffer scale_list {
	vec2 scales[];
};
in vec3 in_vertex;
in vec2 in_texcoord;
uniform mat4 vp;
out vec2 frag_texcoord;
out vec4 gl_Position;

void main(void) {	
	gl_Position = vp * models[gl_InstanceID] * vec4(in_vertex, 1.0);
	frag_texcoord = (vec2(in_vertex.x, in_vertex.z) * 0.5 + vec2(0.5, 0.5)) * scales[gl_InstanceID];
}
