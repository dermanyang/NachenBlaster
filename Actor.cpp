#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

//	Actor.cpp
//	This file contains class and function implementations.
//
//	Built for CS32
//	Completed on 1st March, 2018
//	by Sean Derman Yang
//	UCID: 504768472
//
//


/////			HELPER FUNCTION IMPLEMENTATION			/////

double euclidianDist(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}



/////			ACTOR IMPLEMENTATION					/////

bool Actor::hasCollidedWithPlayer()
{
	if (euclidianDist(getX(), getY(), getWorld()->nachenX(), getWorld()->nachenY()) <
		0.75 * (getRadius() + getWorld()->nachenRad()))
	{
		return true;
	}
	else
		return false;
}


/////			DAMAGABLEACTOR IMPLEMENTATION			/////

void DamagableActor::incHealth(int amt)
{
	//only the NachenBlaster will use this function, so need not worry about health cap for aliens.
	m_hp += amt;
	if (m_hp > 50)
		m_hp = 50;
}

void DamagableActor::decHealth(int amt)
{
	m_hp -= amt;
	if (m_hp <= 0)
	{
		kill();
		getWorld()->dec_displayedAliens();
		getWorld()->inc_destroyedAliens();
	}
}

void DamagableActor::setHP(int amt)
{
	m_hp = amt;

}



/////			NACHENBLASTER IMPLEMENTATION			///// 

void NachenBlaster::initializeHP()
{
	setHP(50);
}

void NachenBlaster::addPedo(int amt)
{
	m_pedo += amt;
}

void NachenBlaster::decPedo(int amt)
{
	m_pedo -= amt;
	if (m_pedo < 0)
		m_pedo = 0;
}

void NachenBlaster::addCabbage(int amt) {
	if (m_cabbage < 30 - amt)
		m_cabbage += amt;
	else
		m_cabbage = 30;
}


void NachenBlaster::doSomething()
{
	if (!isAlive())
		return;
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_SPACE:
			if (cabbagePower() > 5)
			{
				getWorld()->createCabbage(getX() + 12, getY());
				getWorld()->playSound(SOUND_PLAYER_SHOOT);
				m_cabbage -= 5;
			}
			break;

		case KEY_PRESS_LEFT:
			if (getX() > 6)
				moveTo(getX() - 6, getY());
			break;

		case KEY_PRESS_RIGHT:
			if (getX() < VIEW_WIDTH - 6)
				moveTo(getX() + 6, getY());
			break;

		case KEY_PRESS_DOWN:
			if (getY() > 6)
				moveTo(getX(), getY() - 6);

			break;
		case KEY_PRESS_UP:
			if (getY() < VIEW_HEIGHT - 6)
				moveTo(getX(), getY() + 6);
			break;

		case KEY_PRESS_TAB:
			if (pedoPower() > 0)
			{
				getWorld()->createP_Flatulance(getX() + 12, getY());
				getWorld()->playSound(SOUND_TORPEDO);
				decPedo(1);
			}
			break;
		}
	}
	addCabbage(1);
}



/////			ALIEN IMPLEMENTATION			/////
void Alien::newFlightPlan() {
	if (m_planlength == 0 || getY() > VIEW_HEIGHT - 1 || getY() < 0)
	{
		if (getY() >= VIEW_HEIGHT - 1)
			m_flightplan = DOWN_LEFT;
		else if (getY() <= 0)
			m_flightplan = UP_LEFT;
		if (m_planlength == 0)
			m_flightplan = randInt(101, 103);
		m_planlength = randInt(1, 32);
	}
}

void Alien::collisionDamage() {
	getWorld()->sufferDamage(5);
}

void Alien::updatePoints() {
	getWorld()->increaseScore(250);
}

bool Alien::hitByProjectile()
{
	bool flag = false;
	if (getWorld()->projectileCollision(this, CABBAGE))
	{
		getWorld()->playSound(SOUND_BLAST);
		decHealth(2);
		if (!isAlive())
			updateDeath();
		flag = true;
	}

	if (getWorld()->projectileCollision(this, PLAYER_FIRED_FLATULANCE))
	{
		getWorld()->playSound(SOUND_BLAST);
		decHealth(8);
		if (!isAlive())
			updateDeath();
		flag = true;
	}
	return flag;
}


