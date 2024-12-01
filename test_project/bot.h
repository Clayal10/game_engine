#pragma once
/*

	11/28 notes:
		-take a look at to_start vector, seems to miss the turn back home
			-When it phases inside the wall, the engine doesn't like when it shoots its vision balls
		-Think about placing objects nicely

*/
#include "game.h"
#include "pathway.h"
#include "projectiles.h"
#include <math.h>
#include <algorithm>

#define speed 0.015f
//#define speed 0.1f

class bot : public loaded_object{
public:
	projectile* vision; // used for vision

	float rotation = 0; // just assuming this is in degrees from 0 (looking -z)
	double time_buff;
	int rotation_state = 0; // starts looking -z
	/*rotate_dir key:
	 * 	0: -z straight
	 * 	1: +x right
	 * 	2: -x left
	 * 	3: +z back
	 */
	float bot_speed = speed;
	float alive = false;
	float moving = 0;
	float love;
	int dead_cats = 0;
	bool run_away = false;
	bool last_location = false;
	std::vector<glm::vec3> explored;
	std::vector<glm::vec3> to_start;
	std::vector<int> rotation_list;	

	int p;

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

	bool see_player() {

		//if(fmod(rotation, M_PI*2) > fmod(player_heading, M_PI*2)-(M_PI/6) && fmod(rotation, M_PI*2) < fmod(player_heading, M_PI*2)+(M_PI/6)){
		p = vision->locations.size();
		for (int i = 0; i < p; i++) {
			if (vision->locations[i].x > player_position.x - 10 && vision->locations[i].x < player_position.x + 10
				&& vision->locations[i].z > player_position.z - 10 && vision->locations[i].z < player_position.z + 10) {
				return true;
			}
		}
		return false;
	}

	int backwards(int rotation){
		if(rotation == 0)return 3;
		if(rotation == 1)return 2;
		if(rotation == 2)return 1;
		if(rotation == 3)return 0;
		puts("INVALID ROTATION");
		return -1;
	}	

	void move(int elapsed_time){
		//if (vision->locations.size() != 0) printf("Shot. Targets existing: %d\n", vision->locations.size());
		if(alive){
			if (see_player() && !run_away) { // prevents from going through here every iteration
				//puts("sees player");
				//if(love == 0){
					//teleport back to start
				//}
				if (love < 20 && love >= 0) {//change >= to >
					//go to origin
					run_away = true;
					bot_speed = 2 * speed;
					puts("From the start to current location rotations");
					for (long unsigned int i = 0; i < rotation_list.size(); i++) {
						//printf("X: %f\tZ: %f\n", to_start[i].x, to_start[i].z);
						printf("%d\n", rotation_list[i]);
					}
				}
				else if(love < 60 && love >= 20){
					//being skiddish
				}
				else{
					
				}
			}
			if(moving > 0){	
				move_(rotation_state); 
				return;
			}else if(!last_location){
				locations[0].x = roundf(locations[0].x);
				locations[0].z = roundf(locations[0].z);
				explored.push_back(locations[0]);
				to_start.push_back(locations[0]);
				
				printf("X: %f Z: %f Face: %f\n", locations[0].x, locations[0].z, rotation);
				if(run_away){
					last_location = true;
				}
			}
			
			if(run_away){
/*Lets change this up:
 what is happening:
 	as soon as it starts moving towards the home spot, it will shoot a ball since run_away is false, but it will hit the player before it gets home, this means run_away will be true before it gets home and adds another spot to rotation_list, causing it to send an invalid number
 
 */
				rotation_state = backwards(rotation_list[rotation_list.size()-1]);
				rotation_list.pop_back();
				moving = 10;

				if(rotation_list.size() == 0){	
					run_away = false;
					last_location = false;
					bot_speed = speed;
					to_start.clear();

				}else return;			
					
/*
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
					last_location = false;
					bot_speed = speed;
				}else{
					return;

				}
*/
			}		
	

			
			//check if at finish spot
			if(love > 100){
				kill_cat(); // follow method
			}
			
			else{

				vision->add_projectile(locations[0] + glm::vec3(0, 10, 0), 0.05f * (player_position - locations[0] - glm::vec3(0, 10, 0)), 10000, false);
				/*USE ROUNDF TO ROUND LOCATIONS TO NEAREST INTEGER*/
				
				if(explored.size() == path.locations.size()){
					explored.clear();
				}
				
				for(long unsigned int i = 0; i<path.locations.size(); i++){
					//this will be for random movement, but if it sees the player TODO switch it up
					if(path.locations[i] == locations[0] + glm::vec3(0, 0, -10)){
						for(long unsigned int j=0; j<explored.size(); j++){
							if(explored[j].x != locations[0].x || explored[j].z != locations[0].z-10){ // if not in explored
								rotation_state = 0;
								rotation_list.push_back(rotation_state);
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
								rotation_list.push_back(rotation_state);
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
								rotation_list.push_back(rotation_state);
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
								rotation_list.push_back(rotation_state);
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
