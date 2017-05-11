#include "remorse.h"
#include "states.h"
#include "timer.h"
#include "physics.h"

/** Our timer function decides when to call update
    @param value is specifies a why it was called
*/
void timer(int value)
{
    if(R_states::STATE==R_states::GAME && value==UPDATE)
    {
        //call update here

    	//Update physics
		R_physics::stepPhysics();

        R_physics::SCORE++;
        glutTimerFunc(12,timer,UPDATE);
    }
    //cout<<"timer::"<<SCORE<<endl;
}
