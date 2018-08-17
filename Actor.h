#ifndef ACTOR_H_
#define ACTOR_H_
class StudentWorld;
#include "GraphObject.h"

//	Actor.h
//	This file contains class definitions and some trivial implementations.
//
//	Built for CS32
//	Completed on 1st March, 2018
//	by Sean Derman Yang
//	UCID: 504768472
//
//



//SOME CONSTANTS
const int UP_LEFT = 101;
const int DOWN_LEFT = 102;
const int DUE_LEFT = 103;

const int NACHENBLASTER = 201;
const int SNAGGLEGON = 202;
const int SMOREGON = 203;
const int SMALLGON = 204;
const int CABBAGE = 205;
const int TURNIP = 206;
const int ALIEN_FIRED_FLATULANCE = 207;
const int PLAYER_FIRED_FLATULANCE = 208;
const int REPAIR_GOODIE = 209;
const int LIFE_GOODIE = 210;
const int FLAT_GOODIE = 211;
const int STAR = 212;
const int EXPLOSION = 213;
//double check exposion size with doubles


/////			HELPER FUNCTIONS			/////
double euclidianDist(double x1, double y1, double x2, double y2);


/////			ACTOR			/////
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* w, int img_id, double startX, double startY, int startDir, double size, int depth)
		:GraphObject(img_id, startX, startY, startDir, size, depth)
	{	pointer_to_world = w;	}
	virtual ~Actor() {}

	
	//ACCESSOR
	virtual bool isAlive() { return m_alive; }
	StudentWorld* getWorld() { return pointer_to_world; }
	bool hasCollidedWithPlayer();
	virtual int identifier() = 0;

	//MUTATORS
	void kill() { m_alive = false; }

	//OTHER FUNCTIONS
	virtual void doSomething() = 0;

private:
	bool m_alive = true;
	StudentWorld* pointer_to_world;

};


/////			DAMAGABLE ACTOR			/////
class DamagableActor : public Actor
{
public:
	DamagableActor(StudentWorld* w, int id, double x, double y, int startDir, double size, int depth)
		: Actor(w, id, x, y, startDir, size, depth)
	{}

	//ACCESSORS
	int health() { return m_hp; }

	//MUTATORS
	virtual void incHealth(int amt);
	void decHealth(int amt);
	virtual void initializeHP() = 0;
	void setHP(int amt);

private:
	int m_hp;
};

//			ALIEN			//
class Alien : public DamagableActor
{
public:
	Alien(StudentWorld* w, int id, double x, double y, int planlength, double travelspeed)
		:DamagableActor(w, id, x, y, 0, 1.5, 1)
	{
		initializeHP();
		m_planlength = planlength;
		m_travelspeed = travelspeed;
	}

	//MUTATORS
	virtual void initializeHP();
	void updateDeath();
	void doSomething();
	virtual void dropItem() = 0;
	virtual bool attack() = 0;
	void flightPlan(int fp) { m_flightplan = fp; }
	void travelSpeed(double speed) { m_travelspeed = speed; }
	void planLength(int length) { m_planlength = length; }
	virtual void collisionDamage();
	virtual void updatePoints();
	void setFlightPlan(int flightplan) { m_flightplan = flightplan; }

	//ACCESSOR
	bool hitByProjectile();

	//OTHER FUNCTIONS
	bool attackcondition();
	bool basicAttack();
	virtual void newFlightPlan();

private:
	double m_travelspeed;
	int m_flightplan, m_planlength;
};



//			SMALLGON			//
class Smallgon : public Alien
{
public:
	
Smallgon(StudentWorld* w, double x, double y)
		:Alien(w, IID_SMALLGON, x, y, 0, 2)
	{}
	
void dropItem() {}
virtual int identifier() { return SMALLGON; }
virtual bool attack();

};

//			SMOREGON			//
class Smoregon : public Alien
{
public:

	Smoregon(StudentWorld* w, double x, double y)
		:Alien(w,IID_SMOREGON, x, y, 0, 2)
	{}

	virtual int identifier() { return SMOREGON; }
	virtual void dropItem();
	virtual bool attack();
};

//			SNAGGLEGON			//
class Snagglegon : public Alien
{
public:

