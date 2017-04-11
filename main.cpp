///////////////////////////////Requires opengl 1.2/////////////////////////////
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/freeglut.h>
#else
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#endif
#include <stdlib.h>
#include "lib/lodepng.h"
#include "lib/lodepng.cpp"
#include <iostream>
#define GL_CLAMP_TO_EDGE 0x812F
int HEIGHT = 600;
int WIDTH = 800;
GLuint texname;
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
    unsigned logoWidth=261;
    unsigned logoHeight=261;
    const char* samName[]={"sam01.png","sam02.png"};
    vector<unsigned char> sam[2];
    unsigned samWidth[]={110,110};
    unsigned samHeight[]={186,186};

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
        if((error=lodepng::decode(sam[0],samWidth[0],samHeight[0],samName[0])))
        {
            cout<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[0],samWidth[0],samHeight[0]);
        if((error=lodepng::decode(sam[1],samWidth[1],samHeight[1],samName[1])))
        {
            cout<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[1],samWidth[1],samHeight[1]);
    }

}
namespace Rkeys
{
    bool UP=false;
    int ENTER=0;
    static void key(unsigned char key, int x, int y)
    {
        //key down for normal keys
        switch (key)
        {
            case 27 :
            case 'q':
                exit(0); break;
            case 13:
                ENTER++; break;

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
            case GLUT_KEY_UP: UP=true; break;
            case GLUT_KEY_F11:glutFullScreenToggle();break;

        }
    }
    static void splkeyup(int key, int x, int y)
    {
        //key up for special keys
        switch (key)
        {
            case GLUT_KEY_UP: UP=false; break;

        }
    }


}

void setTexture()
{
    // should use a variable input to set specific texture
    int v=rand()%2;
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // without this texture darkens
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Rimages::samWidth[0], Rimages::samHeight[0],
                0, GL_RGBA, GL_UNSIGNED_BYTE, &Rimages::sam[v][0]);
}
//Set a letter on top of screen, used during game loop
void setLetter(char ch)
{
    glLineWidth(5);
    glColor3ub(0x42,0x42,0x42);
    //push and pop is required in model view!
    glPushMatrix();
    glTranslatef(WIDTH/2.0-50,HEIGHT-110,0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)ch);
    //glutStrokeString(GLUT_STROKE_MONO_ROMAN,(unsigned char*)"abcdef");
    glPopMatrix();
}
void menuLoop()
{
    if(Rkeys::ENTER>0)
    {
        Rkeys::ENTER=0;
        Rstates::STATE=Rstates::GAME;
        return;
    }

    glLineWidth(5);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-250,HEIGHT-110,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse");
    //cout<<glutStrokeHeight(GLUT_STROKE_ROMAN)<<" "<<glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse")<<endl;
    glPopMatrix();

    glLineWidth(2);
    glColor3ub(0x42,0x42,0x42);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-150,HEIGHT-150,0);
    glScalef(0.1,.1,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"Press Enter to Start, press Q to quit");
    glPopMatrix();

    glRasterPos2i(WIDTH/2-(Rimages::logoWidth/2),0);
    glDrawPixels(Rimages::logoWidth,Rimages::logoHeight, GL_RGBA, GL_UNSIGNED_BYTE, &Rimages::logo[0]);
}

void gameLoop()
{
    setLetter('R');
    // enable texture.
    // !!!!!!!!Very dangerous!!!!!!!. might affect other objects. disable before drawing other objects
    glEnable(GL_TEXTURE_2D);
    setTexture();
    glPushMatrix();

    // should use global box variables for drawing character quad
    glBegin(GL_POLYGON);
        glTexCoord2d(0,0);  glVertex2f(0+50,0+50);
        glTexCoord2d(0,1);  glVertex2f(0+50,Rimages::samHeight[0]+50);
        glTexCoord2d(1,1);  glVertex2f(Rimages::samWidth[0]+50,Rimages::samHeight[0]+50);
        glTexCoord2d(1,0);  glVertex2f(Rimages::samWidth[0]+50,0+50);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    //draw ground... use actual values later
    glPushMatrix();
    glBegin(GL_POLYGON);
        glColor3ub(0xF4,0x43,0x36);
        glVertex2f(0,0);
        glVertex2f(WIDTH,0);
        glColor3ub(0xC6,0x28,0x28);
        glVertex2f(WIDTH,50);
        glVertex2f(0,50);
    glEnd();
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
    //setLetter('R');
    switch(Rstates::STATE)
    {
    case Rstates::MENU:
        menuLoop();break;
    case Rstates::GAME:
        gameLoop();break;
    }
    glutSwapBuffers();
}


static void idle(void)
{
    //display opengl error for debugging
    if (GLenum err = glGetError())
    {
      cerr << "OpenGL ERROR: " << gluErrorString(err) << endl;
    }

    glutPostRedisplay();
}
void antialias()
{

    ///////////////////////Do anti alias/////////////////////////
    glutSetOption(GLUT_MULTISAMPLE, 8);
    //smoothen lines n points
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    //creates spaces (lines) bw polygon if multi sample does not work
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    ////////////////////////end of anti alias////////////////////
}
/* Program entry point */

int main(int argc, char *argv[])
{
    //should put this in init
    Rimages::loadImages();

    glutInit(&argc, argv);
    //for anti alias

    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE|GL_MULTISAMPLE);
    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
        //fallback if multisample is not possible
        glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    }
    else antialias();
    glClearColor(0.9568f,0.2627f,0.2117f,1.0f);
    glutCreateWindow("ReMorse");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //set appropriate functions, may be we should put this in init as well
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(Rkeys::key);
    glutKeyboardUpFunc(Rkeys::keyup);
    glutSpecialFunc(Rkeys::splkey);
    glutSpecialUpFunc(Rkeys::splkeyup);
    glutIdleFunc(idle);


    //make key not repeat events on long press
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutMainLoop();

    return EXIT_SUCCESS;
}
