
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/freeglut.h>
#include <stdlib.h>
#include "lib/lodepng.h"
#include "lib/lodepng.cpp"
#include <iostream>
int HEIGHT = 480;
int WIDTH = 640;
bool PRESSED=false;
using namespace std;
namespace Rstates
{
    const int MENU=0;
    const int GAME=1;
    const int GAMEOVER=2;

    int STATE=MENU;
}
namespace Rimages
{
    const char* logoName="morse.png";
    vector<unsigned char> logo;
    unsigned logoWidth=326;
    unsigned logoHeight=326;
    void invert(vector<unsigned char> &img,const unsigned width,const unsigned height)
    {
        unsigned char *imagePtr = &img[0];
        unsigned char *top = NULL;
        unsigned char *bottom = NULL;
        unsigned char temp = 0;
        for( int h = 0; h <(int) height/2; ++h )

        {

            top = imagePtr + h * width * 4;
            bottom = imagePtr + (height - h - 1) * width*4;
            for( int w = 0; w < (int)width*4; ++w )
            {
                temp = *top;
                *top = *bottom;
                *bottom = temp;
                ++top;
                ++bottom;
            }
        }
        }
    void loadImages()
    {
        int error;
        if((error=lodepng::decode(logo,logoWidth,logoHeight,logoName)))
        {
            cout<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(logo,logoWidth,logoHeight);

    }

}
//Set a letter on top of screen, used during game loop
void setLetter(char ch)
{
    glLineWidth(5);
    glColor3ub(0x42,0x42,0x42);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-50,HEIGHT-110,0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)ch);
    //glutStrokeString(GLUT_STROKE_MONO_ROMAN,(unsigned char*)"abcdef");
    glPopMatrix();
}
void menuLoop()
{
    glLineWidth(5);
    //glColor3ub(0x42,0x42,0x42);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-250,HEIGHT-110,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse");
    glPopMatrix();
    glRasterPos2i(WIDTH/2-(Rimages::logoWidth/2),0);
    glDrawPixels(Rimages::logoWidth,Rimages::logoHeight, GL_RGBA, GL_UNSIGNED_BYTE, &Rimages::logo[0]);
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
    //setLetter('R');
    switch(Rstates::STATE)
    {
    case Rstates::MENU:
        menuLoop();break;
    }
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
    Rimages::loadImages();
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH,HEIGHT);
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
//    glEnable(GL_MULTISAMPLE);
    //make key not repeat events on long press
    glutSetKeyRepeat(0);

    glutMainLoop();

    return EXIT_SUCCESS;
}
