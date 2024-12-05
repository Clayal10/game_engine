#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>
#include<thread>
#include<chrono>
#include<mutex>
#include "scolor.hpp"

#define _USE_MATH_DEFINES
#define GRAVITY 0.0001f
//#define GRAVITY 0.001f
#define M_PI 3.14159265

struct vertex {
	glm::vec3 pos;
	glm::vec2 tex_coord;

	bool operator==(const vertex& other) const {
		return pos == other.pos && tex_coord == other.tex_coord;
	}
}; // __attribute__((packed)); // This doesn't seem to matter

unsigned int load_texture(const char* filename);
int load_model(std::vector<vertex> &verticies, std::vector<uint32_t> &indices, const char* filename, float scale = 1.0f, bool swap_yz = false, glm::vec2 texture_scale = glm::vec2(1, 1));
GLuint make_program(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file);
GLuint make_shader(const char* filename, GLenum shaderType);
float randvel(float speed);
void init_helpers();
void free_helpers();

class gameobject {
	public:
		std::mutex data_mutex;
		unsigned int mvp_uniform, v_attrib, t_attrib, program, vbuf, cbuf, ebuf, tex, models_buffer;
		bool collision_check = false; // Consider separating out player from projectile collision here
		std::vector<glm::vec3> locations;
		glm::vec3 size; // What about non-square objects?
		virtual int init() { return 0; }
		virtual void deinit() {};
		virtual void draw(glm::mat4) {}
		virtual std::vector<glm::mat4> create_models();
		virtual void move(int elapsed_time) {}
		virtual void animate() {}
		virtual bool is_on_idx(glm::vec3 position, size_t index, float height) {return false;}
		virtual long is_on(glm::vec3 position, float height) {return -1;}
		virtual long collision_index(glm::vec3 position, float distance = 0) {
			return -1;
		}
		virtual glm::vec3 collision_normal(glm::vec3 move_to, glm::vec3 old_position, long index, float distance = 0) {
			return glm::vec3(0, 0, 0);
		}
		virtual bool collision_with_index(glm::vec3 position, size_t index, float distance = 0) { return false;}
		virtual void hit_index(size_t index) {}
};

class loaded_object : virtual  public gameobject {
public:
		const char *objectfile, *texturefile;
		float scale = 1.0f;
		glm::vec2 texture_scale = glm::vec2(1, 1);
		bool swap_yz = false;
		loaded_object(const char* of, const char* tf, glm::vec3 s) : objectfile(of), texturefile(tf) { size = s;	}
		
		int init() override ;
		void draw(glm::mat4 vp) override;
};


class block_object : virtual  public gameobject {
	public:
		bool is_on_idx(glm::vec3 position, size_t index, float height) override;
		long is_on(glm::vec3 position, float height) override;
		long collision_index(glm::vec3 position, float distance = 0) override;
		glm::vec3 collision_normal(glm::vec3 move_to, glm::vec3 old_position, long index, float distance = 0) override;
		bool collision_with_index(glm::vec3 position, size_t index, float distance = 0) override;
};


class wall_block : public loaded_object, public block_object {
public:
		wall_block(const char* of, const char* tf, glm::vec3 s) : loaded_object(of, tf, s) {
			collision_check = true;
		}
};

/* Global variables that are really all the way global */
#ifdef MAIN
#define EXTERN
#define INIT(X) = X
#else
#define EXTERN extern
#define INIT(x)
#endif

/* Player globals */
EXTERN glm::vec3 player_position;
EXTERN float player_heading;
EXTERN float player_height INIT(10);
EXTERN float player_elevation;
EXTERN float player_fall_speed INIT(0);
EXTERN gameobject *player_platform INIT(0);
EXTERN size_t player_platform_index INIT(0);
EXTERN bool player_fly INIT(0);

#include "hud.h"
EXTERN hud main_hud;
