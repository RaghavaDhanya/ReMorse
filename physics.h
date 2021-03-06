namespace R_physics
{
	extern float groundHeight;
	extern float playerWidth;
	extern float playerHeight;
	extern float dotWidth;
	extern float dotHeight;
	extern float dashWidth;
	extern float dashHeight;
	extern float playerX, playerY;		
	extern char curLetter;				//The current letter to be displayed on the screen
	extern bool jumpForceOn;			//Set to true to make player jump and hover. Set to false to make player fall if in the air
	extern long long SCORE;
	extern float triPos[85][2];
	extern long long HIGHSCORE;
	float getPlayerX();
    float getPlayerY();					//x and y coordinates of player in physics system. SCALE BEFORE USE.
	void stepPhysics();					//Call this in each iteration to update the physics world
	void resetPhysics();
}