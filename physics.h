namespace R_physics
{
	extern float playerX, playerY;		//x and y coordinates of player in physics system. SCALE BEFORE USE.
	extern char curLetter;				//The current letter to be displayed on the screen
	extern bool jumpForceOn;			//Set to true to make player jump and hover. Set to false to make player fall if in the air

	void stepPhysics();					//Call this in each iteration to update the physics world
}