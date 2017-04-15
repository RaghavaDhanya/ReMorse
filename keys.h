
namespace R_keys
{
    extern bool UP;
    extern int CURSOR;
    void menu_key(unsigned char key, int x, int y);
    void menu_splkey(unsigned char key, int x, int y);
    void pause_key(unsigned char key, int x, int y);
    void pause_splkey(unsigned char key, int x, int y);
    void game_key(unsigned char key, int x, int y);
    void game_keyup(unsigned char key, int x, int y);
    void game_splkey(unsigned char key, int x, int y);
    void game_splkeyup(unsigned char key, int x, int y);
    void key(unsigned char key, int x, int y);
    void keyup(unsigned char key, int x, int y);
    void splkey(int key, int x, int y);
    void splkeyup(int key, int x, int y);
}

