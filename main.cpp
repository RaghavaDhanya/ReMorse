
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

int HEIGHT = 480;
int WIDTH = 640;
bool PRESSED=false;

namespace states
{
    const int MENU=0;
    const int GAME=1;
    const int GAMEOVER=2;

    int STATE=MENU;
}

void setLetter(char ch)
{
    glLineWidth(2);
    glColor3ub(0x42,0x42,0x42);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-50,HEIGHT-110,0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)ch);
    glPopMatrix();
}
static void resize(int width, int height)
{
    glClearColor(0.9568f,0.2627f,0.2117f,1.0f);
    WIDTH=width;
    HEIGHT=height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,width,0,height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Draw stuff here
    setLetter('R');
    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    //key down for normal keys
    switch (key)
    {
        case 27 :
        case 'q': exit(0);break;

    }

    glutPostRedisplay();
}
static void keyup(unsigned char key, int x, int y)
{
    //key up for normal keys
    switch (key)
    {

    }
}
static void splkey(int key, int x, int y)
{
    //key down for special keys
    switch (key)
    {
        case GLUT_KEY_UP: PRESSED=true; break;

    }
}
static void splkeyup(int key, int x, int y)
{
    //key up for special keys
    switch (key)
    {
        case GLUT_KEY_UP: PRESSED=false; break;

    }
}


static void idle(void)
{
    glutPostRedisplay();
}


/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutCreateWindow("ReMorse");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyup);
    glutSpecialFunc(splkey);
    glutSpecialUpFunc(splkeyup);
    glutIdleFunc(idle);

    //Do anti alias
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
//  glEnable(GL_MULTISAMPLE);
    //make key not repeat events on long press
    glutSetKeyRepeat(0);

    glutMainLoop();

    return EXIT_SUCCESS;
}
