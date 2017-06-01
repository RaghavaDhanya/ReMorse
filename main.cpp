///////////////////////////////Requires opengl 1.2/////////////////////////////
#include <string>
#include <sstream>
#include "remorse.h"
#include "lib/lodepng.h"
#include "lib/lodepng.cpp"
#include "states.h"
#include "keys.h"
#include "timer.h"
#include "physics.h"

// THINK:maybe height and width should be in settings?
int HEIGHT = 600;
int WIDTH = 800;
auto reInitBuffer = glutStrokeCharacter;
//To scale coordinates from physics world to graphics
float B2_SCALEX = 100.0;
float B2_SCALEY = 100.0;
float B2_OFFSETX = -225.0;
float B2_OFFSETY = -400.0;

//THINK: where to put score, may be in the values given by backend
auto BUF1 = GLUT_STROKE_ROMAN;

// THINK:Where the hell do I keep this texname variable?
// may be make static put inside the function? BTW this for loading texture
GLuint texname;
using namespace std;
namespace R_settings
{
	// TODO:put more things here
    bool ANTIALIAS=true;
    int MINWIDTH=640;
    int MINHEIGHT=550;
}

namespace R_images
{
	// THINK:not sure if this is the right way to store images and its properties
    const char* logoName="res/morse.png";
    vector<unsigned char> logo;
    unsigned logoWidth;
    unsigned logoHeight;
    const char* samName[]={"res/sam01.png","res/sam02.png","res/sam03.png","res/sam04.png"};
    vector<unsigned char> sam[4];
    unsigned samWidth[4];
    unsigned samHeight[4];

    /** OpenGL seems to draw images vertically flipped
    	this function inverts our data so that it displays correctly
    	@param img is our image data vector
    	@param width is our image width
    	@param height is our image height
    */
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
    /** Loads all required images for our game
    */
    void loadImages()
    {
    	//THINK:we are doing the same if else many times may be make a function?
        int error;
        if((error=lodepng::decode(logo,logoWidth,logoHeight,logoName)))
        {
            cout<<logoName<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(logo,logoWidth,logoHeight);
        if((error=lodepng::decode(sam[0],samWidth[0],samHeight[0],samName[0])))
        {
            cout<<samName[0]<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[0],samWidth[0],samHeight[0]);
        if((error=lodepng::decode(sam[1],samWidth[1],samHeight[1],samName[1])))
        {
            cout<<samName[1]<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[1],samWidth[1],samHeight[1]);
        if((error=lodepng::decode(sam[2],samWidth[2],samHeight[2],samName[2])))
        {
            cout<<samName[2]<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[2],samWidth[2],samHeight[2]);
        if((error=lodepng::decode(sam[3],samWidth[3],samHeight[3],samName[3])))
        {
            cout<<samName[3]<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[3],samWidth[3],samHeight[3]);
    }

}
/** Sets current texture to given image
    @param img is image vector that has already been loaded
    @param width is width of the image
    @param height is height of image
*/
void setTexture(vector<unsigned char> img, unsigned width, unsigned height)
{
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // without this texture darkens
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, &img[0]);
}
/**
    Set a letter on top of screen, used during game loop
    @param ch specifies the character to be displayed
*/
void setLetter(char ch)
{
    glLineWidth(3);
    glColor3ub(0x42,0x42,0x42);
    //push and pop is required in model view!
    glPushMatrix();
    glTranslatef(WIDTH/2.0-50,HEIGHT-110,0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)ch);
    glPopMatrix();
}
/**
   Draw a Button at 0,0
   @param str specifies the string to be shown in button
   @param outlined specifies outline, mostly used to show selection
*/
void drawButton(const char* str,bool outlined)
{
	// Currently all our buttons use .3x of size of font
	// anyway we can scale it by calling scalef before drawButton
	// all values found by trial and error method
    float width=glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)str)*.3;
    float height=glutStrokeHeight(GLUT_STROKE_ROMAN)*.3;
    glColor3ub(0x42,0x42,0x42);
    glBegin(GL_POLYGON);
        glVertex2f(0,0);
        glVertex2f(0,height);
        glVertex2f(width+10,height);
        glVertex2f(width+10,0);
    glEnd();
    if(outlined)
    {
    	// Draw outline if specified
        glColor3ub(0,0,0);
        glLineWidth(5);
        glBegin(GL_LINE_LOOP);
            glVertex2f(0,0);
            glVertex2f(0,height);
            glVertex2f(width+10,height);
            glVertex2f(width+10,0);
        glEnd();
    }
    glLineWidth(2);
    glColor3ub(0xF4,0x43,0x36);
    glTranslatef(5,7,0);
    glScalef(.3,.3,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)str);
}
void bufferRegenerate()
{
    glColor3ub(0x22,0x22,0x22);
    glPushMatrix();
    glTranslatef(0,10,0);
    glScalef(.15,.12,0);
    glLineWidth(2);
    int buf_offset = 60;
    for(int i = 0; i < 16; ++i)
        reInitBuffer(BUF1,(char)(int)R_physics::triPos[i+buf_offset][0]);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,25,0);
    glScalef(.15,.12,0);
    glLineWidth(2);
    buf_offset = 76;
    for(int i = 0; i <9; ++i)
        reInitBuffer(BUF1,(char)(int)R_physics::triPos[i+buf_offset][0]);
    glPopMatrix();
}
float getButtonWidth(const char* str)
{
    return glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)str)*.3+10;
}
float getButtonHeight(const char* str)
{
    return glutStrokeHeight(GLUT_STROKE_ROMAN)*.3;
}

