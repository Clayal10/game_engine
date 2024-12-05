#define MAIN
#include "game.h"

#include "activation_area.h"
#include "projectiles.h"
#include "tile_floor.h"
#include "moving_platform.h"
#include "turret.h"
#include "geometric_objects.h"
#include "hud.h"
#include "pathway.h"
#include "bot.h"

#include<fstream>
#include<sstream>

std::mutex grand_mutex;

float height = 1600;
float width = 2550;

/* Global section */
int time_resolution = 10;
int framecount = 0;
std::vector<gameobject*> objects;
projectile ice_balls;
fragment brick_fragments;
hud main_hud;
Treat treat;

class target : public loaded_object, public block_object {
	public:
		target() : loaded_object("monkey.obj", "brick.jpg", glm::vec3(15.0f, 10.0f, 15.0f)) {
			collision_check = true;
		}
		void hit_index(size_t index){
			brick_fragments.create_burst(100, locations[index], 0.01f);
			locations.erase(locations.begin() + index);
		}

};

long is_empty(glm::vec3 position, float distance = 0.2f){
	for(gameobject* o : objects) {
		long collide_index = o->collision_index(position, distance);
		if(collide_index != -1)
			return false;
	}
	return true;

}


struct key_status {
	int forward, backward, left, right, up, down;
};
struct key_status player_key_status;

