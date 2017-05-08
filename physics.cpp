#include <iostream>
#include <Box2D/Box2D.h>
#include "states.h"

using namespace std;

/*
1. Ground is static
2. Player is dyanamic. Horizontal(x) position does not change.
   Vertical(y) position changes on jump.
3. Obstacles are kinematic. They force the player to retreat on collision
   They move horizontally towards the player.
*/

string letterToMorse(char let);

////////////////////////////////////////////////////////////
//////////////////////// STRUCTURES ////////////////////////
////////////////////////////////////////////////////////////

//Hold position and angle data
struct Config
{
	float x, y;
	float angle;
};

//To identify body during collision
struct BodyID
{
    //True if player, false if obstacle
    bool isPlayer;

    bool isDot;
};

////////////////////////////////////////////////////////////
//////////////////////// SUPERCLASS ////////////////////////
////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////
//////////////////////// SUBCLASSES ////////////////////////
////////////////////////////////////////////////////////////

//Main player
class Player: public PhysicalObject
{
    static constexpr float WIDTH = 1.10/2.0;
    static constexpr float HEIGHT = 1.86/2.0;

    static const int MAX_JUMP = 4;
    static const int JUMP_IMPULSE = 30;   

    //Replace with actual position of ground wall in game. Somehow.
    int GROUND_POS = 5;   

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

            //Store data in body to identify during collision
            BodyID* id = new BodyID;
            id->isPlayer = true;
            body->SetUserData(id);

            //Shape of fixture
            shape.SetAsBox(WIDTH, HEIGHT);

            //Define fixture with the shape
            fixtureDef.shape = &shape;
            fixtureDef.density = 1;

            //Add fixture to body
            body->CreateFixture(&fixtureDef);
        }

        ~Player() {}

        void setGroundPos(int pos)
        {
        	GROUND_POS = pos;
        }

        void setPos(int x, int y)
        {
        	body->SetLinearVelocity(b2Vec2(0, 0));
        	body->SetTransform(b2Vec2(x, y), 0);
        }

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

                //Player just begins falling down
                else if(body->GetLinearVelocity().y < 0)
                {
                    body->SetLinearVelocity({0, 0});
                    //Ignore gravity
                    body->SetGravityScale(0);
                }
            }  

            else
            {
                if(inAir)
                {
                    //Restore effect of gravity on body
                    body->SetGravityScale(1);
                    body->ApplyForce(body->GetMass()*world->GetGravity(), body->GetWorldCenter(), true);
                }
                
                //Disable inAir on hitting ground
                //TODO: Do this by getting position of ground wall
                if( float(body->GetPosition().y-HEIGHT) <= float(GROUND_POS + 0.2) )
                    inAir = false;
            }
        }

        float getXPos()
        { 
            return body->GetPosition().x - WIDTH;  
        }

        float getYPos()
        { 
            return body->GetPosition().y - HEIGHT;  
        }
};

//The ground, and possibly upper ceiling
class Wall: public PhysicalObject
{
    static constexpr float WIDTH = 200.0f;
    static constexpr float HEIGHT = 5.0f;
    
    public:
        Wall(b2World *world, Config initConfig): PhysicalObject(world) 
        {
            //Static body by default

            bodyDef.position.Set(initConfig.x, initConfig.y);
            body = world->CreateBody(&bodyDef);   

            shape.SetAsBox(WIDTH, HEIGHT);
            body->CreateFixture(&shape, 0.0f);
        }

        ~Wall() {}

        float getHeight()
        {
        	return HEIGHT;
        }
};

////////////////////////////////////////////////////////////
///////////////////// OBSTACLE RELATED /////////////////////
////////////////////////////////////////////////////////////

/* Represents a single letter. 
Holds a set of bodies corresponding to the Morse of the letter.
A dot is a single triangular body.
A dash is a set of three smaller triangles. */
class Obstacle
{
    //Max of 5 characters in a Morse letter
    //Worst case of three obstacles for each
    static const int MAX_BODIES = 15;

    /* Use only even numbers for width 
    Because we use integers and there is a division by 2 involved */

    //Width and height for dot obstacle
    static constexpr float DOT_WIDTH = 0.9;
    static constexpr float DOT_HEIGHT = 0.8;

    //Width and height for dash obstacle
    static constexpr float DASH_WIDTH = 0.6;
    static constexpr float DASH_HEIGHT = 0.5;

    static const int speed = 10;     //Speed of each obstacles
    static const int spacing = 7;   //Spacing between obstacles