void Alien::updateDeath() {
	updatePoints();
	getWorld()->explode(getX(), getY());
	dropItem();
	getWorld()->playSound(SOUND_DEATH);
}


void Alien::initializeHP() {

	setHP(5 * (1 + (getWorld()->getLevel() - 1) * 0.1));
}

void Alien::doSomething() {

	//1) check alive
	if (!isAlive())
		return;

	//2) check off screen
	if (getX() <= 0)
	{
		kill();
		getWorld()->dec_displayedAliens();
		return;
	}

	//3) check collided
	bool hit = false;

	if (hitByProjectile())
		hit = true;
	if (hasCollidedWithPlayer())
	{
		collisionDamage();
		kill();
		getWorld()->dec_displayedAliens();
		getWorld()->inc_destroyedAliens();
		updateDeath();
		hit = true;
	}

	//4) newflight plan
	newFlightPlan();

	//5) special move
	if (attackcondition())
	{
		if (attack())
			return;
	}

	//6) move
    switch (m_flightplan) {
	case UP_LEFT:
		moveTo(getX() - m_travelspeed, getY() + m_travelspeed);
		break;
	case DOWN_LEFT:
		moveTo(getX() - m_travelspeed, getY() - m_travelspeed);
		break;
	case DUE_LEFT:
		moveTo(getX() - m_travelspeed, getY());
		break;
	}
	m_planlength--;

	//7)check again for collision
	if (hit == false) {
		hitByProjectile();
		if (hasCollidedWithPlayer())
		{
			collisionDamage(); //dec nachenblaster health
			kill();
			getWorld()->dec_displayedAliens();
			getWorld()->inc_destroyedAliens();
			updateDeath();
		}
	}
}

bool Alien::attackcondition() {
	if (getX() > getWorld()->nachenX() && abs(getY() - getWorld()->nachenY()) <= 4)
		return true;
	else
		return false;
}

bool Alien::basicAttack() { //returns true if turnip is shot
	if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1)
	{
		getWorld()->createTurnip(getX() - 14, getY());
		getWorld()->playSound(SOUND_ALIEN_SHOOT);
		return true;
	}
	return false;
}



/////			SMALLGON IMPLEMENTATION			/////
bool Smallgon::attack() {
	return basicAttack();
}


/////			SMOREGON IMPLEMENTATION			/////
bool Smoregon::attack() {

	if (basicAttack())
		return true;

	else if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1)
	{
		flightPlan(DUE_LEFT);
		planLength(VIEW_WIDTH);
		travelSpeed(5);
		return false;
	}
	return false;
}

void Smoregon::dropItem()
{
	int n = randInt(1, 6);
	if (n == 1)
		getWorld()->dropGoodie(REPAIR_GOODIE, getX(), getY());
	if (n == 2)
		getWorld()->dropGoodie(FLAT_GOODIE, getX(), getY());
}



/////			SNAGGLEGON IMPLEMENTATION		/////

void Snagglegon::newFlightPlan() {
	if (getY() > VIEW_HEIGHT - 1)
	{
			setFlightPlan(DOWN_LEFT);
	}
	if (getY() <= 0)
	{
		setFlightPlan(UP_LEFT);
	}
}

void Snagglegon::updatePoints() {
	getWorld()->increaseScore(1000);
}

void Snagglegon::collisionDamage() {
	getWorld()->sufferDamage(15);
}


void Snagglegon::initializeHP() {
	setHP(10 * (1 + ((getWorld()->getLevel() - 1) * 0.1)));
}

void Snagglegon::dropItem()
{
	if (randInt(1, 6) == 1)
		getWorld()->dropGoodie(LIFE_GOODIE, getX(), getY());
}

bool Snagglegon::attack() {

if (randInt(1, (15 / getWorld()->getLevel()) + 10) == 1)
	{
		getWorld()->createA_Flatulance(getX() - 14, getY());
		getWorld()->playSound(SOUND_TORPEDO);
		return true;
	}
	return false;
}


/////			STAR IMPLEMENTATION				/////
void Star :: doSomething() 
{
	moveTo(getX() - 1, getY());
}

bool Star::isAlive()
{
	if (getX() <= -1)
	{
		kill(); return false;
	}
	return true;

}
/////			EXPLOSION IMPLEMENTATION			/////
Explosion::Explosion(StudentWorld* w, double x, double y)
	:Actor(w, IID_EXPLOSION, x, y, 0, 1.0, 0)
{}


