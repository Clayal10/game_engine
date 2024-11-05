#pragma once
/*
	This bot only stays on the pathway
	Need to make .obj file
		Using cat, may need to check collision since the location is in "front" of the cat
*/
class bot : public loaded_object{
	public:
	bot() : loaded_object("cat.obj", "Cat_bump.jpg", glm::vec3(4, 4, 4)){
		scale = 0.2;
	}
};
