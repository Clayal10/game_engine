#pragma once
#include "game.h"

#define path_width 10.0f
#define wall_width 10.0f
#define wall_height 80.0f

class pathway : public loaded_object{
public:
	pathway() : loaded_object("objects/path.obj", "images/brick.jpg", glm::vec3(path_width, 2, path_width)){
		
		collision_check = true;
	}




	
};


class pathway_end : public loaded_object{
public:
	pathway_end() : loaded_object("objects/finish_platform.obj", "images/beans.jpg", glm::vec3(path_width, 2, path_width)){
		//locations.push_back(glm::vec3(0, -10, 70));
	}
};

class path_walls : public wall_block{
public:
	path_walls() : wall_block("objects/path_walls.obj", "images/wall.jpeg", glm::vec3(wall_width, wall_height, wall_width)){collision_check = true;}


	void create_walls(pathway* path, pathway_end* end, glm::vec3 center, int radius){ // center is the center of a 10x10 tile, radius is * pathway_width
		int r = radius*10;	
		for(int x=center.x-r; x<center.x + r; x += path_width){
			for(int z = center.z-r; z<center.z + r; z += path_width){
				//this is slow, but can't work is_empty into it :/
				for(long unsigned int i=0; i<path->locations.size(); i++){// check each location
					if(x == path->locations[i].x && z == path->locations[i].z){break;}
					else if(x == end->locations[0].x && z == end->locations[0].z){break;}
					else if(i == path->locations.size()-1){ // made it to the end unscathed
						locations.push_back(glm::vec3(x, 0, z));
					}
				}
			}
		}
	}
};

/*Use a file reading system to create the maze like seth did.
 * */
