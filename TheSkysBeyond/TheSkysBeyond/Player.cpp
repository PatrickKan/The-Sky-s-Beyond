#include "Player.h"

int Player::takeDamage(int damage)
{
	health = health - damage;
	return health;
}

int Player::currentHealth()
{
	return health;
}