void Explosion::doSomething() {
	if (getSize() < 4)
		setSize(1.5*getSize());
	
	else
		kill();
}


/////			PROJECTILE IMPLEMENTATION		/////
Projectile::Projectile(StudentWorld* w, int id, double x, double y, int dir = 0)
	: Actor(w, id, x, y, dir, .5, 1)
{}

void Projectile::doSomething()
{
	if (!isAlive())
		return;

	//if off the screen, kill and return
	if (getX() >= VIEW_WIDTH || getX() <= 0)
	{
		kill(); return;
	}


	if (identifier() == TURNIP)
	{
		if (hasCollidedWithPlayer()) {
			kill();
			getWorld()->sufferDamage(2);
			getWorld()->playSound(SOUND_BLAST);
			return;
		}
	}	if (identifier() == ALIEN_FIRED_FLATULANCE)
	{
		if (hasCollidedWithPlayer()) {
			kill();
			getWorld()->sufferDamage(8);
			getWorld()->playSound(SOUND_BLAST);
			return;
		}
	}

	projectileTravel();

	if (identifier() == CABBAGE || identifier() == TURNIP)
		setDirection(getDirection() + 20);
}



/////		CABBAGE IMPLEMENTATION			/////
Cabbage::Cabbage(StudentWorld* w, double x, double y)
	:Projectile(w, IID_CABBAGE, x, y)
{}

void Cabbage::projectileTravel()
{
	moveTo(getX() + 8, getY());
}


/////			TURNIP IMPLEMENTATION			/////
Turnip::Turnip(StudentWorld* w, double x, double y)
	: Projectile(w, IID_TURNIP, x, y)
{}

void Turnip::projectileTravel()
{
	moveTo(getX() - 6, getY());
}



/////			PLAYER FIRED FLATULANCE TORPEDO IMPLEMENTATION			/////
P_FlatulanceTorpedo::P_FlatulanceTorpedo(StudentWorld* w, double x, double y)
	:Projectile(w, IID_TORPEDO, x, y)
{}

void P_FlatulanceTorpedo::projectileTravel() {
	moveTo(getX() + 8, getY());
}



/////				ALIEN FIRED FLATULANCE TORPEDO IMPLEMENTATION			/////
A_FlatulanceTorpedo::A_FlatulanceTorpedo(StudentWorld* w, double x, double y)
	:Projectile(w, IID_TORPEDO, x, y, 180)
{}

void A_FlatulanceTorpedo::projectileTravel() {
	moveTo(getX() - 8, getY());
}



/////			GOODIE IMPLEMENTATION			/////
Goodie::Goodie(StudentWorld* w, int id, double x, double y)
	:Actor(w, id, x, y, 0, 0.5, 1)
{}

void Goodie::doSomething()
{
	if (!isAlive())
		return;
	if (getX() < 0 || getY() < 0)
	{
		kill(); return;
	}

	if (hasCollidedWithPlayer()) {
		getWorld()->increaseScore(100);
		kill();
		getWorld()->playSound(SOUND_GOODIE);
		doSomethingDifferent();
		return;
	}

	moveTo(getX() - 0.75, getY() - 0.75);

	if (hasCollidedWithPlayer()) {
		getWorld()->increaseScore(100);
		kill();
		getWorld()->playSound(SOUND_GOODIE);
		doSomethingDifferent();
		return;
	}
}



/////			EXTRALIFEGOODIE IMPLEMENTATION			/////
ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* w, double x, double y)
	:Goodie(w, IID_LIFE_GOODIE, x, y)
{}

void ExtraLifeGoodie::doSomethingDifferent()
{
	getWorld()->incLives();
}



/////			REPAIRGOODIE IMPLEMENTATION			/////
RepairGoodie::RepairGoodie(StudentWorld* w, double x, double y)
	:Goodie(w, IID_REPAIR_GOODIE, x, y)
{}

void RepairGoodie::doSomethingDifferent()
{
	getWorld()->gainHealth(10);
}



/////			FLATTORPEDOGOODIE IMPLEMENTATION			/////
FlatTorpedoGoodie::FlatTorpedoGoodie(StudentWorld* w, double x, double y)
	:Goodie(w, IID_TORPEDO_GOODIE, x, y)
{}

void FlatTorpedoGoodie::doSomethingDifferent()
{
	getWorld()->addTorpedo(5);
}


