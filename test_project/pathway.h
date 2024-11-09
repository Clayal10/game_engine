#pragma once
#include "game.h"


class pathway : public loaded_object{
public:
	double width = 10; // it is a cube
	pathway() : loaded_object("path.obj", "brick.jpg", glm::vec3(width, 2, width)){}
	
};

class pathway_end : public loaded_object{
public:
	double width = 10;
	pathway_end() : loaded_object("finish_platform.obj", "beans.jpg", glm::vec3(width, 2, width)){}
};

class path_walls : public loaded_object{
public:
	double width = 10;
	path_walls() : loaded_object("path_walls.obj", "wall.jpeg", glm::vec3(width, 20, width)){}



	void create_walls(pathway* path, pathway_end* end, glm::vec3 center, int radius){ // center is the center of a 10x10 tile, radius is * pathway_width
		int r = radius*10;	
		for(int x=center.x-r; x<center.x + r; x += 10){
			for(int z = center.z-r; z<center.z + r; z += 10){
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
