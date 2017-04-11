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
	//'Publicly' deriving subclasses will have these as private members
    protected:
        b2Body *body;
        b2World *world;
        b2BodyDef bodyDef;
        b2PolygonShape shape;
        b2FixtureDef fixtureDef;

    public:
        PhysicalObject(b2World *world) 
        {
            this->world = world;
        }

        ~PhysicalObject() 
        {
            if(world)
                world->DestroyBody(body);
        }

        Config getConfig() 
        {
            Config config;

            if(body)
            {
                config.x = body->GetPosition().x;
                config.y = body->GetPosition().y;
                config.angle = body->GetAngle();
            }

            return config; 
        }
};

//Main player
class Player: public PhysicalObject
{
    static const int WIDTH = 1;
    static const int HEIGHT = 1;

    static const int MAX_JUMP = 4;
    static const int JUMP_IMPULSE = 100;   

    //Replace with actual position of ground wall in game. Somehow.
    static const int GROUND_POS = 2;   

    bool inAir;     //To hover on long press. Check jump function

    public:
        Player(b2World *world, Config initConfig): PhysicalObject(world) 
        {
            inAir = false;

            //Define body
            bodyDef.type = b2_dynamicBody;
            bodyDef.position.Set(initConfig.x, initConfig.y); //Starting position
            bodyDef.angle = initConfig.angle;

            //Create the body in the world
            body = world->CreateBody(&bodyDef);

            //Shape of fixture
            shape.SetAsBox(WIDTH, HEIGHT);

            //Define fixture with the shape
            fixtureDef.shape = &shape;
            fixtureDef.density = 1;

            //Add fixture to body
            body->CreateFixture(&fixtureDef);
        }

        ~Player() {}

        /* When called with true, player will jump with initial impulse, and hover there.
           When called with false, player will fall if hovering */
        void setJump(bool jumpEnable)
        {
            /* Enforce max jumping height by applying a proporional force downwards */
            if(body->GetPosition().y > MAX_JUMP)
                body->ApplyForce(body->GetMass()*-body->GetLinearVelocity(), 
                    body->GetWorldCenter(), true);

            if(jumpEnable)
            {
                //Apply initial impulse to make player jump
                if(!inAir)
                {
                    inAir = true;
                    body->ApplyLinearImpulse(b2Vec2(0, JUMP_IMPULSE), body->GetWorldCenter(), true);
                }

                //If player begins falling down, apply force opposing gravity
                //Consider setting velocity 0
                else if(body->GetLinearVelocity().y < 0)
                    body->ApplyForce(body->GetMass()*-world->GetGravity(), body->GetWorldCenter(), true);
            }  

            else
            {
                //Disable inAir on hitting ground
                //Do this by getting position of ground wall
                if(body->GetPosition().y < GROUND_POS)
                    inAir = false;
            }
        }
};

//One triangular obstacle. ObstacleManager creates and handles these.
//Position refers to midpoint of base
class Obstacle: public PhysicalObject
{
    int base;
    int height;

    public:
        Obstacle(b2World *world, int base, int height, Config initConfig): PhysicalObject(world) 
        {
            this->base= base;
            this->height = height;

            //Define body
            bodyDef.type = b2_kinematicBody;
            bodyDef.position.Set(initConfig.x, initConfig.y);
            bodyDef.angle = initConfig.angle;

            //Create the body in the world
            body = world->CreateBody(&bodyDef);

            //Triangle fixture vertices
            b2Vec2 vertices[3];
            vertices[0].Set(initConfig.x - (base/2), initConfig.y);
            vertices[1].Set(initConfig.x, initConfig.y + height);
            vertices[2].Set(initConfig.x + (base/2), initConfig.y);
            shape.Set(vertices, 3);

            //Define fixture with the shape
            fixtureDef.shape = &shape;
            fixtureDef.density = 1;

            //Add fixture to body
            body->CreateFixture(&fixtureDef);
        }

        ~Obstacle() {}

        void setSpeed(int speed)
        {
            body->SetLinearVelocity(b2Vec2(-speed, 0));
        }
};

//The ground, and possibly upper ceiling
class Wall: public PhysicalObject
{
    static constexpr float WIDTH = 50.0f;
    static constexpr float HEIGHT = 10.0f;
    
    public:
        Wall(b2World *world, Config initConfig): PhysicalObject(world) 
        {
            bodyDef.position.Set(initConfig.x, initConfig.y);
            body = world->CreateBody(&bodyDef);   

            shape.SetAsBox(WIDTH, HEIGHT);
            body->CreateFixture(&shape, 0.0f);
        }

        ~Wall() {}
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