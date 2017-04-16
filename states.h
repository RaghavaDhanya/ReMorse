namespace R_states
{
	// well these are self explanatory
	// don't use extern for const definitions
    const int MENU=0;
    const int GAME=1;
    const int GAMEOVER=2;
    const int PAUSE=3;

    extern int STATE;
    void setState(int);
}
