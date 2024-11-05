#pragma once
#include "game.h"


class pathway : public loaded_object{
public:
	double width = 10; // it is a cube
	pathway() : loaded_object("path.obj", "brick.jpg", glm::vec3(width, 2, width)){
		scale = 1;
	}
};