void fire(bool burst = false){
	if(burst)
		ice_balls.add_projectile(player_position, player_heading, player_elevation, 0.4f, 10000.0f, 1.0f, true);
	else
		ice_balls.add_projectile(player_position, player_heading, player_elevation, 0.05f, 100000.0f, 1.0f, false);

}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods){
	if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		/*This will require the most work.*/
		// Spawn a block
		glm::vec3 look_at_point = 2.0f * glm::vec3(roundf(player_position.x/2.0f), roundf(player_position.y/2.0f), roundf(player_position.z/2.0f)) ;
		look_at_point.x += 2.0f * roundf(4.0f * cosf(player_elevation) * sinf(player_heading));
		look_at_point.y += 2.0f * roundf(4.0f * sinf(player_elevation));
		look_at_point.z += 2.0f * roundf(4.0f * cosf(player_elevation) * cosf(player_heading));
		if(!is_empty(look_at_point, 0.0f)){
			puts("placed object? BAD");
			
			/*
			look_at_point = 2.0f * glm::vec3(roundf(player_position.x/2.0f), roundf(player_position.y/2.0f), roundf(player_position.z/2.0f)) ;
			look_at_point.x += 2.0f * roundf(2.0f * cosf(player_elevation) * sinf(player_heading));
			look_at_point.y += 2.0f * roundf(2.0f * sinf(player_elevation));
			look_at_point.z += 2.0f * roundf(2.0f * cosf(player_elevation) * cosf(player_heading));
			*/
		}
		if(is_empty(look_at_point, 0.0f)){
			look_at_point.y = -10;

			treat.locations.push_back(look_at_point);
			puts("placed");
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	
	if(GLFW_KEY_W == key && 1 == action){
		player_key_status.forward = 1;
	}	
	else if(GLFW_KEY_W == key && 0 == action){
		player_key_status.forward = 0;
	}	
	if(GLFW_KEY_S == key && 1 == action){
		player_key_status.backward = 1;
	}	
	else if(GLFW_KEY_S == key && 0 == action){
		player_key_status.backward = 0;
	}
	if(GLFW_KEY_A == key)
		player_key_status.left = action;
	if(GLFW_KEY_D == key)
		player_key_status.right = action;
	/*for the game, will want to disable jumping and running, but will be fine through project 4 probably*/
	
	if(GLFW_KEY_SPACE == key){	
		
		if(player_platform || player_position.y < 0.1f + player_height){
			player_fly = 0;
			//player_fall_speed = 0.04f;
			player_fall_speed = 0.2f;

			player_position.y += 1.0f;
			player_platform = 0;
		}
	} // may need to check and make sure shift and space aren't pressed at the same time
	if(GLFW_KEY_LEFT_CONTROL == key){
		player_key_status.up = action;
		player_fall_speed = 0.0f;
		player_fly = 1;
	}
	if(GLFW_KEY_LEFT_SHIFT == key){
		if(!player_platform)
			player_key_status.down = action;
	}
}

int shutdown_engine = 0;
/* Must be called at a consistent rate */
void player_movement(){
	while(!shutdown_engine){
		//		grand_mutex.lock();
		auto start = std::chrono::system_clock::now();
		glm::vec3 step_to_point = player_position;
		if(player_key_status.forward){
			step_to_point += 0.1f * glm::vec3(sinf(player_heading), 0, cosf(player_heading));
			//step_to_point += 0.6f * glm::vec3(sinf(player_heading), 0, cosf(player_heading));

		}
		if(player_key_status.backward){
			step_to_point += 0.1f * glm::vec3(-sinf(player_heading), 0, -cosf(player_heading));
			//step_to_point += 0.6f * glm::vec3(-sinf(player_heading), 0, -cosf(player_heading));

		}
		if(player_key_status.left){
			step_to_point += 0.05f * glm::vec3(sinf(player_heading + M_PI/2), 0, cosf(player_heading + M_PI/2));
			//step_to_point += 0.4f * glm::vec3(sinf(player_heading + M_PI / 2), 0, cosf(player_heading + M_PI / 2));

		}
		if(player_key_status.right){
			step_to_point += 0.05f * glm::vec3(-sinf(player_heading + M_PI/2), 0, -cosf(player_heading + M_PI/2));
			//step_to_point += 0.4f * glm::vec3(-sinf(player_heading + M_PI / 2), 0, -cosf(player_heading + M_PI / 2));

		}
		if(player_key_status.up){
			step_to_point += 0.05f * glm::vec3(0, 1, 0);
			//step_to_point += 0.3f * glm::vec3(0, 1, 0);

		}
		if(player_key_status.down){
			step_to_point += 0.05f * glm::vec3(0, -1, 0);
			//step_to_point += 0.3f * glm::vec3(0, -1, 0);

		}
		for(gameobject* o : objects) {
			long collide_index = o->collision_index(step_to_point, 0.2f);
			if(collide_index != -1) {
				if(is_empty(glm::vec3(player_position.x, step_to_point.y, step_to_point.z), 0.2f)) {
					step_to_point.x = player_position.x;
					break;
				}
				else if(is_empty(glm::vec3(step_to_point.x, step_to_point.y, player_position.z), 0.2f)) {
					step_to_point.z = player_position.z;
					break;
				}
				else if(is_empty(glm::vec3(step_to_point.x, step_to_point.y, player_position.z), 0.2f)) {
					step_to_point.y = player_position.y;
					break;
				}
				else {
					step_to_point = player_position;
					break;
				}


			}
		}
		player_position = step_to_point;

		if(player_platform){
			if(!player_platform->is_on_idx(player_position, player_platform_index, player_height))
				player_platform = 0;
		} else {
			float floor_height = -10;
			for(gameobject* o : objects) {
				long ppi = o->is_on(player_position, player_height);
				if(ppi != -1) {
					player_platform_index = ppi;
					player_platform = o;	
					floor_height = player_platform->locations[player_platform_index].y + (player_platform->size.y / 2);
					player_fall_speed = 0;
					player_position.y = floor_height + player_height; 
				}
			}
			if(player_position.y - player_height > floor_height) {
				player_position.y += player_fall_speed;
				if(!player_fly)
					player_fall_speed -= GRAVITY;
			} else {
				player_fall_speed = 0;
				player_position.y = floor_height + player_height; 
			}
		}
		main_hud.lprintf(2, "Player Position:  (%f, %f, %f)", player_position.x, player_position.y, player_position.z);
		//		grand_mutex.unlock();
		auto end = std::chrono::system_clock::now();
		//		double difference = std::chrono::duration_cast<std::chrono::milliseconds>(start - end).count();
		//		printf("Time difference:  %lf\n", difference);
		std::this_thread::sleep_for(std::chrono::microseconds(1000) - (start - end));
	}
}

void object_movement(){
	int loop_time;
	auto last_call = std::chrono::system_clock::now(); 
	while(!shutdown_engine){
		auto start_time = std::chrono::system_clock::now();
		loop_time = std::chrono::duration_cast<std::chrono::microseconds>(start_time - last_call).count();

		//		grand_mutex.lock();
		if(player_platform){
			glm::vec3 pltloc = player_platform->locations[player_platform_index];
			float floor_height = pltloc.y + (player_platform->size.y / 2);
			player_position.y = floor_height + player_height;
		}
		//		grand_mutex.unlock();
		for(gameobject* o : objects)
			o->move(loop_time); // move needs a parameter which indicates how long since we last called move (loop_time)
		auto end_time = std::chrono::system_clock::now();
		int cpu_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
		last_call = start_time;
		int sleep_time = 1000 - cpu_time;
		//		printf("Loop time:  %d		Sleep time:  %d	CPU time:  %d\n", loop_time, sleep_time, cpu_time);
		if(sleep_time > 100 )
			std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}
}

void animation(){
	while(!shutdown_engine){
		auto start = std::chrono::system_clock::now();
		for(gameobject* o : objects)
			o->animate();
		auto end = std::chrono::system_clock::now();
		//		double difference = std::chrono::duration_cast<std::chrono::milliseconds>(start - end).count();
		//		printf("Time difference:  %lf\n", difference);
		std::this_thread::sleep_for(std::chrono::microseconds(10000) - (start - end));
	}
}

void collision_detection(){
	while(!shutdown_engine){
		auto start = std::chrono::system_clock::now();
		ice_balls.data_mutex.lock();
		size_t nproj = ice_balls.locations.size();
		for(size_t proj_index = 0; proj_index < nproj; proj_index++){
			glm::vec3 l = ice_balls.locations[proj_index];
			for(auto o : objects){
				if(o->collision_check){
					long index = o->collision_index(l);
					if(index != -1) {
						o->hit_index(index);
						ice_balls.hit_index(proj_index);
						break;
					}
				}
			}
		}	
		ice_balls.data_mutex.unlock();
		auto end = std::chrono::system_clock::now();
		//		double difference = std::chrono::duration_cast<std::chrono::milliseconds>(start - end).count();
		//		printf("Time difference:  %lf\n", difference);
		std::this_thread::sleep_for(std::chrono::microseconds(1000) - (start - end));
	}
}
void pos_callback(GLFWwindow* window, double xpos, double ypos){
	double center_x = width/2;
	double diff_x = xpos - center_x;
	double center_y = height/2;
	double diff_y = ypos - center_y;
	glfwSetCursorPos(window, center_x, center_y);
	player_heading -= diff_x / 1000.0; // Is this too fast or slow?
	player_elevation -= diff_y / 1000.0;
}

void resize(GLFWwindow*, int new_width, int new_height){
	width = new_width;
	height = new_height;
	printf("Window resized, now %f by %f\n", width, height);
	glViewport(0, 0, width, height);
}


target targets;
void bob(){
	static bool bob_happened = false;
	if(!bob_happened){
		bob_happened = true;
		targets.locations.push_back(glm::vec3(-10, 5, 10));
	}
};

turret t;
void activate_turret(){
	t.active = !t.active;
}

void debug_callback(GLenum, GLenum, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*) {
	if(severity == GL_DEBUG_SEVERITY_HIGH)
		puts(RED(message).c_str());
	else if(severity == GL_DEBUG_SEVERITY_MEDIUM)
		puts(YELLOW(message).c_str());
	/* Only uncomment if you want a lot of messages */
	/*
		else
		puts(message);
		*/
}


int main(int argc, char** argv) {
	init_helpers();
	if(!glfwInit()) {
		puts(RED("GLFW init error, terminating\n").c_str());
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(width, height, "Simple OpenGL 4.0+ Demo", 0, 0);
	if(!window){
		puts(RED("Window creation failed, terminating\n").c_str());
		return 1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glewInit();

	unsigned supported_threads = std::thread::hardware_concurrency();
	printf("Supported threads:  %u\n", supported_threads);

	/* Set up callbacks */
	glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(debug_callback, 0);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, pos_callback);
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetMouseButtonCallback(window, mouse_click_callback);

	/* Set starting point */
	player_position = glm::vec3(10, 10, 500);
	player_heading = M_PI;


	/* Level Loading (hardcoded at the moment) */

	tile_floor fl;
	objects.push_back(&ice_balls);
	objects.push_back(&fl);
	/*Maze path*/
	pathway path;
	pathway_end path_end;
	//trigger_block start_block;
	//trigger_block end_block
	path_walls walls;
	
	std::ifstream fp;
	fp.open("maze.txt");

	if(!fp){
		puts(RED("Maze could not be opened!\n").c_str());
		exit(-1);
	}
	std::string buffer;
	glm::vec3 place = glm::vec3(0, 0, 0);
	while(getline(fp, buffer, '\n')){
		//std::stringstream s(buffer);
		for(char c : buffer){
			if(c == '#'){
				walls.locations.push_back(place + glm::vec3(0, 10, 0));
			}else if(c == ' '){
				path.locations.push_back(place + glm::vec3(0, -10, 0));
			}else if(c == 'b'){
				path_end.locations.push_back(place + glm::vec3(0, -10, 0));
			}
			/*
			else if(c == 's'){
				start_block.locations.push_back(place + glm::vec3(0, -10, 0));
				start_block.start_trigger();
			}else if(c == 'e'){
				end_block.locations.push_back(place + glm::vec3(0, -10, 0));
				end_block.end_trigger();
			}
			*/
			else{
				puts(RED("Invalid character in maze file!\n").c_str());
				return -1;
			}
			place.x += 10;
		}
		place.z += 10;
		place.x = 0;
	}
	fp.close();
	/*Cat Bot*/
	bot cat_bot(100, path_end.locations[0], path.locations);
	cat_bot.vision = &ice_balls;
	cat_bot.treat = &treat;

	objects.push_back(&path_end);
	objects.push_back(&path);
	//objects.push_back(&start_block);
	//objects.push_back(&end_block);

	
	//walls.create_walls(&path, &path_end,  glm::vec3(10, 0, 70), 14);
	
	
	objects.push_back(&walls);

	aimpoint main_aimpoint;
	objects.push_back(&main_aimpoint);
	objects.push_back(&main_hud);
	objects.push_back(&treat);
	objects.push_back(&cat_bot);

	/* Initialize game objects */
	for(gameobject* o : objects){
		if(o->init()){
			puts(RED("Compile Failed, giving up!").c_str());
			return 1;
		}
	}
	
	main_hud.set_text("\n\n");

	/* Start Other Threads */
	std::thread player_movement_thread(player_movement);
	std::thread object_movement_thread(object_movement);
	std::thread animation_thread(animation);
	std::thread collision_detection_thread(collision_detection);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		framecount++;
		glfwPollEvents();
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		//		grand_mutex.lock();

		/* Where are we?  A:  player_position
		 * What are we looking at?
		 */
		glm::vec3 look_at_point = player_position;
		look_at_point.x += cosf(player_elevation) * sinf(player_heading);
		look_at_point.y += sinf(player_elevation);
		look_at_point.z += cosf(player_elevation) * cosf(player_heading);
		glm::mat4 view = glm::lookAt(player_position, look_at_point, glm::vec3(0, 1, 0));
		glm::mat4 projection = glm::perspective(45.0f, width / height, 0.1f, 10000.0f);
		glm::mat4 vp = projection * view;

		for(gameobject* o : objects)
			o->draw(vp);
		//		grand_mutex.unlock();

		glfwSwapBuffers(window);
	}
	shutdown_engine = 1;
	player_movement_thread.join();
	// TODO:  join other threads
	glfwDestroyWindow(window);
	glfwTerminate();
	free_helpers();
}


