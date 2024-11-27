#pragma once
#include "projectiles.h"

class turret : public loaded_object {
public:
	size_t target_idx = 0;
	gameobject *current_target; // Might be nice if this was a list
	projectile *vision;
	int countdown = 0;
	float rotation = 0;
	bool active = false;

	turret() : loaded_object("cube.obj", "monster1.png", glm::vec3(5, 5, 5)) {
		scale = 5;
	}

	void move(int elapsed_time) { // TODO:  Incorporate elapsed_time
		/* At some rate (every so many calls to this function)
		 * Pick a target  (How do we know what targets we have?)
		 * Launch a projectile at it (How do we launch a projectile from here?)
		 */
		if(!active)
			return;
		if(countdown > 0){
			countdown--;
			return;
		}
		countdown = 314;
		if(!current_target->locations.size())
			return;
		for(auto l : locations){
			glm::vec3 target_location = current_target->locations[target_idx];
			vision->add_projectile(l, 0.2f * glm::normalize(target_location - l), 100000); 
			target_idx++;
			if(target_idx >= current_target->locations.size())
				target_idx = 0;
		}
	}

	void animate(){
		if(active)
			rotation += 0.01;
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


