#pragma once

class elevator : public loaded_object, public block_object {
	public:
		bool up = true;

		elevator(const char* of, const char* tf, glm::vec3 s) : loaded_object(of, tf, s) {}
		void move(int elapsed_time){ // TODO:  Incorporate elapsed_time
			// Just one elevator for now
			if(up) {
				locations[0].y += .1;
				if(locations[0].y > 100)
					up = false;
			} else {
				locations[0].y -= .1;
				if(locations[0].y <= 0)
					up = true;
			}
		}
		void draw(glm::mat4 vp){
			loaded_object::draw(vp);
		}
};

