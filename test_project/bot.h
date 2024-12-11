#pragma once
/*
	12/3 notes: For the final project
		The bot movement AI is pretty trash, need to redo that.
		Need to have a placement of a treat function. just by using right click and snap to the ground probably.
		Change around the Love score conditionals

*/
#include "game.h"
#include "pathway.h"
#include "projectiles.h"

#include <math.h>
#include <algorithm>

#define speed 0.015f
//#define speed 0.1f

class Treat : public loaded_object{
public:
	/*This will be a placable object*/

	Treat() : loaded_object("cat.obj", "Cat_bump.jpg", glm::vec3(1, 1, 1)){
		swap_yz = true;
		scale = 0.1f;
	}

};



class bot : public loaded_object{
public:
	projectile* vision; // used for vision
	Treat* treat;

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
	bool searching_for_player = true;
	bool loves_player = false;
	std::vector<glm::vec3> explored;
	std::vector<glm::vec3> to_start;
	std::vector<int> rotation_list;	
	std::vector<glm::vec3> neighbors;
	int p;

	pathway path;
	glm::vec3 spawn_point;
	std::vector<glm::vec3> exit_points;

	bot(double h) : loaded_object("cat.obj", "Cat_bump.jpg", glm::vec3(5, 5, 5)) {
		scale = 0.25f;
		swap_yz = true;
		love = 0;
	}
	void add_exit_point(glm::vec3 place){
		exit_points.push_back(place);
	}
	void set_path(std::vector<glm::vec3> places){
		path.locations = places;
	}
	void set_spawn(glm::vec3 place){
		spawn_point = place;
		locations.push_back(place);
		explored.push_back(place);
	}

	/*TODO unused, should have functinality changed*/
	void kill_cat(){
		locations.erase(locations.begin());
		to_start.clear();
		explored.clear();
		alive = false;

	}

	float x_buff, z_buff;
	void update_neighbors(std::vector<glm::vec3>* neighbors){
		neighbors->clear();
		glm::vec3 buffer;
		//This is returning eligable neighbors
		for(auto place : path.locations){
			buffer = locations[0] + glm::vec3(0, 0, -10);
			if(buffer == place) neighbors->push_back(buffer);
			buffer = locations[0] + glm::vec3(10, 0, 0);
			if(buffer == place) neighbors->push_back(buffer);
			buffer = locations[0] + glm::vec3(-10, 0, 0);
			if(buffer == place) neighbors->push_back(buffer);
			buffer = locations[0] + glm::vec3(0, 0, 10);
			if(buffer == place) neighbors->push_back(buffer);
		}
		
	
	}

	/*Logic check*/
	bool see_player() {

		//if(fmod(rotation, M_PI*2) > fmod(player_heading, M_PI*2)-(M_PI/6) && fmod(rotation, M_PI*2) < fmod(player_heading, M_PI*2)+(M_PI/6)){
		p = vision->locations.size();
		for (int i = 0; i < p; i++) {
			if (vision->locations[i].x > player_position.x - 5 && vision->locations[i].x < player_position.x + 5
				&& vision->locations[i].z > player_position.z - 5 && vision->locations[i].z < player_position.z + 5) {
				return true;
			}
		}
		return false;
	}
	/*Helper for the bot to back track TODO may need to modify*/
	int backwards(int rotation){
		if(rotation == 0)return 3;
		if(rotation == 1)return 2;
		if(rotation == 2)return 1;
		if(rotation == 3)return 0;
		puts("INVALID ROTATION");
		return -1;
	}	
	/*Helper for floating point comparison*/
	bool close_enough(float one, float two){// using bot_speed as my margin of error
		if(one < two+bot_speed && one > two-bot_speed){
			return true;
		}
		return false;
	}
	bool close_enough_large(float one, float two){// using bot_speed as my margin of error
		if(one < two+5.0f && one > two-5.0f){
			return true;
		}
		return false;
	}
	/*TODO Check for if treat is in the tile*/
	int collect_treat(){
		//for(glm::vec3 place : treat->locations){
		for(long unsigned int idx = 0; idx<treat->locations.size(); idx++){
			if(close_enough_large(treat->locations[idx].x, locations[0].x) && close_enough_large(treat->locations[idx].z, locations[0].z)){
				return idx;
			}
		}
	
		return -1;
	}

	/*End conditional for the game*/
	bool win_check(){ // cat is not on the pathway
		if(close_enough_large(locations[0].x, spawn_point.x) && close_enough_large(locations[0].z, spawn_point.z)) return false;
		for(glm::vec3 place : path.locations){
			if(close_enough_large(locations[0].x, place.x) && close_enough_large(locations[0].z, place.z)) return false;
		}
		return true;
	}
	glm::vec3 place_from_state(glm::vec3 place, int rotation_state){
		glm::vec3 return_place;
		switch(rotation_state){
			case 0:
				return_place = place + glm::vec3(0, 0, -10);
				break;
			case 1:
				return_place = place + glm::vec3(10, 0, 0);
				break;
			case 2:
				return_place = place + glm::vec3(-10, 0, 0);
				break;
			case 3:
				return_place = place + glm::vec3(0, 0, 10);
				break;
		}
		return return_place;
	}
	
