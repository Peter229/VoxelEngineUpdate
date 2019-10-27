#pragma once

#include "glm/glm.hpp"
#include <iostream>

class Player {

public:
	Player();
	~Player();

	void setEquipState(uint8_t state);
	uint8_t getEquipState();
	void checkCollision();

private:
	glm::vec3 pos;
	glm::vec3 ori;
	uint8_t equipState;
};