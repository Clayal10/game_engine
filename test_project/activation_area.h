#pragma once

/*
 * Type 0:   Call whenever the player is in the area
 * Type 1:   Call once on entry
 * Type 2:   Call on entry and exit
 *
 * Internal types:
 * Type 100: Inactive
 * Type 101: Call on exit
 */

class activation_area : public gameobject {
	public:
		std::vector<void (*)()> callbacks;
		std::vector<char> types;
		activation_area() {
			collision_check = false;
		}
		void add_area(glm::vec3 location, void (*callback_function)(), char type = 1){
			locations.push_back(location);
			callbacks.push_back(callback_function);
			types.push_back(type);
		}
		bool collision_with_index(glm::vec3 position, size_t index, float distance){
			glm::vec3 l = locations[index]; // This'll get optimized out
			return (size.x/2.0f + distance > abs(l.x-position.x) && 
					size.y/2.0f + distance > abs(l.y-position.y) && 
					size.z/2.0f + distance > abs(l.z-position.z));

		}
		ssize_t collision_index(glm::vec3 position, float distance){
			for(size_t i = 0; i < locations.size(); i++){
				if(collision_with_index(position, i, distance)){ 
					if(types[i] == 0)
						callbacks[i]();
					if(types[i] == 1){
						callbacks[i]();
						types[i] = 100;
					} if(types[i] == 2){
						callbacks[i]();
						types[i] = 101;
					}
				}
			}
			return -1;  // Don't block movement
		}
		void move(int elapsed_time){
			for(size_t i = 0; i < types.size(); i++){
				if(types[i] == 101){
					if(!collision_with_index(player_position, i, 0.2f)){  // Note:  Included collision check as a magic number
						callbacks[i]();
						types[i] = 2;
					}
				}
			}
		}
};