//return graphics scaled values for physics coordinates
float getScaled(float val, bool x)
{
	if(x)
		return val*B2_SCALEX + B2_OFFSETX;

	return val*B2_SCALEY + B2_OFFSETY;
}

void menuLoop()
{
    glClear(GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Thickness of font
    glLineWidth(3);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-250,HEIGHT-110,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse");
    glPopMatrix();

    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("PLAY")/2.0,HEIGHT-200,0);
    drawButton("PLAY",R_keys::CURSOR==0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("QUIT")/2.0,HEIGHT-260,0);
    drawButton("QUIT",R_keys::CURSOR==1);
    glPopMatrix();
    bufferRegenerate();
    glPushMatrix();
    glRasterPos2i(WIDTH/2-(R_images::logoWidth/2),0);
    glDrawPixels(R_images::logoWidth,R_images::logoHeight, GL_RGBA, GL_UNSIGNED_BYTE, &R_images::logo[0]);
    glPopMatrix();

    glFlush();
}

void gameLoop()
{
    glClear(GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setLetter(R_physics::curLetter);

    //display score
    //why StringStream? Cuz to_string() doesn't freaking work in mingw compiler
    glLineWidth(3);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(5,HEIGHT-40,0);
    glScalef(.3,.3,0);
    ostringstream stm;
    stm<<R_physics::SCORE;
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)stm.str().c_str());
    stm.str("");
    glPopMatrix();

    glPushMatrix();
    stm<<"$"<<R_physics::HIGHSCORE;
    glTranslatef(WIDTH- getButtonWidth(stm.str().c_str()),HEIGHT-40,0);
    glScalef(.3,.3,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)stm.str().c_str());
    glPopMatrix();
    /* enable texture.
    !!!!!!!!Very dangerous!!!!!!!. might affect other objects. disable before drawing other objects */
    glEnable(GL_TEXTURE_2D);
    {
        //scoping so that these variables aren't accessible elsewhere
        int sel=rand()%2;
        int i=(R_physics::jumpForceOn?2:0);
        setTexture(R_images::sam[i+sel],R_images::samWidth[i+sel],R_images::samHeight[i+sel]);
    }
    glPushMatrix();

    glBegin(GL_POLYGON);
    {
        //scoping so that these variables aren't accessible elsewhere
    	float p00x = getScaled(R_physics::getPlayerX(), true);
    	float p00y = getScaled(R_physics::getPlayerY(), false);
        float p01x = p00x;
        float p01y = getScaled(R_physics::getPlayerY()+R_physics::playerHeight*2.0, false);
        float p11x = getScaled(R_physics::getPlayerX()+R_physics::playerWidth*2.0, true);
        float p11y = p01y;
        float p10x = p11x;
        float p10y = p00y;

    	glTexCoord2d(0,0);  glVertex2f(p00x, p00y);
        glTexCoord2d(0,1);  glVertex2f(p01x, p01y);
        glTexCoord2d(1,1);  glVertex2f(p11x, p11y);
        glTexCoord2d(1,0);  glVertex2f(p10x, p10y);
    }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    //draw ground...
    glPushMatrix();
    glBegin(GL_POLYGON);
        glColor3ub(0xF4,0x43,0x36);
        glVertex2f(0,0);
        glVertex2f(WIDTH,0);

        glColor3ub(0xC6,0x28,0x28);

        float g_height = getScaled(R_physics::groundHeight, false);
        glVertex2f(WIDTH,g_height);
        glVertex2f(0,g_height);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glColor3ub(0x00,0x00,0x00);
    glBegin(GL_TRIANGLES);
    for(int i=0;i<60;i++)
    {
        if(R_physics::triPos[i][0]!=-1)
        {
            float wid=R_physics::dotWidth;
            float hei=R_physics::dotHeight;
            if(R_physics::triPos[i][1]==0)
            {
                wid=R_physics::dashWidth;
                hei=R_physics::dashHeight;
            }

            float x0=(float)R_physics::triPos[i][0]-wid/2.0;
            float y0=(float)R_physics::groundHeight;
            float x1=x0+wid/2.0;
            float y1=y0+hei;
            float x2=x0+wid;
            float y2=y0;
            glVertex2f(getScaled(x0,true),getScaled(y0,false));
            glVertex2f(getScaled(x1,true),getScaled(y1,false));
            glVertex2f(getScaled(x2,true),getScaled(y2,false));
        }
    }
    glEnd();
    glPopMatrix();
    glFlush();
}

