#include "remorse.h"
#include "states.h"
#include "timer.h"
/** Our timer function decides when to call update
    @param value is specifies a why it was called
*/
void timer(int value)
{
    extern int SCORE;
    if(R_states::STATE==R_states::GAME && value==UPDATE)
    {
        //call update here
        SCORE++;
        glutTimerFunc(17,timer,UPDATE);
    }
    //cout<<"timer::"<<SCORE<<endl;
}
