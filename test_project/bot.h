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


class bot : public loaded_object{
public:
	float rotation = 0; // just assuming this is in degrees from 0 (looking -z)
	double time_buff;
	int rotation_state = 0; // starts looking -z
	/*rotate_dir key:
	 * 	0: -z straight
	 * 	1: +x right
	 * 	2: -x left
	 * 	3: +z back
	 */
	float bot_speed = 0.015f;
	//float bot_speed = 0.1f;
	float alive = false;
	float moving = 0;
	double health;
	int dead_cats = 0;
	std::vector<glm::vec3> explored;

	bot(double h, glm::vec3 spawn) : loaded_object("cat.obj", "Cat_bump.jpg", glm::vec3(5, 5, 5)) {
		scale = 0.25f;
		swap_yz = true;
		health = 100;
		locations.push_back(spawn);
		explored.push_back(spawn);
	}

	pathway path;

	void kill_cat(){
		//TODO some cool thing for when it dies
		locations.erase(locations.begin());
		explored.clear();
		alive = false;

	}

	float x_buff, z_buff;

	void move(int elapsed_time){
		/*TODO add support for mulitple cats*/
		
		if(alive){
			//check if at finish spot
			if(health <= 0){
				kill_cat();
			}
			else if(moving > 0){	
				move_(rotation_state); 
			}
			else{//if not finished with the maze, move; going to be a follow the left wall kinda dealio
				/*USE ROUNDF TO ROUND LOCATIONS TO NEAREST INTEGER*/
				locations[0].x = roundf(locations[0].x);
				locations[0].z = roundf(locations[0].z);
				//printf("X: %f Z: %f Face: %f\n", locations[0].x, locations[0].z, rotation);
				
				if(explored.size() == path.locations.size()){
					explored.clear();
				}
				
				//check every location around and move if unexplored
				for(long unsigned int i = 0; i<path.locations.size(); i++){
					//this will be for random movement, but if it sees the player TODO switch it up
					if(path.locations[i] == locations[0] + glm::vec3(0, 0, -10)){
						for(int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x || explored[j].z != locations[0].z-10){ // if not in explored
								rotation_state = 0;
								moving = 10;
								break;
							
							}
						}
					}
					if(path.locations[i] == locations[0] + glm::vec3(10, 0, 0)){
						for(int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x+10 || explored[j].z != locations[0].z){ // if not in explored
								rotation_state = 1;
								moving = 10;
								break;
							
							}
						}
					}
					if(path.locations[i] == locations[0] + glm::vec3(-10, 0, 0)){
						for(int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x-10 || explored[j].z != locations[0].z){ // if not in explored
								rotation_state = 2;
								moving = 10;
								break;
							
							}
						}
					}
					if(path.locations[i] == locations[0] + glm::vec3(0, 0, 10)){
						for(int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x || explored[j].z != locations[0].z+10){ // if not in explored
								rotation_state = 3;
								moving = 10;
								break;
							
							}
						}
					}
					
					
				}
			}
		}else{
			for(int i=0; i<path.locations.size(); i++){
				if(player_position.x > path.locations[i].x-5 && player_position.x < path.locations[i].x+5 && 
						player_position.z > path.locations[i].z-5 && player_position.z < path.locations[i].z+5){
					if(dead_cats == 0)
						alive = true;
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