void pauseLoop()
{
    glClear(GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLineWidth(3);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-250,HEIGHT-110,0);
    glScalef(.5,.5,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"Game Paused!");
    glPopMatrix();

    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("RESUME")/2.0,HEIGHT-200,0);
    drawButton("RESUME",R_keys::CURSOR==0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("MENU")/2.0,HEIGHT-260,0);
    drawButton("MENU",R_keys::CURSOR==1);
    glPopMatrix();
    glFlush();

}
void overLoop()
{
    glClear(GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLineWidth(3);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-210,HEIGHT-110,0);
    glScalef(.5,.5,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"Game Over!");
    glPopMatrix();
    glLineWidth(2);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-210,HEIGHT-210,0);
    glScalef(.2,.2,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"Press any key to continue.");
    glPopMatrix();
    glFlush();
}
static void resize(int width, int height)
{
    // check if window size is too small, call reshape appropriately
    // THINK/TODO: may be we should find a scale factor or something and scale each objects
    if(width<R_settings::MINWIDTH && height<R_settings::MINHEIGHT)
        glutReshapeWindow(R_settings::MINWIDTH,R_settings::MINHEIGHT);
    else if(width<R_settings::MINWIDTH)
        glutReshapeWindow(R_settings::MINWIDTH,height);
    else if(height<R_settings::MINHEIGHT)
        glutReshapeWindow(width,R_settings::MINHEIGHT);
    else
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

}

static void display(void)
{
    static int frame=0,curtime,timebase=0;
    // Draw stuff here
    switch(R_states::STATE)
    {
    case R_states::MENU:
        menuLoop();break;
    case R_states::GAME:
        gameLoop();break;
    case R_states::PAUSE:
        pauseLoop();break;
    case R_states::GAMEOVER:
        overLoop();
        break;
    }
    // FPS calculation
    frame++;
    curtime=glutGet(GLUT_ELAPSED_TIME);
    if (curtime - timebase > 1000) {
        printf("FPS:%4.2f\n",
            frame*1000.0/(curtime-timebase));
        timebase = curtime;
        frame = 0;
    }
    glutSwapBuffers();
}


static void idle(void)
{
    // display opengl error for debugging
    if (GLenum err = glGetError())
    {
      cerr << "OpenGL ERROR: " << gluErrorString(err) << endl;
    }
    //glutPostRedisplay();
}
/**
    Do anti alias if set in settings
*/
void antialias()
{
    if(R_settings::ANTIALIAS)
    {
        ///////////////////////Do anti alias/////////////////////////
        // creates spaces (lines) bw polygon if multisample does not work
        glEnable(GL_POLYGON_SMOOTH);
        // THINK:not sure enabling again is required?
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
    //TODO:should put this in init
    R_images::loadImages();
    glGenTextures(1, &texname);
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
     /* smoothen lines n points, doesn't seem to get affected by MULTISAMPLE.
     works only if called after the BlendFunc */
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    antialias();
    // make cursor invisible
    glutSetCursor(GLUT_CURSOR_NONE);
    // set appropriate functions, THINK::may be we should put this in init as well
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(R_keys::key);
    glutKeyboardUpFunc(R_keys::keyup);
    glutSpecialFunc(R_keys::splkey);
    glutSpecialUpFunc(R_keys::splkeyup);
    glutTimerFunc(17,timer,UPDATE);
    // glutMouseFunc(R_mouse::mouse);
    glutIdleFunc(idle);
    // make key not repeat events on long press
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutMainLoop();

    return EXIT_SUCCESS;
}