    float curPos;                     //Starting position of first body
    char curLetter;                 //The letter represented by this obstacle
    char lastMorseChar;             //Last letter of Morse characters

    float groundLevel = 5.0;
    
    b2World *world;
    b2Body *bodies[MAX_BODIES];

    //Creates a triangular kinematic body in bodies[] array at the index
    void createBody(int index, float base, float height, Config initConfig)
    {
        curPos = initConfig.x;

        //Define body
        b2BodyDef def;
        def.type = b2_kinematicBody;
        def.position.Set(initConfig.x, initConfig.y);
        def.angle = initConfig.angle;

        //Create the body in the world
        bodies[index] = world->CreateBody(&def);

        //Store data in body to identify during collision
        BodyID* id = new BodyID;
        id->isPlayer = false;
        id->isDot = false;
        if(base == DOT_WIDTH)
        	id->isDot = true;
        bodies[index]->SetUserData(id);

        //Triangle fixture vertices relative to BodyDef position as origin
        b2Vec2 vertices[3];
        vertices[0].Set(0.0, 0.0); //Bottom left
        vertices[1].Set((base/2.0), height);   //Top
        vertices[2].Set(base, 0.0); //Bottom right

        //Triangle shape
        b2PolygonShape shape;
        shape.Set(vertices, 3);

        //Define fixture with the shape
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 1;

        //Add fixture to body
        bodies[index]->CreateFixture(&fixtureDef);
        bodies[index]->SetLinearVelocity({-speed, 0});
    }

    void destroyBody(int index)
    {
        if(bodies[index]) 
        {
            world->DestroyBody(bodies[index]);
            bodies[index] = NULL;
        }
    }

    public:
        Obstacle() {}

        Obstacle(b2World *world, float startPos) 
        {
            this->world = world;
            curPos = startPos;

            for(int i = 0; i < MAX_BODIES; ++i)
                bodies[i] = NULL;
        }

        ~Obstacle() {}

        /* Calling this function will destroy existing bodies, and create 
        new ones to represent the new letter. Although this can be optimized 
        by doing checks such as checking for same letter, retain similar 
        bodies etc, it is not done for the sake of simplicity */
        void setLetter(char letter)
        {
            curLetter = letter;

            //Destory all existing bodies
            for(int i = 0; i < MAX_BODIES; ++i)
                destroyBody(i);

            string morseText = letterToMorse(letter);

            float pos = curPos;   //To handle positioning of bodies
            int index = -1;     //For adding bodies to the array

            for(int i = 0; i < morseText.length(); ++i)
            {
                //Create single body
                if(morseText[i] == '.')
                {
                    createBody(++index, DOT_WIDTH, DOT_HEIGHT, {pos, groundLevel, 0});
                    pos += DOT_WIDTH;
                }
                //Create three smaller bodies
                else 
                    for(int k = 0; k < 3; ++k)
                    {
                        createBody(++index, DASH_WIDTH, DASH_HEIGHT, {pos, groundLevel, 0});
                        pos += DASH_WIDTH;
                    }

                pos += spacing;
            }

            //Needed for getLastPos()
            lastMorseChar = morseText[morseText.length() - 1];
        } 

        //Destroys all bodies
        void reset()
        {
            for(int i = 0; i < MAX_BODIES; ++i)
                destroyBody(i);
        }

        //Set curPos value. Change will reflect only when setLetter is called.
        void setCurPos(int pos)
        {
            curPos = pos;
        }

        void setGroundLevel(int gLevel)
        {
        	groundLevel = gLevel;
        }

        //Returns right most coordinate of last body in the array(bottom right vertex)
        int getLastPos()
        {
            int ret;

            for(int i = MAX_BODIES-1; i >= 0; --i)
                if(bodies[i])
                {
                    ret = bodies[i]->GetPosition().x;

                    if(lastMorseChar == '.')
                        ret += DOT_WIDTH;
                    else
                        ret += DASH_WIDTH;

                    break;
                }

            return ret;
        }

        float getBodyPos(int index)
        {
        	if(bodies[index])
        		return bodies[index]->GetPosition().x;

        	return -1;
        }

        int getBodyType(int index)
        {
        	if(bodies[index])
        		return (static_cast<BodyID*>(bodies[index]->GetUserData()))->isDot;

        	return -1;
        }

        char getCurLetter()
        {
            return curLetter;
        }
};

//Manages obstacles. Set the string once and call update periodically.
class ObstacleManager
{
    static const int LEFT_BOUNDARY = -2;
    static const int BUFFER_SIZE = 4;
    static const int letter_spacing = 15;

