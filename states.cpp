namespace R_states
{
    const int MENU=0;
    const int GAME=1;
    const int GAMEOVER=2;
    const int PAUSE=3;

    int STATE=0;
    void setState(int state)
    {
        STATE=state;
    }
}
