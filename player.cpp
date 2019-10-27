#include "player.h"

Player::Player() {

	pos = ori = glm::vec3(1.0f);

	equipState = 0;
}

Player::~Player() {

}

void Player::checkCollision() {


}

void Player::setEquipState(uint8_t state) {

	equipState = state;
}

uint8_t Player::getEquipState() {

	return equipState;
}