    int curIndex;                       //Index of left most obstacle
    int textIndex;                      //Index of current letter in letterQueue
    string letterQueue;                 //String of letters
    Obstacle buffer[BUFFER_SIZE];       //Obstacles that will be recycled

    int numReset;                       //To keep track of number of times reset() has been called
    int groundLevel = 5;

    public:
        //Important: text should have atleast one character
        ObstacleManager() 
        {
            //init();
        }
        
        ~ObstacleManager() {}

        //Call to reinitialize. There is no reset method for ObstacleManager because the physics bodies are explicitly destroyed on collision.
        void init(b2World* world, string text)
        {
        	//Create the first obstacle with required x position
            buffer[0] = Obstacle(world, 40);
            buffer[0].setGroundLevel(groundLevel);
            buffer[0].setLetter(text[0]);
            textIndex = 0;                      //First letter has been set

            //Subsequent obstacles will follow the previous obstacle with given spacing
            for(int i = 1; (i < BUFFER_SIZE) && (textIndex < text.length()-1); ++i)
            {
                buffer[i] = Obstacle(world, buffer[i-1].getLastPos()+letter_spacing);
                buffer[i].setGroundLevel(groundLevel);
                buffer[i].setLetter(text[++textIndex]);
            }

            letterQueue = text;
            curIndex = 0;
            numReset = 0;
        }

        void update()
        {
            //Update only if there are obstacles left
            if(numReset < BUFFER_SIZE)
                //Check if leftmost obstacle has crossed beyond left edge
                if(buffer[curIndex].getLastPos() < LEFT_BOUNDARY)
                {
                    //There are more letters to display
                    if(textIndex < letterQueue.length()-1)
                    {
                        //Index of previous obstacle
                        int prev = curIndex - 1;
                        if(prev == -1)
                            prev = BUFFER_SIZE - 1;

                        //Next letter
                        buffer[curIndex].setCurPos(buffer[prev].getLastPos()+letter_spacing);
                        buffer[curIndex].setLetter(letterQueue[++textIndex]);
                    }
                    else
                    {
                        buffer[curIndex].reset();
                        ++numReset;
                    }

                    //Increment index to next obstacle
                    ++curIndex;
                    curIndex %= BUFFER_SIZE;
                }
        }

        void updateTriPos(float arr[60][2])
        {
        	int arrIndex = 0;
        	for(int i = 0; i < BUFFER_SIZE; ++i)
        	{
        		for(int k = 0; k < 15; ++k)
        		{
        			arr[arrIndex][0] = buffer[i].getBodyPos(k);
        			arr[arrIndex][1] = buffer[i].getBodyType(k);
        			++arrIndex;
        		}
        	}
        }

        void setGroundLevel(int level)
        {
        	groundLevel = level;
        }

        char getDisplayChar()
        {
            if(numReset < BUFFER_SIZE)
                return buffer[curIndex].getCurLetter();
            else
                return '!';
        }
};

////////////////////////////////////////////////////////////
////////////////////////// OTHERS //////////////////////////
////////////////////////////////////////////////////////////

//Class that has methods to listen to collisions
class ContactListener: public b2ContactListener
{
    bool collided;

    public:
        ContactListener() 
        {
            collided = false;
        }
        
        ~ContactListener() {}

        bool hasCollided()
        {
            return collided;
        }

        //Gets called on contact between two fixtures A and B
        void BeginContact(b2Contact* contact) 
        {
            //Get data from both A and B
            BodyID* aID = static_cast<BodyID*>(contact->GetFixtureA()->GetBody()->GetUserData());
            BodyID* bID = static_cast<BodyID*>(contact->GetFixtureB()->GetBody()->GetUserData());

            //Both are not null
            if(aID && bID)
                //One is player and the other is obstacle
                if( (aID->isPlayer==true && bID->isPlayer==false)
                    || (aID->isPlayer==false && bID->isPlayer==true) )
            	{
            		collided = true;
            		cout<<"Collide aithu\n";
            	}
        }

        void reset()
        {
        	collided = false;
        }
};

//Returns Morse string of '.' and '-' for given letter
//TODO: Add morse code for numbers
string letterToMorse(char let)
{
	//Reference: https://en.wikipedia.org/wiki/Morse_code#/media/File:International_Morse_Code.svg
	switch(let=toupper(let))
	{
		case 'A': return ".-";		case 'B': return "-...";
		case 'C': return "-.-.";	case 'D': return "-..";
		case 'E': return ".";		case 'F': return "..-.";
		case 'G': return "--.";		case 'H': return "....";
		case 'I': return "..";		case 'J': return ".---";
		case 'K': return "-.-";		case 'L': return ".-..";
		case 'M': return "--";		case 'N': return "-.";
		case 'O': return "---";		case 'P': return ".--.";
		case 'Q': return "--.-";	case 'R': return ".-.";
		case 'S': return "...";		case 'T': return "-";
		case 'U': return "..-";		case 'V': return "...-";
		case 'W': return ".--";		case 'X': return "-..-";
		case 'Y': return "-.--";	case 'Z': return "--..";
	}
	
	return "0";
}

