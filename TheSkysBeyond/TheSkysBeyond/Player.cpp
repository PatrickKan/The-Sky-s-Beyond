#include "Player.h"
#include "src//ofApp.h"

int Player::takeDamage(int damage)
{
	//If more damage done then health, health gets set to 0 instead of negative
	if (damage > health)
	{
		health = 0;
		return health;
	}
	
	health = health - damage;

	return health;
}

int Player::currentHealth()
{
	return health;
}