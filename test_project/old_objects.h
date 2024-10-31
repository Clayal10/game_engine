#pragma once

class weirdbox : public gameobject {
	public:
		unsigned int mvp_uniform, anim_uniform, v_attrib, c_attrib, program, vbuf, cbuf, ebuf, acount;
		glm::mat4 anim;
		int init() override;
		void draw(glm::mat4 vp) override;
		void animate() override;
		void deinit() override;
};

class lightbox : public gameobject {
	public:
		unsigned int mvp_uniform, anim_uniform, v_attrib, c_attrib, program, vbuf, cbuf, ebuf;
		int init() override;
		void draw(glm::mat4 vp) override;
};

