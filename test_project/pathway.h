#pragma once
#include "game.h"

#define path_width 10.0f
#define wall_width 10.0f
#define wall_height 80.0f

class pathway : public loaded_object{
public:
	pathway() : loaded_object("path.obj", "brick.jpg", glm::vec3(path_width, 2, path_width)){
		
		collision_check = true;
		/*
		//could push back all locations into locations here and just init this before bot.
		for(int z = 200; z>100; z -= path_width)
			locations.push_back(glm::vec3(0, -10, z));
		for(int x =-40; x<100; x += path_width)
			locations.push_back(glm::vec3(x, -10, 100));
		for(int z=130; z > -30; z -= path_width)
			locations.push_back(glm::vec3(100, -10, z));
		for(int x=110; x > -50; x -= path_width)
			locations.push_back(glm::vec3(x, -10, -30));
		for(int z=-30; z <= 70; z += path_width)
			locations.push_back(glm::vec3(0, -10, z)); // fake finish
		for(int z=-30; z < 70; z+= path_width)
			locations.push_back(glm::vec3(-50, -10, z)); // just to the side of finish
		for(int x=-50; x <= 0; x+=path_width)
			locations.push_back(glm::vec3(x, -10, 70));

		*/
	}




	
};


class pathway_end : public loaded_object{
public:
	pathway_end() : loaded_object("finish_platform.obj", "beans.jpg", glm::vec3(path_width, 2, path_width)){
		//locations.push_back(glm::vec3(0, -10, 70));
	}
};

class path_walls : public wall_block{
public:
	path_walls() : wall_block("path_walls.obj", "wall.jpeg", glm::vec3(wall_width, wall_height, wall_width)){collision_check = true;}
	//path_walls() : wall_block("", "wall.jpeg", glm::vec3(wall_width, wall_height, wall_width)){collision_check = true;}


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