////////////////////////////////////////////////////////////
/////////////////////////// MAIN ///////////////////////////
////////////////////////////////////////////////////////////

float32 timeStep = 1.0f / 60.0f;
int32 velocityIterations = 10;
int32 positionIterations = 10;

//Create box2d world with gravity
/*b2World m_world(b2Vec2(0, -80));

Player player(&m_world, {4, 14, 0});
Wall wall(&m_world, {0, 0, 0});
ObstacleManager manager;
ContactListener contactListener;*/

b2World* m_world;

Player* player;
Wall* wall;
ObstacleManager* manager;
ContactListener* contactListener;

bool needInit = true;

//For explanation, check physics.h
namespace R_physics
{
	float groundHeight = 5;//wall.getHeight();
	float playerWidth = 1.10/2.0;
	float playerHeight = 1.86/2.0;
	float dotWidth = 0.9;
	float dotHeight = 0.8;
	float dashWidth = 0.6;
	float dashHeight = 0.5;
    char curLetter = ' ';
    bool jumpForceOn = false;
    long long SCORE = 0;
    float triPos[60][2];
    
    float getPlayerX();
    float getPlayerY();
    void stepPhysics();
    void resetPhysics();
}

float R_physics::getPlayerX()
{
	cout<<"player.getXPos():\n";//<<player.getXPos()<<"\n";
	
	if(player)
		return (*player).getXPos();

	return 4;
}

float R_physics::getPlayerY()
{
	cout<<"player.getYPos():\n";//<<player.getYPos()<<"\n";
	if(player)
		return (*player).getYPos();

	return 0;
}

void R_physics::stepPhysics()
{
	//Initialize stuff for the first time
	if(needInit)
	{

		R_physics::SCORE = 0;

		m_world = new b2World(b2Vec2(0, -50));

		player = new Player(m_world, {4, 14, 0});
		wall = new Wall(m_world, {0, 0, 0});
		manager = new ObstacleManager();
		contactListener = new ContactListener();



		/*m_world.SetContactListener(&contactListener);
		player.setGroundPos(R_physics::groundHeight);
		manager.setGroundLevel(R_physics::groundHeight);
		manager.init(&m_world,"abcdefghijkl");*/
		(*m_world).SetContactListener(contactListener);
		(*player).setGroundPos(R_physics::groundHeight);
		(*manager).setGroundLevel(R_physics::groundHeight);
		(*manager).init(m_world,"abcdefghijkl");
		needInit = false;
	}

    //Make player jump if true
	(*player).setJump(jumpForceOn);

	R_physics::curLetter = toupper((*manager).getDisplayChar());
	(*manager).update();
	(*manager).updateTriPos(R_physics::triPos);
	for(int i=0; i<60; ++i)
		cout<<"{"<<R_physics::triPos[i][0]<<","<<R_physics::triPos[i][1]<<"},";
	cout<<"\n\n";

	(*m_world).Step(timeStep, velocityIterations, positionIterations);

	//On collision with obstacle
	if((*contactListener).hasCollided())
	{
		R_physics::resetPhysics();
		//Set as game over
	    R_states::STATE = R_states::GAMEOVER;
	}
}

void R_physics::resetPhysics()
{

	cout<<"Start reset\n";
	needInit = true;
	R_physics::curLetter = ' ';
	R_physics::jumpForceOn = false;

	//(*player).setPos(4, 0);
	/*player.setJump(false);
	contactListener.reset();
	player.setPos(4, 0);

	//Destroy all obstacles
    for ( b2Body* b = m_world.GetBodyList(); b; b = b->GetNext())
    {
        BodyID* id = static_cast<BodyID*>(b->GetUserData());
        if(id && id->isPlayer==false)
            m_world.DestroyBody(b);
    }*/

    cout<<"Start delete\n";
    delete player;
    delete wall;
    delete contactListener;
    delete manager;
    delete m_world;
    player=NULL;
    wall=NULL;
    contactListener=NULL;
    manager=NULL;
    m_world=NULL;
    cout<<"End delete\n";
}