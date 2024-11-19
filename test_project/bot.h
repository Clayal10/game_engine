#pragma once
/*
	This bot only stays on the pathway
	Need to make .obj file
		Using cat, may need to check collision since the location is in "front" of the cat

	11/18 notes:
		-take a look at to_start vector, seems to miss the turn back home
		-prevent it seeing through walls
			-could have cat shoot invisible projectiles every so often, see if it hits the wall or goes near player
				(already have some of that code)
		-Think about placing objects nicely

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
	float love;
	int dead_cats = 0;
	bool run_away = false;
	std::vector<glm::vec3> explored;
	std::vector<glm::vec3> to_start;

	bot(double h, glm::vec3 spawn) : loaded_object("cat.obj", "Cat_bump.jpg", glm::vec3(5, 5, 5)) {
		scale = 0.25f;
		swap_yz = true;
		love = 0;
		locations.push_back(spawn);
		explored.push_back(spawn);
	}

	pathway path;

	void kill_cat(){
		locations.erase(locations.begin());
		to_start.clear();
		explored.clear();
		alive = false;

	}

	float x_buff, z_buff;

	bool see_player(){
		if(fmod(rotation, M_PI*2) > fmod(player_heading, M_PI*2)-(M_PI/6) && fmod(rotation, M_PI*2) < fmod(player_heading, M_PI*2)+(M_PI/6)){
			//need to check if there is a wall in the way
			return true;
		}


		//if(locations[0].z > 85)
		//	return true;

		return false;
	}
	//check if look at eachother. if see_player && player_heading at cat

	void move(int elapsed_time){
		if(alive){

			if(moving > 0){	
				move_(rotation_state); 
				return;
			}
			if(see_player()){
				if(love < 1){
					//go to origin
					run_away = true;		
					puts("sees player");
				}
				else{
					//being skiddish
				}
			}
			if(run_away){
				puts("going back");
			
				to_start.pop_back();
				if(to_start[to_start.size()-1].z < locations[0].z){
					rotation_state = 0;
					moving = 10;
				}	
				else if(to_start[to_start.size()-1].z > locations[0].z){
					rotation_state = 3;
					moving = 10;
				}
				else if(to_start[to_start.size()-1].x < locations[0].x){
					rotation_state = 2;
					moving = 10;
				}
				else if(to_start[to_start.size()-1].x > locations[0].x){
					rotation_state = 1;
					moving = 10;
				}
				if(to_start.size() == 1){
					run_away = false;
				}else{
					return;
				}
			}		
	

			
			//check if at finish spot
			if(love > 100){
				kill_cat(); // follow method
			}
			
			else{
				/*USE ROUNDF TO ROUND LOCATIONS TO NEAREST INTEGER*/
				locations[0].x = roundf(locations[0].x);
				locations[0].z = roundf(locations[0].z);
				explored.push_back(locations[0]);
				to_start.push_back(locations[0]);
				
				printf("X: %f Z: %f Face: %f\n", locations[0].x, locations[0].z, rotation);
				
				if(explored.size() == path.locations.size()){
					explored.clear();
				}
				
				//need to have some sort of 'junction' vector so it can go back if it gets stuck; impliment some sort of randomness for when it gets back to the junction
				
				for(long unsigned int i = 0; i<path.locations.size(); i++){
					//this will be for random movement, but if it sees the player TODO switch it up
					if(path.locations[i] == locations[0] + glm::vec3(0, 0, -10)){
						for(long unsigned int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x || explored[j].z != locations[0].z-10){ // if not in explored
								rotation_state = 0;
								moving = 10;
								break;
							
							}
						}
						if(moving == 10)
							break;
					}
					if(path.locations[i] == locations[0] + glm::vec3(10, 0, 0)){
						for(long unsigned int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x+10 || explored[j].z != locations[0].z){ // if not in explored
								rotation_state = 1;
								moving = 10;
								break;
							
							}
						}
						if(moving == 10)
							break;
					}
					if(path.locations[i] == locations[0] + glm::vec3(-10, 0, 0)){
						for(long unsigned int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x-10 || explored[j].z != locations[0].z){ // if not in explored
								rotation_state = 2;
								moving = 10;
								break;
							
							}
						}
						if(moving == 10)
							break;
					}
					if(path.locations[i] == locations[0] + glm::vec3(0, 0, 10)){
						for(long unsigned int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x || explored[j].z != locations[0].z+10){ // if not in explored
								rotation_state = 3;
								moving = 10;
								break;
							
							}
						}
						if(moving == 10)
							break;
					}
					
					
				}
			
			}
		}else{
			for(long unsigned int i=0; i<path.locations.size(); i++){
				if(player_position.x > path.locations[i].x-5 && player_position.x < path.locations[i].x+5 && 
						player_position.z > path.locations[i].z-5 && player_position.z < path.locations[i].z+5){
					if(dead_cats == 0)
						alive = true;
				}
			}
		}
	}
	bool close_enough(float one, float two){// using bot_speed as my margin of error
		if(one < two+bot_speed && one > two-bot_speed){
			return true;
		}
		return false;
	}

	void move_(int state){
		if(state == 0){
			locations[0].z -= bot_speed;
			moving -= bot_speed;
			if(close_enough(0, fmod(rotation, M_PI*2))){
				rotation = 0.0f;
			}else{
				rotation += bot_speed/2;
			}
		}
		else if(state == 1){
			locations[0].x += bot_speed;
			moving -= bot_speed;
			if(close_enough((3*M_PI)/2, fmod(rotation, M_PI*2))){
				rotation = ((3*M_PI)/2);
			}else{
				rotation += bot_speed/2;
			}
		}
		else if(state == 2){
			locations[0].x -= bot_speed;
			moving -= bot_speed;
			if(close_enough(M_PI/2, fmod(rotation, M_PI*2))){
				rotation = M_PI/2;
			}else{
				rotation += bot_speed/2;
			}
		}
		else if(state == 3){
			locations[0].z += bot_speed;
			moving -= bot_speed;
			if(close_enough(M_PI, fmod(rotation, M_PI*2))){
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