	Snagglegon(StudentWorld* w, double x, double y)
		:Alien(w, IID_SNAGGLEGON, x, y, 999, 1.75)
	{
		setFlightPlan(DOWN_LEFT);
		Snagglegon::initializeHP();
	}
	virtual int identifier() { return SNAGGLEGON; }
	virtual void initializeHP();
	virtual void dropItem();
	virtual bool attack();
	virtual void collisionDamage();
	virtual void updatePoints();
	virtual void newFlightPlan();
};



//			NACHENBLASTER			//
class NachenBlaster : public DamagableActor
{
public:

	NachenBlaster(StudentWorld* ptr)
		: DamagableActor(ptr, IID_NACHENBLASTER, 0, 128, 0, 1.0, 0)
	{
		initializeHP(); m_cabbage = 30; m_pedo = 0;  m_pedo = 0;
	}

	//ACCESSOR
	int cabbagePower() { return m_cabbage; }
	int pedoPower() { return m_pedo;  }
	virtual int identifier() { return NACHENBLASTER; }

	//MUTATOR
	virtual void initializeHP();
	virtual void doSomething();
	void addCabbage(int amt);
	void addPedo(int amt);
	void decPedo(int amt);

private:
	int m_cabbage;
	int m_pedo;
};



//			STAR			//
class Star : public Actor
{	
public:
	Star(StudentWorld* w, double x = VIEW_WIDTH-1, double y = randInt(0, VIEW_HEIGHT - 1))
		: Actor(w, IID_STAR, x, y, 0, randInt(5, 50)*0.01, 3)
	{}

	virtual ~Star(){}
	void initializeHP() { return; }
	virtual void doSomething();
	virtual int identifier() { return STAR; }
	virtual bool isAlive();
};



//			EXPLOSION			//
class Explosion : public Actor
{
public: 
	Explosion(StudentWorld* w, double x, double y);
	void doSomething();
	virtual int identifier() { return EXPLOSION; }
};


//			PROJECTILE			//
class Projectile : public Actor
{
public:
	Projectile(StudentWorld* w, int id, double x, double y, int dir);
	virtual void doSomething();
	virtual void projectileTravel()=0;
};

//			CABBAGE			//
class Cabbage : public Projectile
{
public:
	Cabbage(StudentWorld* w, double x, double y);
	virtual void projectileTravel();
	virtual int identifier() { return CABBAGE; }
};

//			TURNIP			//
class Turnip : public Projectile
{
public:
	Turnip(StudentWorld* w, double x, double y);
	virtual void projectileTravel();
	virtual int identifier() { return TURNIP; }
};

//			PLAYER FIRED TORPEDO			//
class P_FlatulanceTorpedo : public Projectile
{
public: 
	P_FlatulanceTorpedo(StudentWorld* w, double x, double y);
	virtual void projectileTravel();
	virtual int identifier() { return PLAYER_FIRED_FLATULANCE; }
};


//			ALIEN FIRED TORPEDO			//
class A_FlatulanceTorpedo : public Projectile
{
public:
	A_FlatulanceTorpedo(StudentWorld* w, double x, double y);
	virtual void projectileTravel();
	virtual int identifier() { return ALIEN_FIRED_FLATULANCE; }
};
//			GOODIE			//
class Goodie : public Actor
{
public: 
	Goodie(StudentWorld* w, int id, double x, double y);
	virtual void doSomething();
	virtual void doSomethingDifferent() = 0;
};

class ExtraLifeGoodie : public Goodie
{
public:
	ExtraLifeGoodie(StudentWorld* w, double x, double y);
	virtual void doSomethingDifferent();
	virtual int identifier() { return LIFE_GOODIE; }
};

class RepairGoodie : public Goodie
{
public: 
	RepairGoodie(StudentWorld* w, double x, double y);
	virtual void doSomethingDifferent();
	virtual int identifier() { return REPAIR_GOODIE; }
};

class FlatTorpedoGoodie : public Goodie
{
public:
	FlatTorpedoGoodie(StudentWorld* w, double x, double y);
	virtual void doSomethingDifferent();
	virtual int identifier() { return FLAT_GOODIE; }
};
#endif // ACTOR_H_

