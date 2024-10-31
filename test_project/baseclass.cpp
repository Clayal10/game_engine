#include "game.h"

std::vector<glm::mat4> gameobject::create_models(){
	std::vector<glm::mat4> models;
	data_mutex.lock();
	models.reserve(locations.size());
	for(glm::vec3 l : locations){
		glm::mat4 new_model = glm::mat4(1.0f);
		new_model = translate(new_model, l);
		models.push_back(new_model);
	}
	data_mutex.unlock();
	return models;
}

int loaded_object::init() {
	// Initialization part
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices; // Consider unified terminology

	load_model(vertices, indices, objectfile, scale, swap_yz, texture_scale);

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// TODO:  Remember to explain the layout later

	glGenBuffers(1, &ebuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &models_buffer);

	tex = load_texture(texturefile);

	program = make_program("loaded_object_vertex_shader.glsl",0, 0, 0, "loaded_object_fragment_shader.glsl");
	if (!program)
		return 1;

	v_attrib = glGetAttribLocation(program, "in_vertex");
	t_attrib = glGetAttribLocation(program, "in_texcoord");
	mvp_uniform = glGetUniformLocation(program, "vp");
	return 0;
}

void loaded_object::draw(glm::mat4 vp) {
	glUseProgram(program);
	std::vector<glm::mat4> models = create_models();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, models_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, models_buffer);

	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 3, GL_FLOAT, GL_FALSE, 20, 0);

	glEnableVertexAttribArray(t_attrib);
	glVertexAttribPointer(t_attrib, 2, GL_FLOAT, GL_FALSE, 20, (const void*)12);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	int size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

	glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0, models.size());
}

bool block_object::is_on_idx(glm::vec3 position, size_t index, float height){
	return (0.0f < (position.y - locations[index].y) && 
			1.0f > (position.y - height) - (locations[index].y + size.y/2) &&
			size.x/2 > fabs(position.x - locations[index].x) && 
			size.z/2 > fabs(position.z - locations[index].z));

}

ssize_t block_object::is_on(glm::vec3 position, float height) {
	for(size_t i = 0; i < locations.size(); i++)
		if(is_on_idx(position, i, height))
			return i;
	return -1;
}

ssize_t block_object::collision_index(glm::vec3 position, float distance){
	for(size_t i = 0; i < locations.size(); i++){
		glm::vec3 l = locations[i]; // This'll get optimized out
		if(	size.x/2.0f + distance > abs(l.x-position.x) && 
				size.y/2.0f + distance > abs(l.y-position.y) && 
				size.z/2.0f + distance > abs(l.z-position.z)){
			return i;	
		}
	}
	return -1;
}

/* Leaving y for later, so we finish today */
glm::vec3 block_object::collision_normal(glm::vec3 move_to, glm::vec3 old_position, ssize_t index, float distance){
	glm::vec3 l = locations[index]; // This'll get optimized out
	if(	old_position.z > l.z + (size.z/2 + distance) &&
			old_position.x >= l.x - (size.x/2 + distance) &&
			old_position.x <= l.x + (size.x/2 + distance)){
		return glm::vec3(0, 0, 1);
	}
	if(	old_position.z < l.z - (size.z/2 + distance) &&
			old_position.x >= l.x - (size.x/2 + distance) &&
			old_position.x <= l.x + (size.x/2 + distance)){
		return glm::vec3(0, 0, -1);
	}
	if(	old_position.x < l.x - (size.x/2 + distance) &&
			old_position.z >= l.z - (size.z/2 + distance) &&
			old_position.z <= l.z + (size.z/2 + distance)){
		return glm::vec3(1, 0, 0);
	}
	if(	old_position.x > l.x + (size.x/2 + distance) &&
			old_position.z >= l.z - (size.z/2 + distance) &&
			old_position.z <= l.z + (size.z/2 + distance)){
		return glm::vec3(-1, 0, 0);
	}
	puts("Ended collision normal without returning");
	return glm::vec3(0, 0, 0);
}

bool block_object::collision_with_index(glm::vec3 position, size_t index, float distance){	
	glm::vec3 l = locations[index]; // This variable will get optimized out
	if(size.x/2.0f + distance > abs(l.x-position.x) && 
			size.y/2.0f + distance > abs(l.y-position.y) &&
			size.z/2.0f + distance > abs(l.z-position.z)){
		return true;
	}
	return false;
}

