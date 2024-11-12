#pragma once
/*
	This bot only stays on the pathway
	Need to make .obj file
		Using cat, may need to check collision since the location is in "front" of the cat
*/
#include "game.h"
#include "pathway.h"
#include <math.h>
#include <algorithm>

bool operator==(const glm::vec3 &one, const glm::vec3 &two){
	if(one.z == two.z && one.x == two.x)
		return true;
	return false;
}

class bot : public loaded_object{
public:
	float rotation = 0; // just assuming this is in degrees from 0 (looking -z)
	double time_buff;
	int rotation_state = 0; // starts looking -z
	std::vector<glm::vec3> to_start; // list of when it needed to make a choice
	std::vector<glm::vec3> explored;
	/*rotate_dir key:
	 * 	0: -z straight
	 * 	1: +x right
	 * 	2: -x left
	 * 	3: +z back
	 */
	float bot_speed = 0.025f;
	float alive = false;
	float moving = 0;

	bot() : loaded_object("cat.obj", "Cat_bump.jpg", glm::vec3(5, 5, 5)) {
		scale = 0.25f;
		swap_yz = true;
		//to_start.push_back(glm::vec3(0, 0, 200));
		//explored.push_back(glm::vec3(0, 0, 200));
	}

	pathway path;

	void kill_cat(){
		locations.erase(locations.begin());
		alive = false;
		to_start.clear();
		explored.clear();

	}

	float x_buff, z_buff;

	void move(int elapsed_time){
		//TODO Set up conditionals for hitting walls. TODO the AI
		/*heading of M_PI is towards -z: may or may not be useful
		 *	right is M_PI/2
		 *	left is 3/2*M_PI
		 * */
		//path_end is at x=0 z=70
		if(alive){
			//check if at finish spot
			if(locations[0].x == 0 && locations[0].z == 70){
				locations[0].y += 0.5f;
				if(locations[0].y > 200){
					kill_cat();
				}	
			}
			else if(moving > 0){	
				move_(rotation_state); 
			}
			else{//if not finished with the maze, move; going to be a follow the left wall kinda dealio
				/*USE ROUNDF TO ROUND LOCATIONS TO NEAREST INTEGER*/
				locations[0].x = roundf(locations[0].x);
				locations[0].z = roundf(locations[0].z);
				printf("X: %f Z: %f Face: %f\n", locations[0].x, locations[0].z, rotation);
				
				to_start.push_back(locations[0]); // we will pop off if needed here
				explored.push_back(locations[0]); // we won't remove any glm::vec3 nodes
				//check every location around and move if unexplored
				for(long unsigned int i = 0; i<path.locations.size(); i++){
					if(path.locations[i] == locations[0] + glm::vec3(0, -10, -10)){
						for(int j=0; j<explored.size(); j++){
							if(explored[i].x != locations[0].x || explored[i].z != locations[0].z-10){ // if not in explored
								rotation_state = 0;
								moving = 10;
								break;
							
							}
						}
						//if(moving == 10)
						//	break;
					}
					if(path.locations[i] == locations[0] + glm::vec3(10, -10, 0)){
						for(int j=0; j<explored.size(); j++){
							if(explored[i].x != locations[0].x+10 || explored[i].z != locations[0].z){ // if not in explored
								rotation_state = 1;
								moving = 10;
								break;
							
							}
						}
						//if(moving == 10)
						//	break;
					}
					if(path.locations[i] == locations[0] + glm::vec3(-10, -10, 0)){
						for(int j=0; j<explored.size(); j++){
							if(explored[i].x != locations[0].x-10 || explored[i].z != locations[0].z){ // if not in explored
								rotation_state = 2;
								moving = 10;
								break;
							
							}
						}
						//if(moving == 10)
						//	break;
					}
					if(path.locations[i] == locations[0] + glm::vec3(0, -10, 10)){
						for(int j=0; j<explored.size(); j++){
							if(explored[i].x != locations[0].x || explored[i].z != locations[0].z+10){ // if not in explored
								rotation_state = 3;
								moving = 10;
								break;
							
							}
						}
						//if(moving == 10)
						//	break;
					}/*
					//this section will only run if there are no available paths
					if(i == path.locations.size()-1){puts("we out here");
					to_start.pop_back(); // pops off current location, now the end is the previous one
					x_buff = locations[0].x - to_start.back().x;
					z_buff = locations[0].z - to_start.back().z;
					if(x_buff < 0)
						rotation_state = 1;
					else if(x_buff > 0)
						rotation_state = 2;
					else if(z_buff < 0)
						rotation_state = 3;
					else if(z_buff > 0)
						rotation_state = 0;
					}
					*/
					
				}
			}
		}
	}
	void move_(int state){
		if(state == 0){
			locations[0].z -= bot_speed;
			moving -= bot_speed;
			if(fmod(rotation, (2*M_PI)) < 0.1){
				rotation = 0.0f;
			}else{
				rotation += bot_speed/2;
			}
		}
		else if(state == 1){
			locations[0].x += bot_speed;
			moving -= bot_speed;
			if(fmod(rotation, ((3*M_PI)/2)) < 0.1 && rotation >= 0.1){
				rotation = ((3*M_PI)/2);
			}else{
				rotation += bot_speed/2;
			}
		}
		else if(state == 2){
			locations[0].x -= bot_speed;
			moving -= bot_speed;
			if(fmod(rotation, (M_PI/2)) < 0.1 && rotation >= 0.1){
				rotation = M_PI/2;
			}else{
				rotation += bot_speed/2;
			}
		}
		else if(state == 3){
			locations[0].z += bot_speed;
			moving -= bot_speed;
			if(fmod(rotation, (M_PI)) < 0.1 && rotation >= 0.1){
				rotation = M_PI;
			}else{
				rotation += bot_speed/2;
			}
		}
	}

	std::vector<glm::mat4> create_models() override {
		std::vector<glm::mat4> models;
		data_mutex.lock();
		models.reserve(locations.size());
		for(glm::vec3 l : locations){
			glm::mat4 new_model = glm::mat4(1.0f);
			new_model = translate(new_model, l);
			new_model = rotate(new_model, rotation, glm::vec3(0, 1, 0));
			models.push_back(new_model);
		}
		data_mutex.unlock();
		return models;
	}
};
