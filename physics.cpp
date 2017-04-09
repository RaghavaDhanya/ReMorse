#include<iostream>
#include<Box2D/Box2D.h>

using namespace std;

/*
1. Ground is static
2. Player is dyanamic. Horizontal(x) position does not change.
   Vertical(y) position changes on jump.
3. Obstacles are kinematic. They force the player to retreat on collision
   They move horizontally towards the player.
*/

//Hold position and angle data
struct Config
{
	int x, y;
	int angle;
};

//Generic physical object
class PhysicalObject
{
	protected:
		Config config;
		b2Body *body;
		b2World *world;
		b2BodyDef bodyDef;
		b2PolygonShape shape;
		b2FixtureDef fixtureDef;

	public:
		PhysicalObject(b2World *world) {
			this->world = world;
			config.x = config.y = config.angle = 0;
		}

		~PhysicalObject() {}

		void setX(int x) { config.x = x; }

		void setY(int y) { config.y = y; }

		void setAngle(int angle) { config.angle = angle; }

		void setConfig(Config config) { this->config = config; }

		Config getConfig() { return config; }
};

//Main player
class Player: public PhysicalObject
{
	public:
		Player(b2World *world): PhysicalObject(world) {}

		~Player() {}
};

//Obstacle
class Obstacle: public PhysicalObject
{
	public:
		Obstacle(b2World *world): PhysicalObject(world) {}
		~Obstacle() {}
};

//Manages obstacles
class ObstacleManager
{
	public:
		ObstacleManager() {}
		~ObstacleManager() {}
};

int main()
{
	
	cout<<"\nFin\n";
	return 0;
}