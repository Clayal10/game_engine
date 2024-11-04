#include "game.h"
#include "geometric_objects.h"

int flat_panel::init()  {
	// Initialization part
	float vertices[] = {	
		-1.0, 0,  1.0,
		1.0, 0,  1.0,
		1.0,  0,  -1.0,
		-1.0, 0,  1.0,
		1.0, 0,  -1.0,
		-1.0, 0,  -1.0,
	};
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	program = make_program("panel_vertex_shader.glsl",0, 0, 0, "loaded_object_fragment_shader.glsl");
	if (!program)
		return 1;

	tex = load_texture(texturefile);

	glGenBuffers(1, &models_buffer);
	glGenBuffers(1, &scales_buffer);

	v_attrib = glGetAttribLocation(program, "in_vertex");
	mvp_uniform = glGetUniformLocation(program, "vp");
	return 0;
}

void flat_panel::addpanel(glm::vec3 location, glm::vec2 object_scale, glm::vec2 texture_scale){
	locations.push_back(location);
	object_scales.push_back(object_scale);
	texture_scales.push_back(texture_scale);

}

std::vector<glm::mat4> flat_panel::create_models(){
	std::vector<glm::mat4> models;
	models.reserve(locations.size());
	for(size_t i = 0; i < locations.size(); i++){
		glm::mat4 new_model = glm::mat4(1.0f);
		new_model = glm::translate(new_model, locations[i]);
		new_model = glm::scale(new_model, glm::vec3(object_scales[i].x, 1.0f, object_scales[i].y));
		models.push_back(new_model);
	}
	return models;
}

void flat_panel::draw(glm::mat4 vp)  {
	glUseProgram(program);
	data_mutex.lock();
        std::vector<glm::mat4> models = create_models();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, models_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, models_buffer);
        
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scales_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, texture_scales.size() * sizeof(glm::vec2), texture_scales.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scales_buffer);
	data_mutex.unlock();

	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, models.size());
}
