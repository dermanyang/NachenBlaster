#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
using namespace std;

//	StudentWorld.cpp
//	This file contains class and function implementations.
//
//	Built for CS32
//	Completed on 1st March, 2018
//	by Sean Derman Yang
//	UCID: 504768472
//
//

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

//NACHENBLASTER ACCESSORS
double StudentWorld :: nachenX() { return nachenblaster()->getX(); }
double StudentWorld::nachenY() { return nachenblaster()->getY(); }
double StudentWorld::nachenRad() { return nachenblaster()->getRadius(); }

//NACHENBLASTER MUTATORS
void StudentWorld::sufferDamage(int amt) { nachenblaster()->decHealth(amt); }
void StudentWorld::gainHealth(int amt) { nachenblaster()->incHealth(amt); }
void StudentWorld::addTorpedo(int amt) { nachenblaster()->addPedo(amt); }


void StudentWorld::dropGoodie(int id, double x, double y)
{
	if (id == LIFE_GOODIE)
		m_actors.push_back(new ExtraLifeGoodie(this, x, y));
	else if (id == REPAIR_GOODIE)
		m_actors.push_back(new RepairGoodie(this, x, y));
	else if (id == FLAT_GOODIE)
		m_actors.push_back(new FlatTorpedoGoodie(this, x, y));
}

bool StudentWorld :: projectileCollision(Actor* obj, int identifier)
{

	for (vector<Actor*> ::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->identifier() == identifier && euclidianDist(obj->getX(), obj->getY(), (*it)->getX(), (*it)->getY()) <
			0.75 * (obj->getRadius() + (*it)->getRadius()))
		{
			(*it)->kill(); //kill the projectile
			return true;
		}
	}
	return false;
}

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{}

StudentWorld:: ~StudentWorld()
{
	cleanUp();
}
int StudentWorld::init()	
{
	m_nB = new NachenBlaster(this);
	m_numDestroyedAliens = 0; m_displayedAliens = 0;
	for (int i = 0; i < 30; i++)
	{
		m_actors.push_back(new Star(this, randInt(0, VIEW_WIDTH-1)));
	}
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::newAlien()
{
	//reset paramaters
	int tot = 6 + (4 * getLevel());
	int rem = tot - destroyedAliens();
	int max_on_screen = 4 + (.5 * getLevel());
		
	//algorithm
	int S1 = 60, S2 = (20 + (getLevel() * 5)), S3 = (5 + (getLevel() * 10));
	int S = S1 + S2 + S3;
	if (AliensOnScreen() < min(max_on_screen, rem)) 
	{
		int number = randInt(1, S);
		if (number <= S1)
			m_actors.push_back(new Smallgon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
		else if (number > S1 && number <= S1 + S2)
			m_actors.push_back(new Smoregon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
		else if (number > S1 + S2 && number <= S)
			m_actors.push_back(new Snagglegon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
		m_displayedAliens++;
	}
}

void StudentWorld::spawnStars()
{
	if (randInt(1, 15) == 1)
	{
		m_actors.push_back(new Star(this));
	}
}
void StudentWorld:: makeActorsDoSomething()
{
	for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->isAlive())
		{
			(*it)->doSomething();
		}
	}
}

void StudentWorld::deleteDeadActors()
{
	for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();)
	{
		if (!(*it)->isAlive())
		{
			delete *it;
			it = m_actors.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::NBdoSomething() {
	if ((m_nB)->isAlive())
		(m_nB)->doSomething();
}

bool StudentWorld::checkPlayerAlive() {
	return (m_nB->isAlive());
}

bool StudentWorld::finishedLevel() {

	int tot = 6 + (4 * getLevel());
	int rem = tot - destroyedAliens();
	if (rem <= 0)
		return true;
	return false;

}

void StudentWorld::cleanUp()
{
	//DELETE NACHENBLASTER
	{if (m_nB != nullptr)
		delete m_nB;
	m_nB = nullptr;
	}

	//DELETE EVERYTHING ELSE
	for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); )
	{
		delete *it;
		it = m_actors.erase(it);
	}
}

void StudentWorld::createP_Flatulance(double x, double y)
{
	m_actors.push_back(new P_FlatulanceTorpedo(this, x, y));
}

void StudentWorld::createA_Flatulance(double x, double y)
{
	m_actors.push_back(new A_FlatulanceTorpedo(this, x, y));
}

void StudentWorld::createCabbage(double x, double y)
{
	m_actors.push_back(new Cabbage(this, x, y));
}

void StudentWorld::createTurnip(double x, double y)
{
	m_actors.push_back(new Turnip(this, x, y));
}

string StudentWorld :: updateGameStats() {
	string stats;

	ostringstream oss_lives;
	oss_lives << "Lives: ";
	oss_lives << getLives() << setw(2);
	stats += oss_lives.str();

	ostringstream oss_health;
	oss_health << "  Health: ";
	double h = m_nB->health()*100/50;
	oss_health << h << setw(5);
	string health = oss_health.str();
	stats += health;

	ostringstream oss_score;
	oss_score << "%  Score: ";
	int p = getScore();
	oss_score << p << setw(6);
	string score = oss_score.str();
	stats += score;

	ostringstream oss_level;
	oss_level << "  Level: ";
	int q = getLevel();
	oss_level << q << setw(2);
	string level = oss_level.str();
	stats += level;

	ostringstream oss_cabbage;
	oss_cabbage << "  Cabbages: ";
	int r = m_nB->cabbagePower() * 100 / 30;
	oss_cabbage << r << setw(4);
	string cabbage = oss_cabbage.str();
	stats += cabbage;

	ostringstream oss_pedo;
	oss_pedo << "%  Torpedoes: ";
	int x = m_nB->pedoPower();
	oss_pedo << x << setw(2);
	string pedo = oss_pedo.str();
	stats += pedo;

	return stats;
}

void StudentWorld::explode(double x, double y) {
	m_actors.push_back(new Explosion(this, x, y));
}


