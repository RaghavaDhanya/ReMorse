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

string letterToMorse(char let);

////////////////////////////////////////////////////////////
//////////////////////// STRUCTURES ////////////////////////
////////////////////////////////////////////////////////////

//Hold position and angle data
struct Config
{
	int x, y;
	int angle;
};

//To identify body during collision
struct BodyID
{
    //True if player, false if obstacle
    bool isPlayer;
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
                if(body->GetPosition().y < GROUND_POS)
                    inAir = false;
            }
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
            //Static body by default

            bodyDef.position.Set(initConfig.x, initConfig.y);
            body = world->CreateBody(&bodyDef);   

            shape.SetAsBox(WIDTH, HEIGHT);
            body->CreateFixture(&shape, 0.0f);
        }

        ~Wall() {}
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
    static const int DOT_WIDTH = 6;
    static const int DOT_HEIGHT = 5;

    //Width and height for dash obstacle
    static const int DASH_WIDTH = 4;
    static const int DASH_HEIGHT = 3;

    static const int speed = 15;     //Speed of each obstacles
    static const int spacing = 7;   //Spacing between obstacles

    int curPos;                     //Starting position of first body
    char curLetter;                 //The letter represented by this obstacle
    char lastMorseChar;             //Last letter of Morse characters
    
    b2World *world;
    b2Body *bodies[MAX_BODIES];

    //Creates a triangular kinematic body in bodies[] array at the index
    void createBody(int index, int base, int height, Config initConfig)
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
        bodies[index]->SetUserData(id);

        //Triangle fixture vertices relative to BodyDef position as origin
        b2Vec2 vertices[3];
        vertices[0].Set(0, 0); //Bottom left
        vertices[1].Set((base/2), height);   //Top
        vertices[2].Set(base, 0); //Bottom right

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

        Obstacle(b2World *world, int startPos) 
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

            int pos = curPos;   //To handle positioning of bodies
            int index = -1;     //For adding bodies to the array

            for(int i = 0; i < morseText.length(); ++i)
            {
                //Create single body
                if(morseText[i] == '.')
                {
                    createBody(++index, DOT_WIDTH, DOT_HEIGHT, {pos, 0, 0});
                    pos += DOT_WIDTH;
                }
                //Create three smaller bodies
                else 
                    for(int k = 0; k < 3; ++k)
                    {
                        createBody(++index, DASH_WIDTH, DASH_HEIGHT, {pos, 0, 0});
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

    public:
        //Important: text should have atleast one character
        ObstacleManager(b2World *world, string text) 
        {
            //Create the first obstacle with required x position
            buffer[0] = Obstacle(world, 0);
            buffer[0].setLetter(text[0]);
            textIndex = 0;                      //First letter has been set

            //Subsequent obstacles will follow the previous obstacle with given spacing
            for(int i = 1; (i < BUFFER_SIZE) && (textIndex < text.length()-1); ++i)
            {
                buffer[i] = Obstacle(world, buffer[i-1].getLastPos()+letter_spacing);
                buffer[i].setLetter(text[++textIndex]);
            }

            letterQueue = text;
            curIndex = 0;
            numReset = 0;
        }
        
        ~ObstacleManager() {}

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
                    collided = true;
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

int main()
{
	for(char l='a'; l<='z'; ++l)
	{
		cout<<l<<": "<<letterToMorse(l)<<"\n";
	}
	cout<<"\nFin\n";

	return 0;
}