	int closest_interval(float heading){ // returns rotation_direction
		float buffer;
		float ans = 1000000.0f; // worst case
		int idx = 0;
		//for(int i=-M_PI; i<M_PI + 0.1; i+= M_PI/2){
		for(int i=0; i<5; i++){
			buffer = fabs(heading - (i*(M_PI/2) - M_PI));
			//printf("BUFFER: %f\n", buffer);
			if(buffer < ans){
				ans = buffer;
				idx = i;
			}
		}
		int answer;
		switch(idx){
			case 0:
				answer = 1;
				break;
			case 1:
				answer = 3;
				break;
			case 2:
				answer = 2;
				break;
			case 3: 
				answer = 0;
				break;
			case 4:
				answer = 1;
				break;
		}

		return answer;


	}
	
	/*This is where all movement and logic happens. This is ran every 'frame'*/
	int treat_idx;
	float player_dir; // in radians
	void move(int elapsed_time){
		if(alive){
			treat_idx = collect_treat();
			if(treat_idx != -1){
				puts("FOUND TREAT");
				treat->locations.erase(treat->locations.begin()+treat_idx);
				love += 5;

			}
			if (see_player() && !run_away) { // prevents from going through here every iteration
				//puts("sees player");
				if (love < 20) {//change >= to >
					//go to origin
					run_away = true;
					bot_speed = 3 * speed;
					puts("From the start to current location rotations");
					for (long unsigned int i = 0; i < rotation_list.size(); i++) {
						//printf("X: %f\tZ: %f\n", to_start[i].x, to_start[i].z);
						printf("%d\n", rotation_list[i]);
					}
				}
				else if(love < 60 && love >= 20){
					//being skiddish
					run_away = false;
					searching_for_player = false;

				}
				else if(love >= 60){
					run_away = false;
					loves_player = true;
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
			/*Once we succeed, everything is skipped after this*/
			//player_dir = atan2(player_position.z-locations[0].z , player_position.x-locations[0].x);
			//printf("Player angle: %f\n", player_dir);
			if(loves_player){
				main_hud.lprintf(1, "Looks like it loves you!\tTime to get out of this maze.");
				player_dir = atan2(locations[0].z - player_position.z , locations[0].x-player_position.x);
				rotation_state = closest_interval(player_dir);
				
				printf("New Rotation State: %d\n", rotation_state);
				
				for(glm::vec3 place : path.locations){
					if(place_from_state(locations[0], rotation_state) == place){
						moving = 10;
						return;
					}
				}
				for(auto place : exit_points){
					if(place.x == locations[0].x && place.z == locations[0].z){
						moving = 10;
						main_hud.lprintf(0, "You Won!");

						return;
					}
				}
				moving = 0;

				return;
			}
			if(run_away){
 
				rotation_state = backwards(rotation_list[rotation_list.size()-1]);
				rotation_list.pop_back();
				moving = 10;

				if(rotation_list.size() == 1){	
					run_away = false;
					last_location = false;
					bot_speed = speed;
					to_start.clear();

				}else return;			
					
			}else{
				if(win_check()){
					puts("You Won!!!");
				}
				
				/*Need the cat to not shoot right away to fix a bug*/
				if(to_start.size() > 1){
					vision->add_projectile(locations[0] + glm::vec3(0, 10, 0), 0.05f * (player_position - locations[0] - glm::vec3(0, 10, 0)), 10000, false);
				}
				/*USE ROUNDF TO ROUND LOCATIONS TO NEAREST INTEGER*/
				
				if(explored.size() == path.locations.size()){
					explored.clear();
				}
				/*New Movement AI*/
				update_neighbors(&neighbors);
				printf("Number of neighbors: %ld\n", neighbors.size());
				
				int state_buffer;
				for(auto place : neighbors){
					//if we can make a turn left or right
					state_buffer = closest_interval(atan2(locations[0].z - place.z , locations[0].x-place.x));
					if(state_buffer != rotation_state && state_buffer != backwards(rotation_state)){
						
						rotation_state = state_buffer;
						rotation_list.push_back(rotation_state);
						moving = 10;
						return;
					}
				}
			
				for(auto place : neighbors){
					//if can go straight
					if(place == place_from_state(locations[0], rotation_state)){
						rotation_list.push_back(rotation_state);
						moving = 10;
						return;
					}
				}
				//This is if no neighbor in front of us
				for(auto place : neighbors){
					rotation_state = closest_interval(atan2(locations[0].z - place.z , locations[0].x-place.x));
					rotation_list.push_back(rotation_state);
					moving = 10;
					return;
				}
				
				//rotation_state = closest_interval(atan2(locations[0].z - place.z , locations[0].x-place.x));
				/*None of the neighbors are unexplored at this point*/
				puts("\t\tback tracking");
				rotation_list.pop_back();
				rotation_state = rotation_list[rotation_list.size()-1]; // goes back 1
					

			
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
