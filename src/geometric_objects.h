#pragma once

/* TODO:
 * 	Normal Vector for panel to set orientation
 * 	Collision detection (can't move through panel)
 */
class flat_panel : public gameobject {
public:
	std::vector<glm::vec2> object_scales, texture_scales;
	unsigned scales_buffer;
	const char *texturefile;
	flat_panel(const char* tf) : texturefile(tf) {}
	int init() override;
	void draw(glm::mat4 vp) override;
	std::vector<glm::mat4> create_models();
	void addpanel(glm::vec3 location, glm::vec2 object_scale, glm::vec2 texture_scale);
};

