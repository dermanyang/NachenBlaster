#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
using namespace std;

//	StudentWorld.h
//	This file contains class and function definitions, with some trivial implementations.
//
//	Built for CS32
//	Completed on 1st March, 2018
//	by Sean Derman Yang
//	UCID: 504768472
//
//

class Actor;
class NachenBlaster;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
	~StudentWorld();
	void spawnStars();
	void makeActorsDoSomething();
	void deleteDeadActors();
	void NBdoSomething();
	void newAlien();
    virtual int init();

	virtual int move()
	{
		if (finishedLevel())
		{
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
		if (!checkPlayerAlive())
		{
			decLives();
			return	GWSTATUS_PLAYER_DIED;
		}
		setGameStatText(updateGameStats());
		spawnStars();
		newAlien();
		makeActorsDoSomething();
		deleteDeadActors();
		NBdoSomething();
		return GWSTATUS_CONTINUE_GAME; //keep playing
	}


    virtual void cleanUp();
	bool finishedLevel();
	bool checkPlayerAlive();
	void createCabbage(double x, double y);
	void createTurnip(double x, double y);
	void createP_Flatulance(double x, double y);
	void createA_Flatulance(double x, double y);

	//ACCESSORS
	StudentWorld* getWorld() {return this;}
	int destroyedAliens() { return m_numDestroyedAliens; }
	int AliensOnScreen() { return m_displayedAliens; }
	NachenBlaster* nachenblaster() const { return m_nB; }
	bool projectileCollision(Actor* obj, int identifier);

	//NACHENBLASTER ACCESSORS
	double nachenX();
	double nachenY();
	double nachenRad();
	
	//NACHENBLASTER MUTATORS
	void sufferDamage(int amt);
	void gainHealth(int amt);
	void addTorpedo(int amt);

	//MUTATORS
	void dropGoodie(int id, double x, double y);
	string updateGameStats();
	void inc_destroyedAliens() { m_numDestroyedAliens++; }
	void dec_displayedAliens() { m_displayedAliens--; }
	void explode(double x, double y);

	
private:
	vector<Actor*> m_actors; //pointer to actors
	NachenBlaster* m_nB; //pointer to nachenblaster
	int m_numDestroyedAliens;
	int m_displayedAliens;
};



#endif // STUDENTWORLD_H_
