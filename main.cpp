///////////////////////////////Requires opengl 1.2/////////////////////////////
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/freeglut.h>
#include <GLUT/glext.h>
#else
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#endif
#include <stdlib.h>
#include <cmath>
#include "lib/lodepng.h"
#include "lib/lodepng.cpp"
#include <iostream>
int HEIGHT = 600;
int WIDTH = 800;
GLuint texname;
int frame=0,time,timebase=0;
using namespace std;
namespace R_settings
{
	bool ANTIALIAS=true;
}
namespace R_states
{
    const int MENU=0;
    const int GAME=1;
    const int GAMEOVER=2;

    int STATE=MENU;
}
namespace R_images
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
namespace R_keys
{
    bool UP=false;
    int ENTER=0;
    void menu_key(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case 27 :
            case 'q':
                exit(0); break;
            case 13:
                R_states::STATE=R_states::GAME; break;

        }
    }
    void menu_splkey(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case GLUT_KEY_F11:glutFullScreenToggle();break;
        }
    }
    void game_key(unsigned char key, int x, int y)
    {
        switch (key)
        {
        }
    }
    void game_keyup(unsigned char key, int x, int y)
    {
        switch (key)
        {
        }
    }
    void game_splkey(unsigned char key, int x, int y)
    {
        switch (key)
        {
             case GLUT_KEY_UP: UP=true; break;
             case GLUT_KEY_F11:glutFullScreenToggle();break;
        }
    }
     void game_splkeyup(unsigned char key, int x, int y)
    {
        switch (key)
        {
             case GLUT_KEY_UP: UP=false; break;
        }
    }
    static void key(unsigned char key, int x, int y)
    {
        //key down for normal keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            menu_key(key,x,y);break;
            case R_states::GAME:
                            game_key(key,x,y);break;
        }

        //glutPostRedisplay();
    }
    static void keyup(unsigned char key, int x, int y)
    {
        //key up for normal keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            break;
            case R_states::GAME:
                            game_keyup(key,x,y);break;
        }
    }
    static void splkey(int key, int x, int y)
    {
        //key down for special keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            menu_splkey(key,x,y);break;
            case R_states::GAME:
                            game_splkey(key,x,y);break;
        }
    }
    static void splkeyup(int key, int x, int y)
    {
        //key up for special keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            break;
            case R_states::GAME:
                            game_splkeyup(key,x,y);break;
        }
    }


}

namespace R_mouse
{
    void menu_mouse(int button,int state,int x,int y)
    {
        if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
        {
            printf("LEFT DOWN at (%d,%d)\n",x,y);
        }
    }
    static void mouse(int button,int state,int x,int y)
    {
        /*
                    Our co-ordinate system || Mouse co-ordinate system
            +HEIGHT ^                      ||   0    x+    +WIDTH
                    |                      ||   ----------->
                  +y|                      ||   |
                    |                      || +y|
                    ---------->            ||   |
                   0    x+    +WIDTH       ||   v +HEIGHT

                    So we need to translate the point(x,y) to our co-ordinate system
        */
        y=abs(y-HEIGHT);
        switch(R_states::STATE)
        {
            case R_states::MENU:
                menu_mouse(button,state,x,y);

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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, R_images::samWidth[0], R_images::samHeight[0],
                0, GL_RGBA, GL_UNSIGNED_BYTE, &R_images::sam[v][0]);
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
//    if(R_keys::ENTER>0)
//    {
//        R_keys::ENTER=0;
//        R_states::STATE=R_states::GAME;
//        return;
//    }
    //Thickness of font
    glLineWidth(5);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-250,HEIGHT-110,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse");
    //cout<<glutStrokeHeight(GLUT_STROKE_ROMAN)<<" "<<glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse")<<endl;
    glPopMatrix();

    //Thickness of font
    glLineWidth(2);
    glColor3ub(0x42,0x42,0x42);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-150,HEIGHT-150,0);
    glScalef(.1f,.1f,0.0f);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"Press Enter to Start, press Q to quit");
    glPopMatrix();

    glPushMatrix();
    glRasterPos2i(WIDTH/2-(R_images::logoWidth/2),0);
    glDrawPixels(R_images::logoWidth,R_images::logoHeight, GL_RGBA, GL_UNSIGNED_BYTE, &R_images::logo[0]);
    glPopMatrix();
}

void gameLoop()
{
    setLetter('R');
    /* enable texture.
    !!!!!!!!Very dangerous!!!!!!!. might affect other objects. disable before drawing other objects */
    glEnable(GL_TEXTURE_2D);
    setTexture();
    glPushMatrix();

    // should use global box variables for drawing character quad
    glBegin(GL_POLYGON);
        glTexCoord2d(0,0);  glVertex2f(0+50,0+50);
        glTexCoord2d(0,1);  glVertex2f(0+50,R_images::samHeight[0]+50);
        glTexCoord2d(1,1);  glVertex2f(R_images::samWidth[0]+50,R_images::samHeight[0]+50);
        glTexCoord2d(1,0);  glVertex2f(R_images::samWidth[0]+50,0+50);
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
    switch(R_states::STATE)
    {
    case R_states::MENU:
        menuLoop();break;
    case R_states::GAME:
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
    frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		printf("FPS:%4.2f\n",
			frame*1000.0/(time-timebase));
		timebase = time;
		frame = 0;
	}
    glutPostRedisplay();
}
void antialias()
{
	if(R_settings::ANTIALIAS)
	{
	    ///////////////////////Do anti alias/////////////////////////
	    //creates spaces (lines) bw polygon if multi sample does not work
	    glEnable(GL_POLYGON_SMOOTH);
	    //not sure enabling again is required
	    glEnable(GL_MULTISAMPLE);
	    ////////////////////////end of anti alias////////////////////
	    GLint iMultiSample = 0;
        GLint iNumSamples = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
        glGetIntegerv(GL_SAMPLES, &iNumSamples);
        printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);
	}
	else
    {
        glDisable(GL_MULTISAMPLE);
        printf("MSAA off\n");
    }
}
/* Program entry point */

int main(int argc, char *argv[])
{
    //should put this in init
    R_images::loadImages();
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GL_MULTISAMPLE);
    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
        //fallback if multisample is not possible
        glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
        R_settings::ANTIALIAS=false;
    }
    else
        glutSetOption(GLUT_MULTISAMPLE, 8);
    glClearColor(0.9568f,0.2627f,0.2117f,1.0f);
    glutCreateWindow("ReMorse");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
 	/*smoothen lines n points, doesn't seem to get affected by MULTISAMPLE.
 	works only if called after the BlendFunc*/
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    antialias();
    //set appropriate functions, may be we should put this in init as well
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(R_keys::key);
    glutKeyboardUpFunc(R_keys::keyup);
    glutSpecialFunc(R_keys::splkey);
    glutSpecialUpFunc(R_keys::splkeyup);
    glutMouseFunc(R_mouse::mouse);
    glutIdleFunc(idle);
    //make key not repeat events on long press
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutMainLoop();

    return EXIT_SUCCESS;
}
