/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Moshball - This video game has several modes.
See the README for the complete write-up about the 
game.
*/

#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <conio.h>
//#include <AL/al.h>
//#include <AL/alc.h>
//#include <AL/alut.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "shader_utils.h"

#include "squareStadium.h"
#include "discStadium.h"
#include "sphereStadium.h"
#include "coin.h"
#include "triangle.h"
#include "gengar.h"

#include "cube2.h"
#include "sphere2.h"
#include "square2.h"
#include "square4.h"

#include "vertexattributes2.h"
#include "object2.h"
#include "square2.h"
#include "shader2.h"
#include "globals.h"
#include "fbo.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

using namespace std;using namespace glm;

//x (0=Normal, 1=night, 2=twilight), y(0=normal, 1=underwater, 2=fog)
ivec2 tod = ivec2(0, 0);

//Creates a class for maintaining the window information for easy access.
class Window
{
public:
	Window()
	{
		this->time_last_pause_began = this->total_time_paused = 0;
		this->normals = this->wireframe = this->paused = false;
		this->slices = 20;this->interval = 1000 / 120;this->window_handle = -1;
	}
	float time_last_pause_began;float total_time_paused;bool paused , wireframe, normals;
	int window_handle;int interval;int slices;ivec2 size;float window_aspect;
	vector<string> instructions, countdown, scores;
} window;

/////////////////////////////////////VARIABLE DECLARATIONS/////////////////////////////
//Menu decision variables
bool menuOn = true;int menuSelect = 0;bool singlePlayer = false;
bool twoPlayer = false;bool secondPlayerTurn = false;int stadiumSelect = 0;
bool weatherSelect = false;

bool loadCars = true;

int lap = 0;
int pt1 = 0; int pt2 = 0; int pt3 = 0;

//Values defining the position/rotation of the camera.
double RotatedX = 1;double RotatedY = 0;float transX = -243.0f; //-243.0f
float transY = 0;float transZ = -13;

float cpuPos = 0;
float* cpuX = new float[8];float* cpuZ = new float[8];float* cpuRotate = new float[8];

float velocity = 0; //Define the travel of the user
int score = 0; //The user's score for the game
bool caught = false; //If the enemy caught the user
float firstPersonTime, firstPersonScore; //Variables used for the 2 player game
bool startupTime = true; //Account for delay in loading the music
int gameSelect = 0; //Select amongst the game types
int viewPerspective = 0; //Select amongst 1st and 3rd person views
int weatherType = 0; //Select amongst the weather types

//Used for adjusting the y value for the lookAt function for the first and third person views.
float perspectiveOffset = 0.0f;
//Used for adjusting the z value for the lookAt function for the first and third person views.
float perspectiveStepBack = 50.0f;

bool countdownOn = false; //If the menu is off and the countdown is on
bool won = false; //If the user won
float* partX, *partY, *partOffset;int numParts; //Variables controlling the precipitation.

//Time control variables.
float current_timeDisplay, menuTime, countdownTime; 
string timeString;int first = 1;int menuFirst = 1;

//The projection and modelview matrices. 
mat4 projection, modelview;

//Mouse value/identification variables.
float mouseX = 0;float mouseY = 0;
float prevMouseY = (float)(window.size.y/2);

float maxSpeed = 1.6f; //Maximum velocity for user

//////////////////////////////////////TEXT RENDERING/////////////////////////////////////////////
////////Font variables//////////
GLuint program;GLint attribute_coord;GLint uniform_tex;GLint uniform_color;
struct point {GLfloat x;GLfloat y;GLfloat s;GLfloat t;};
GLuint vbo;FT_Library ft;FT_Face face;const char *fontfilename;
int init_resources()
{        /* Initialize the FreeType2 library */
        if(FT_Init_FreeType(&ft)){fprintf(stderr, "Could not init freetype library\n");return 0;}
        /* Load a font */
        if(FT_New_Face(ft, fontfilename, 0, &face)){fprintf(stderr, "Could not open font %s\n", fontfilename);return 0;}
        /* Compile and link the shader program */
        GLint link_ok = GL_FALSE;GLuint vs, fs;
        if ((vs = create_shader("text.vert", GL_VERTEX_SHADER))        == 0) return 0;
        if ((fs = create_shader("text.frag", GL_FRAGMENT_SHADER)) == 0) return 0;

        program = glCreateProgram();glAttachShader(program, vs);glAttachShader(program, fs);
        glLinkProgram(program);glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
        if (!link_ok) {fprintf(stderr, "glLinkProgram:");print_log(program);return 0;}

        const char* attribute_name;attribute_name = "coord";
        attribute_coord = glGetAttribLocation(program, attribute_name);
        if (attribute_coord == -1) {fprintf(stderr, "Could not bind attribute %s\n", attribute_name);return 0;}

        const char* uniform_name;uniform_name = "tex";
        uniform_tex = glGetUniformLocation(program, uniform_name);
        if (uniform_tex == -1){fprintf(stderr, "Could not bind uniform %s\n", uniform_name);return 0;}

        uniform_name = "color";uniform_color = glGetUniformLocation(program, uniform_name);
        if (uniform_color == -1){fprintf(stderr, "Could not bind uniform %s\n", uniform_name);return 0;}

        // Create the vertex buffer object
        glGenBuffers(1, &vbo);
        return 1;
}

/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
void render_text(const char *text, float x, float y, float sx, float sy) {
        const char *p;FT_GlyphSlot g = face->glyph;

        /* Create a texture that will be used to hold one "glyph" */
        GLuint tex;glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tex);glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(uniform_tex, 0);
        /* We require 1 byte alignment when uploading texture data */
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        /* Clamping to edges is important to prevent artifacts when scaling */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        /* Linear filtering usually looks best for text */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /* Set up the VBO for our vertex data */
        glEnableVertexAttribArray(attribute_coord);glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
        /* Loop through all characters */
        for(p = text; *p; p++) {
                /* Try to load and render the character */
                if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
                        continue;

                /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
                glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
                /* Calculate the vertex and texture coordinates */
                float x2 = x + g->bitmap_left * sx;float y2 = -y - g->bitmap_top * sy;
                float w = g->bitmap.width * sx;float h = g->bitmap.rows * sy;

                point box[4] = {
                        {x2,     -y2    , 0, 0}, {x2 + w, -y2    , 1, 0},
                        {x2,     -y2 - h, 0, 1}, {x2 + w, -y2 - h, 1, 1},
                };

                /* Draw the character on the screen */
                glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                /* Advance the cursor to the start of the next character */
                x += (g->advance.x >> 6) * sx;y += (g->advance.y >> 6) * sy;
        }
        glDisableVertexAttribArray(attribute_coord);glDeleteTextures(1, &tex);
}

void textDisplay(int textIndex, float windowX, float windowY, int timeOfDay, bool pokemon)
{
		glDisable(GL_CULL_FACE);
        float sx = (float)(2.0/windowX);
        float sy = (float)(2.0/windowY);

        glUseProgram(program);

        glClearColor(1.0,1.0,0.25,1);
       // glClear(GL_COLOR_BUFFER_BIT);

        /* Enable blending, necessary for our alpha texture */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLfloat realRed[4] = {1, 0, 0, 1.0};
        GLfloat red[4] = {1, 1, 0, 1};
        GLfloat transparent_green[4] = {0, 1, 0, 0.5};

        /* Set font size to 48 pixels, color to black */
        FT_Set_Pixel_Sizes(face, 0, 48);
		
		if(textIndex == 0){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Main Menu",            -1 + 8 * sx,   1 - 50 * sy,    sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Tiger Echo Plateau",            -1 + 8 * sx,   1 - 150 * sy,    sx, sy);
			render_text("(2) The Intergalactic Dome",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(3) Marine Layer Arena",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
			render_text("(4) Weather Select",            -1 + 8 * sx,   1 - 300 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 350 * sy,   sx, sy);
		}
		else if(textIndex == 1){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Weather Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) No weather",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Fog",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(3) Rain",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
			render_text("(4) Snow",            -1 + 8 * sx,   1 - 300 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 350 * sy,   sx, sy);
		}
		else if(textIndex == 2){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Player Options",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Single Player",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Two Players (Not implemented)",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
		}
		else if(textIndex == 3 || textIndex == 4 || textIndex == 5){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Game Type Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Moshball",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);	
			if(textIndex == 3){
				if(pokemon){render_text("(2) Avoid the Gengar",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}
				else{render_text("(2) Avoid the Tiger",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}}
			else if(textIndex == 4){
				if(pokemon){render_text("(2) Avoid the Magneton",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}
				else{render_text("(2) Avoid the Alien",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}}
			else if(textIndex == 5){
				if(pokemon){render_text("(2) Avoid the Tentacool",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}
				else{render_text("(2) Avoid the Shark",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);}}
			render_text("(3) Coin Time Attack",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 300 * sy,   sx, sy);
		}
		else if(textIndex == 6){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - View Perspective Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) First person view",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Third person view",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
		}
		else if(textIndex == 7){
			glUniform4fv(uniform_color, 1, realRed);
			render_text("Moshball - Game Type Select",            -1 + 8 * sx,   1 - 50 * sy,   sx, sy);
			glUniform4fv(uniform_color, 1, red);
			render_text("(1) Moshball (Timed)",            -1 + 8 * sx,   1 - 150 * sy,   sx, sy);
			render_text("(2) Coin Collection",            -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
			render_text("(b) Back",            -1 + 8 * sx,   1 - 250 * sy,   sx, sy);
		}
		else if(textIndex == 8){glUniform4fv(uniform_color, 1, realRed);render_text("Get Ready!",            -0.2f,  0,   sx, sy);}
		glUniform4fv(uniform_color, 1, red);
		if(timeOfDay == 0){render_text("(5) Daytime",      -1 + 8 * sx,   1 - 400 * sy,   sx, sy);}
		else if(timeOfDay == 1){render_text("(5) Night",      -1 + 8 * sx,   1 - 400 * sy,   sx, sy);}
		else if(timeOfDay == 2){render_text("(5) Twilight",      -1 + 8 * sx,   1 - 400 * sy,   sx, sy);}

		if(pokemon){render_text("(6) vs Pokemon",      -1 + 8 * sx,   1 - 450 * sy,   sx, sy);}
		else{render_text("(6) No Pokemon",      -1 + 8 * sx,   1 - 450 * sy,   sx, sy);}
		
		glDisable(GL_BLEND);
		glUseProgram(0);
		glEnable(GL_CULL_FACE);
		
		
}

void activeTextDisplay(const char* words, float x, float y)
{
		glDisable(GL_CULL_FACE);
        float sx = (float)(2.0/glutGet(GLUT_WINDOW_WIDTH));
        float sy = (float)(2.0/glutGet(GLUT_WINDOW_HEIGHT));

        glUseProgram(program);

        /* Enable blending, necessary for our alpha texture */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/* Set font size to 48 pixels, color to black */
        FT_Set_Pixel_Sizes(face, 0, 36);

		if(weatherType != 3 || tod.x == 1){GLfloat black[4] = {1, 1, 1, 1};glUniform4fv(uniform_color, 1, black);}
		else{GLfloat black[4] = {0,0,0,0.75};glUniform4fv(uniform_color, 1, black);}
		
		render_text(words,x,y,sx,sy); 
		
		glDisable(GL_BLEND);
		glUseProgram(0);
        //glFinish();
		glEnable(GL_CULL_FACE);
}

/*
/////////////////////////////////////AUDIO CONTROLS/////////////////////////////////////////////////////
 // Buffers to hold sound data.
ALuint Buffer;ALuint origBuffer;
// Sources are points of emitting sound.
ALuint Source;ALuint origSource;
/*
 * These are 3D cartesian vector coordinates. A structure or class would be
 * a more flexible of handling these, but for the sake of simplicity we will
 * just leave it as is.
 */
/*
// Position of the source sound.
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
// Velocity of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
// Position of the Listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
// Velocity of the Listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

/*
 * ALboolean LoadALData()
 *
 *         This function will load our sample data from the disk using the Alut
 *         utility and send the data into OpenAL as a buffer. A source is then
 *         also created to play that buffer.
 *//*
ALboolean LoadALData(ALbyte *song, bool looping){
            // Variables to load into.
	        ALenum format;ALsizei size;ALvoid* data;ALsizei freq;
            ALboolean loop = true; 

            // Load wav data into a buffer.
            alGenBuffers(1, &Buffer);
			if(alGetError() != AL_NO_ERROR)
                        return AL_FALSE;

    // Load any of your favourite wav song here
            alutLoadWAVFile(song, &format, &data, &size, &freq, &loop);
            alBufferData(Buffer, format, data, size, freq);
            alutUnloadWAV(format, data, size, freq);
			
            // Bind the buffer with the source.
            alGenSources(1, &Source);
            if(alGetError() != AL_NO_ERROR)
                        return AL_FALSE;
			
            alSourcei (Source, AL_BUFFER,   Buffer   );
            alSourcef (Source, AL_PITCH,    1.0      );
            alSourcef (Source, AL_GAIN,     1.0      );
            alSourcefv(Source, AL_POSITION, SourcePos);
            alSourcefv(Source, AL_VELOCITY, SourceVel);
			if(looping){
            alSourcei (Source, AL_LOOPING,  AL_TRUE     );
			}
			else{
			alSourcei (Source, AL_LOOPING,  AL_FALSE     );
			}

            // Do another error check and return.
            if(alGetError() == AL_NO_ERROR)
                        return AL_TRUE;
			
            return AL_FALSE;
}

/*
 * void SetListenerValues()
 *
 *         We already defined certain values for the Listener, but we need
 *         to tell OpenAL to use that data. This function does just that.
 *//*
void SetListenerValues(){
            alListenerfv(AL_POSITION,    ListenerPos);alListenerfv(AL_VELOCITY,    ListenerVel);
            alListenerfv(AL_ORIENTATION, ListenerOri);
}

/*
 * void KillALData()
 *
 *         We have allocated memory for our buffers and sources which needs
 *         to be returned to the system. This function frees that memory.
 *//*
void KillALData(){/*alDeleteBuffers(1, &Buffer);alDeleteSources(1, &Source);alDeleteBuffers(1, &origBuffer);alDeleteSources(1, &origSource);*///alutExit();}
/*
void init(ALbyte *song, bool loop){
            // Initialize OpenAL and clear the error bit. 
            alutInit(NULL, 0);alGetError();
			
            // Load the wav data. 
            if(LoadALData(song, loop) == AL_FALSE) //method 1: LoadALData()
            {
                printf("Error loading data.");
				return;
            }
			
            SetListenerValues(); //method 2: void SetListenerValues()
			
            // Setup an exit procedure. 
            atexit(KillALData); //method 3: void KillALData()
			if(loop){origSource = Source;}
			alSourcePlay(Source);

}*/

////////////////////////////////////OBJECT CREATION////////////////////////////////////
Sphere rain2;Sphere snow2;
DiscStadium stadium;Coin coin;Square4 sq4;
Gengar arena, renault, ferrari, lotus, redbull, mercedes, forceIndia, vMobile, mcLaren,
	itemBox, speedBoost;
Square2 tri2;FrameBufferObject fbo;
Cube2 skybox, skybox2, skybox3, skyboxUW;Sphere2 egg;

////////////////////////////////////MOUSE DETECTION & MANAGEMENT/////////////////////////
void mouseMovement(int x, int y) {
    mouseX=(float)x; //set lastx to the current x position
    mouseY=(float)y; //set lasty to the current y position
}

void mouseRotations(int stadium, int person){
	if(person == 1 || person == 3){
		//Turning controls
		if(mouseX < window.size.x/2){RotatedY -= abs((mouseX - window.size.x/2)/75);}
		else if(mouseX > window.size.x/2){RotatedY += abs((mouseX - window.size.x/2)/75);}
		//Forward & Backward Velocity controls
		/*if(mouseY < window.size.y/2 && velocity < maxSpeed*(2*abs(mouseY-(window.size.y/2))/window.size.y)){
			velocity += abs((mouseY - window.size.y/2)/1000);
		}
		else if(mouseY < window.size.y/2 && mouseY > prevMouseY){velocity += -abs((mouseY - window.size.y/2)/1000);}
		if(mouseY > window.size.y/2 && velocity > -maxSpeed*(2*abs(mouseY-(window.size.y/2))/window.size.y)){
			velocity += -abs((mouseY - window.size.y/2)/1000);
		}
		else if(mouseY > window.size.y/2 && mouseY < prevMouseY){
			velocity += abs((mouseY - window.size.y/2)/1000);
		}*/
	}
	prevMouseY = mouseY;
}

void moveCpu(){
	cpuX[0] = 215 * cos(-cpuPos);
	cpuZ[0] = 215 * sin(-cpuPos);
	cpuRotate[0] = 57.3 * cpuPos;
	cpuPos+=0.007f;
}

/////////////////////////////////////CAMERA CONTROLS//////////////////////////////////////////
//Moves the camera according to user inputs
glm::mat4 render(glm::mat4 Model){		
	Model = glm::rotate(Model, (GLfloat) RotatedX, glm::vec3(1,0,0));
	Model = glm::rotate(Model, (GLfloat) RotatedY, glm::vec3(0,1,0));
	Model = glm::translate(Model, glm::vec3(transX,0,0));    
	Model = glm::translate(Model, glm::vec3(0,0,transZ));    
	Model = glm::translate(Model, glm::vec3(0,transY,0));	
	glLoadMatrixf(glm::value_ptr(Model));
    return Model;
}

//Undoes the render function. 
glm::mat4 endRender(glm::mat4 Model){	
	Model = glm::translate(Model, glm::vec3(0,-transY,0));	
	Model = glm::translate(Model, glm::vec3(0,0,-transZ));
	Model = glm::translate(Model, glm::vec3(-transX,0,0));
	Model = glm::rotate(Model, (GLfloat) -RotatedY, glm::vec3(0,1,0));
	Model = glm::rotate(Model, (GLfloat) -RotatedX, glm::vec3(1,0,0));			
	glLoadMatrixf(glm::value_ptr(Model));
    return Model;
}

//Changes the value of the angle in which the camera should be rotated vertically.
void rotateX(double angle){
    if(!(RotatedX>=89 && angle>0) && !(RotatedX<=-89 && angle<0)){
        RotatedX += angle;
    }
}

//Changes the value of the angle in which the camera should be rotated horizontally.
void rotateY(double angle){
    RotatedY += angle;
}

//Deletes all of the created objects here.
void CloseFunc(){
	window.window_handle = -1;
	stadium.TakeDown();coin.TakeDown();arena.TakeDown();skyboxUW.TakeDown();fbo.TakeDown();rain2.TakeDown();
	tri2.TakeDown();skybox.TakeDown();skybox2.TakeDown();skybox3.TakeDown();egg.TakeDown();snow2.TakeDown();
	redbull.TakeDown();lotus.TakeDown();ferrari.TakeDown();renault.TakeDown();speedBoost.TakeDown();
	mercedes.TakeDown();mcLaren.TakeDown();forceIndia.TakeDown();vMobile.TakeDown();
	itemBox.TakeDown();
	sq4.TakeDown();
}

//Maintains aspect ratio when window is resized.
void ReshapeFunc(int w, int h){
	if (h > 0){window.size = ivec2(w, h);window.window_aspect = float(w) / float(h);}



	//stadium.setCamInternals(25.0f, window.window_aspect, 0.01f, 1200.0f);
}

void KeyboardFunc(unsigned char c, int x, int y){
	float current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	switch (c){
	case 'b':  //Shifts backwards through menu
		if(menuOn){
			if(stadiumSelect > 0){
				if(singlePlayer || twoPlayer){
					if(gameSelect > 0){gameSelect = 0;}
					else{
						singlePlayer = false;
						twoPlayer = false;
					}
				}
				else{
					stadiumSelect = 0;
					menuSelect = 0;
				}
			}
		}
		//Goes back out of the weather menu
		if(weatherSelect){
			weatherSelect = false;
		}
		break;

	case 'n':  //Moves user down one unit
		transY = transY + 1;
		if(stadiumSelect == 3 && (sqrt(dot(vec3(transX, transY, transZ), vec3(transX, transY, transZ)))) > 51.8){
			transY = transY - 1;
		}
		break;

	case 'u':  //Moves user up one unit
		transY = transY - 1;
		if(stadiumSelect == 3 && (sqrt(dot(vec3(transX, transY, transZ), vec3(transX, transY, transZ)))) > 51.8){
			transY = transY + 1;
		}
		break;

	case 'e':  //Forward
		if(velocity < 1.6){velocity += 0.02 + 0.1/(5+5*velocity); if(velocity > 1.6){velocity = 1.6;}}
		break;

	case 'd':  //Reverse
		if(velocity > -0.9){velocity -= 0.03;if(velocity < -0.9){velocity = -0.9;}}
		break;

	case 's':  //Stop
		if(velocity > 0){velocity -= 0.015;if(velocity < 0){velocity = 0;}}
		break;

	case 'i':  //Moves user forward one unit
		if(stadiumSelect == 1 || stadiumSelect == 2){
			transZ = (float)(transZ + 2*cos(-RotatedY*3.14/180));
			transX = (float)(transX + 2*sin(-RotatedY*3.14/180));
		}
		if(stadiumSelect == 3){
			transZ = (float)(transZ + 0.5*cos(-RotatedY*3.14/180)*cos(RotatedX*3.14/180));
			transX = (float)(transX + 0.5*sin(-RotatedY*3.14/180)*cos(RotatedX*3.14/180));
			transY = (float)(transY + 0.5*sin(RotatedX*3.14/180));
		}
		if(stadiumSelect == 2 && (sqrt(transX*transX + transZ*transZ)) > 52){
			transZ = (float)(transZ - 1*cos(-RotatedY*3.14/180));
			transX = (float)(transX - 1*sin(-RotatedY*3.14/180));
		}
		else if(stadiumSelect == 3 && (sqrt(dot(vec3(transX, transY, transZ), vec3(transX, transY, transZ)))) > 51.8){
			transZ = (float)(transZ - 0.5*cos(-RotatedY*3.14/180)*cos(RotatedX*3.14/180));
			transX = (float)(transX - 0.5*sin(-RotatedY*3.14/180)*cos(RotatedX*3.14/180));
			transY = (float)(transY - 0.5*sin(RotatedX*3.14/180));
		}
		break;

	case 'k':  //Moves user backward one unit
		transZ = (float)(transZ - 1*cos(-RotatedY*3.14/180));
		transX = (float)(transX - 1*sin(-RotatedY*3.14/180));
		if(stadiumSelect == 2 && (sqrt(transX*transX + transZ*transZ)) > 52){
			transZ = (float)(transZ + 1*cos(-RotatedY*3.14/180));
			transX = (float)(transX + 1*sin(-RotatedY*3.14/180));
		}
		else if(stadiumSelect == 3 && (sqrt(dot(vec3(transX, transY, transZ), vec3(transX, transY, transZ)))) > 51.8){
			transZ = (float)(transZ + 1*cos(-RotatedY*3.14/180));
			transX = (float)(transX + 1*sin(-RotatedY*3.14/180));
		}
		break;

	case 'j':  //Moves user left one unit
		transX = (float)(transX + 1*cos(RotatedY*3.14/180));
		transZ = (float)(transZ + 1*sin(RotatedY*3.14/180));
		if(stadiumSelect == 2 && (sqrt(transX*transX + transZ*transZ)) > 52){
			transX = (float)(transX - 1*cos(RotatedY*3.14/180));
			transZ = (float)(transZ - 1*sin(RotatedY*3.14/180));
		}
		else if(stadiumSelect == 3 && (sqrt(dot(vec3(transX, transY, transZ), vec3(transX, transY, transZ)))) > 51.8){
			transX = (float)(transX - 1*cos(RotatedY*3.14/180));
			transZ = (float)(transZ - 1*sin(RotatedY*3.14/180));
		}
		break;

	case 'l':  //Moves user right one unit
		transX = (float)(transX - 1*cos(RotatedY*3.14/180));
		transZ = (float)(transZ - 1*sin(RotatedY*3.14/180));
		if(stadiumSelect == 2 && (sqrt(transX*transX + transZ*transZ)) > 52){
			transX = (float)(transX + 1*cos(RotatedY*3.14/180));
			transZ = (float)(transZ + 1*sin(RotatedY*3.14/180));
		}
		else if(stadiumSelect == 3 && (sqrt(dot(vec3(transX, transY, transZ), vec3(transX, transY, transZ)))) > 51.8){
			transX = (float)(transX + 1*cos(RotatedY*3.14/180));
			transZ = (float)(transZ + 1*sin(RotatedY*3.14/180));
		}
		break;

	case 'p':
		if(!menuOn && !countdownOn){
			if (window.paused == true)
			{
				// Will be leaving paused state
				window.total_time_paused += (current_time - window.time_last_pause_began);
			}
			else
			{
				// Will be entering paused state
				window.time_last_pause_began = current_time;
			}
			window.paused = !window.paused;
		}
		break;

	case 'w':  //Toggles wireframe mode
		window.wireframe = !window.wireframe;
		break;

	case 'x':  //Exit the program
	case 27:
		glutLeaveMainLoop();
		return;
	case '1': 	
		//Select no weather and go back to the main menu
		if(weatherSelect){
			weatherSelect = false;weatherType = 0;
			if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}
		}
		//Select 1st person perspective in certain single player modes
		else if(menuOn && menuSelect > 0 && singlePlayer && gameSelect>1){
			viewPerspective = 1;
			perspectiveOffset = 0.0f;
			perspectiveStepBack = 1.0f;
			countdownOn = true;
			menuOn = false;
			transY = -1;
			RotatedX = 0;
		}
		//Select Moshball for the game in either mode (always 1st person)
		else if(menuOn && menuSelect > 0 && (singlePlayer || twoPlayer)){
			gameSelect = 1;
			viewPerspective = 1;perspectiveOffset = 0.0f;perspectiveStepBack = 1.0f;
			countdownOn = true;menuOn = false;transY = -1;RotatedX = 0;
		}
		//Select single player
		else if(menuOn && menuSelect > 0){singlePlayer = true;}
		//Select jungle stadium
		else if(menuOn && menuSelect == 0){
			menuSelect = 1;stadiumSelect = 1;
			if(tod.y != 2){tod.y = 0;}
		}
		return;
	case '2':  
		//Select fog and go back to the main menu
		if(weatherSelect){
			weatherType = 1;
			tod.y = 2;
			weatherSelect = false;			
		}
		//Select 3rd person view for single player games
		else if(menuOn && menuSelect > 0 && singlePlayer && gameSelect > 1){
			viewPerspective = 3;
			perspectiveOffset = 0.0f;
			perspectiveStepBack = 10.0f;
			countdownOn = true;
			menuOn = false;
			transY = 0;
		}
		//Select the avoid game for single player/ coin collection for 2 player
		else if(menuOn && menuSelect > 0 && (singlePlayer || twoPlayer)){
			gameSelect = 2;
		}
		//Select two player
		else if(menuOn && menuSelect > 0){
			twoPlayer = true;
		}
		//Select ufo stadium
		else if(menuOn && menuSelect == 0){
			menuSelect = 1;
			stadiumSelect = 2;
			if(tod.y != 2){tod.y = 0;}
		}
		return;
	case '3':
		//Select rain and go back to the main menu
		if(weatherSelect){
			weatherType = 2;
			if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}
			weatherSelect = false;			
		}
		//Select Coin Time Attack for single player mode
		else if(menuOn && menuSelect > 0 && singlePlayer){
			gameSelect = 3;
		}
		//Select underwater stadium
		else if(menuOn && menuSelect == 0){
			menuSelect = 1;
			stadiumSelect = 3;
			if(tod.y != 2){tod.y = 1;}
		}
		return;
	case '4':
		//Select snow and go back to the main menu
		if(weatherSelect){
			weatherType = 3;
			if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}
			weatherSelect = false;
			menuOn = true;			
		}
		//Select weather menu
		else if(menuOn && menuSelect == 0){
			weatherSelect = true;
		}
		else if(!menuOn){
			weatherType = (weatherType + 1)%4;
			if(weatherType == 1){tod.y = 2;}
			else{if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}}
		}
		return;
	case '5':
		//Cycle through time of day
		//if(menuOn){
			tod.x  = (tod.x + 1)%3;
		//}
		return;
	case 32:   //space bar
		return;
	}	
}

void SpecialFunc(int c, int x, int y)
{
	switch(c){
        case GLUT_KEY_UP:rotateX(1);return;
        case GLUT_KEY_DOWN:rotateX(-1);return;
        case GLUT_KEY_RIGHT:rotateY(-1);return;
        case GLUT_KEY_LEFT:rotateY(1);return;
		case GLUT_KEY_F1:	
			weatherType = (weatherType + 1)%4;
			if(weatherType == 1){tod.y = 2;}
			else{if(stadiumSelect == 3){tod.y = 1;}else{tod.y=0;}}
			return;
		case GLUT_KEY_F2:
			tod.x  = (tod.x + 1)%3;
			return;
    }
}

//Orchestrates all the objects and variables into a playable game
void GameDisplay(){
	glEnable(GL_CULL_FACE);
	glClearColor(0.1f, 0.1f, 0.35f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	projection = perspective(25.0f, window.window_aspect, 0.01f, 1200.0f);
	modelview = lookAt(vec3(0.0f, perspectiveOffset, perspectiveStepBack), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	modelview = render(modelview);
	glLoadMatrixf(value_ptr(modelview));
	glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);	
	/////////////////////////////Menu Display////////////////////////////////
	if(menuOn){
		//Main Menu
		if(menuFirst == 1){
			//init((ALbyte *)"./music/menuMusic.wav", true);
			//menuFirst = 4;
		}
		menuTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
		if(menuSelect == 0){
			if(!weatherSelect){textDisplay(0, (float)(window.size.x), (float)(window.size.y), tod.x, false);}
			else{textDisplay(1, (float)(window.size.x), (float)(window.size.y), tod.x, false);}
		}
		//Stadium Selection
		else if(menuSelect == 1){
			if(!(singlePlayer || twoPlayer)){textDisplay(2, (float)(window.size.x), (float)(window.size.y), tod.x, false);}
			else if(singlePlayer){
				if(gameSelect == 0){					
					if(stadiumSelect == 1){textDisplay(3, (float)(window.size.x), (float)(window.size.y), tod.x, false);}
					else if(stadiumSelect == 2){textDisplay(4, (float)(window.size.x), (float)(window.size.y), tod.x, false);}
					else if(stadiumSelect == 3){textDisplay(5, (float)(window.size.x), (float)(window.size.y), tod.x, false);}
				}
				else{textDisplay(6, (float)(window.size.x), (float)(window.size.y), tod.x, false);}				
			}
			else if(twoPlayer){textDisplay(7, (float)(window.size.x), (float)(window.size.y), tod.x, false);}
		}
	}
	//Countdown Display
	else if(countdownOn){
		if(menuFirst < 3){
			countdownTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;countdownTime -= menuTime;
			textDisplay(8, (float)(window.size.x), (float)(window.size.y), tod.x, false);
		}

		else if(menuFirst == 3){
			if (!ferrari.Initialize("./models/Ferrari.obj", "./models/Ferrari.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
				cout << "ferrari failed to initialize" << endl;

			if (!redbull.Initialize("./models/RedbullCar.obj", "./models/RedbullCar.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
					cout << "redbull failed to initialize" << endl;

			if(false == true){

				if (!lotus.Initialize("./models/LotusCar.obj", "./models/LotusCar.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
					cout << "lotus failed to initialize" << endl;	

				if (!renault.Initialize("./models/Renault.obj", "./models/Renault.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
					cout << "renault failed to initialize" << endl;

				if (!mcLaren.Initialize("./models/mcLaren.obj", "./models/mcLaren.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
					cout << "mcLaren failed to initialize" << endl;

				if (!forceIndia.Initialize("./models/forceIndia.obj", "./models/forceIndia.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
					cout << "forceIndia failed to initialize" << endl;

				if (!mercedes.Initialize("./models/mercedes.obj", "./models/mercedes.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
					cout << "mercedes failed to initialize" << endl;	

				if (!vMobile.Initialize("./models/vMobile.obj", "./models/vMobile.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
					cout << "virgin mobile failed to initialize" << endl;
			}			
			loadCars = false;
			countdownOn = false;
		}
		menuFirst++;
			
	}

	///////////////////////Game Display///////////////////////////
	else{
		current_timeDisplay = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
		current_timeDisplay = (window.paused ? window.time_last_pause_began : current_timeDisplay) - window.total_time_paused;
		
		//Mouse movement
		mouseRotations(stadiumSelect, viewPerspective);
		
		//Music change when caught
		if(caught){
			if(first == 2){
				//alSourceStop(origSource);
				//init((ALbyte *)"./music/tiger.wav", false);
				first = 3;
			}
			if(first == 3){
				//init((ALbyte *)"./music/deepJungle.wav", true);
				first = 4;
			}
		}

		//Make the skybox based on weather//
		modelview = endRender(modelview);

		modelview = rotate(modelview, (GLfloat) RotatedX, vec3(1,0,0));
		modelview = rotate(modelview, (GLfloat) RotatedY, vec3(0,1,0));
		mat4 skyboxScale = scale(modelview, vec3(1.0f,1.0f,1.0f));
		skyboxScale = translate(skyboxScale, vec3(0.0f, 50.0f, 0.0f));
		if(weatherType == 0){skybox.Draw(projection, skyboxScale, tod, 0);}
		else if(weatherType == 1 || weatherType == 2){skybox3.Draw(projection, modelview, tod, 0);}
		else if(weatherType == 3){skybox2.Draw(projection, modelview, tod, 0);}
		//Make precipitation
		if(weatherType > 1){
			mat4 another;
			for(int i=0; i<numParts; i++){
				another = rotate(modelview, 90.0f, vec3(1,0,0));
				another = translate(another, vec3(partX[i],partY[i],3*tan((current_timeDisplay)+partOffset[i])));
				another = rotate(another, 270.0f, vec3(1,0,0));
				another = rotate(another, 90.0f, vec3(0,1,0));
				if(weatherType == 2){rain2.Draw(projection, another, tod, 0);}
				else{snow2.Draw(projection, another, tod, 0);}
			}
		}
			
		modelview = rotate(modelview, (GLfloat) -RotatedY, vec3(0,1,0));
		modelview = rotate(modelview, (GLfloat) -RotatedX, vec3(1,0,0));

		modelview = render(modelview);
			
		//Used for displaying floats as strings
		stringstream tt (stringstream::in | stringstream::out);

		//Stop the clock, user, and score calculations when caught
		if(!caught){
			

			modelview = endRender(modelview);
				
			modelview = rotate(modelview, (GLfloat) RotatedX, vec3(1,0,0));

			mat4 scaler;
			scaler = translate(modelview, vec3(0.0f, -1.0f, 0.0f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			ferrari.Draw(projection, scaler, window.size, 0);

			modelview = rotate(modelview, (GLfloat) -RotatedX, vec3(1,0,0));
			
			modelview = render(modelview);
		}			
		else{
			if(gameSelect == 3){ //End time on 100 for time attack
				if(current_timeDisplay - menuTime - countdownTime >= 100){
					current_timeDisplay = 100 + menuTime + countdownTime;
				}
				tt << current_timeDisplay-menuTime-countdownTime;
				timeString = tt.str();
			}
		}
		//Change the stadium to match the weather
		modelview = translate(modelview, vec3(52.8,-1,52.8));
		//mat4 scaler = scale(modelview, vec3(2.2,2.2, 2.2));
		//arena.Draw(projection, modelview, tod, 0.0f);
		//cout << "start draw stadium" << endl;
		stadium.userX = transX;stadium.userZ = transZ;stadium.rotation = RotatedY;
		stadium.nextX = (float)(transX + 200*sin(-RotatedY*3.14/180));
		stadium.nextZ = (float)(transZ + 200*cos(-RotatedY*3.14/180));

		stadium.nextX2 = (float)(transX + 100*sin(-RotatedY*3.14/180) + transX - 300*cos(RotatedY*3.14/180));
		stadium.nextZ2 = (float)(transZ + 100*cos(-RotatedY*3.14/180) + transZ - 300*sin(RotatedY*3.14/180));

		stadium.nextX3 = (float)(transX + 100*sin(-RotatedY*3.14/180) + transX + 300*cos(RotatedY*3.14/180));
		stadium.nextZ3 = (float)(transZ + 100*cos(-RotatedY*3.14/180) + transZ + 300*sin(RotatedY*3.14/180));

		stadium.Draw(projection, modelview, tod, current_timeDisplay);
		//cout << "end draw stadium" << endl;
		mat4 scaler = translate(modelview, vec3(205.2f, 0.0f, -52.8f));
		scaler = translate(modelview, vec3(cpuX[0], 0.0f, cpuZ[0]));
			scaler = rotate(scaler, cpuRotate[0], vec3(0,1,0));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			redbull.Draw(projection, scaler, window.size, 0);
			//cout << "cpuX = " << cpuX[0] << endl;
			//cout << "cpuZ = " << cpuZ[0] << endl;
			//cout << "cpuRot = " << cpuRotate[0] << endl;

/*			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			renault.Draw(projection, scaler, window.size, 0);

		

		scaler = translate(modelview, vec3(195.2f, 0.0f, -52.8f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			lotus.Draw(projection, scaler, window.size, 0);

		scaler = translate(modelview, vec3(210.2f, 0.0f, -52.8f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			mercedes.Draw(projection, scaler, window.size, 0);

		scaler = translate(modelview, vec3(215.2f, 0.0f, -52.8f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			mcLaren.Draw(projection, scaler, window.size, 0);

		scaler = translate(modelview, vec3(220.2f, 0.0f, -52.8f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			forceIndia.Draw(projection, scaler, window.size, 0);

		scaler = translate(modelview, vec3(225.2f, 0.0f, -52.8f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			vMobile.Draw(projection, scaler, window.size, 0);*/

		scaler = translate(modelview, vec3(190.2f, 0.0f, -52.8f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(1.0f, 1.0f, 1.0f));
			ferrari.Draw(projection, scaler, window.size, 0);

		scaler = translate(modelview, vec3(195.2f, -0.25f, -42.8f));
			scaler = scale(scaler, vec3(0.01f, 0.01f, 0.01f));
			itemBox.Draw(projection, scaler, window.size, 0);

		scaler = translate(modelview, vec3(190.2f, 0.75f, -42.8f));
			scaler = rotate(scaler, 90.0f, vec3(1,0,0));
			scaler = rotate(scaler, 180.0f, vec3(0,1,0));
			scaler = scale(scaler, vec3(0.0025f, 0.0025f, 0.0025f));
			speedBoost.Draw(projection, scaler, window.size, 0);


		modelview = translate(modelview, vec3(-52.8,0,-52.8));

		current_timeDisplay = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
		if(startupTime){
			countdownTime = current_timeDisplay;
			startupTime = false;
		}

		tt << lap;
				
		timeString = tt.str();
		
		//Display Time, crosshairs, and Targets/Score
		if(viewPerspective == 1 && !caught){activeTextDisplay("+", -0.015f, -0.06f);}
		activeTextDisplay(("Lap: " + timeString).c_str(), 0.5f, -0.9f);
		stringstream ss (stringstream::in | stringstream::out);
		ss << (float)((int)(velocity*velocity*velocity*75));
		string scoreString = ss.str();
		activeTextDisplay(("Velocity: " + scoreString).c_str(), 0.0f, -0.9f);


		moveCpu();

		//Velocity controls
		float centerX = 43;
		float centerZ = 53.5;
		float userRadius = sqrt(((transX+centerX)*(transX+centerX) + (transZ+centerZ)*(transZ+centerZ)));
		if(userRadius <= 250 || userRadius >= 200){
			transZ = (float)(transZ + velocity*velocity*velocity*cos(-RotatedY*3.14/180));
			userRadius = sqrt(((transX+centerX)*(transX+centerX) + (transZ+centerZ)*(transZ+centerZ)));
			if(userRadius > 250	|| userRadius < 200){
				transZ = (float)(transZ - velocity*velocity*velocity*cos(-RotatedY*3.14/180));
				velocity -= velocity * 0.05 / abs(velocity);
			}
			transX = (float)(transX + velocity*velocity*velocity*sin(-RotatedY*3.14/180));
			userRadius = sqrt(((transX+centerX)*(transX+centerX) + (transZ+centerZ)*(transZ+centerZ)));
			if(userRadius > 250	|| userRadius < 200){
				transX = (float)(transX - velocity*velocity*velocity*sin(-RotatedY*3.14/180));
				velocity -= velocity * 0.05 / abs(velocity);
			}
		}
		userRadius = sqrt(((transX+centerX)*(transX+centerX) + (transZ+centerZ)*(transZ+centerZ)));
		//cout << "trans X = " << transX << endl;
		//cout << "trans Z = " << transZ << endl;
		/*cout << "radius = " << userRadius << endl;*/
		
		
		
		/*if(sqrt(((transX+44)*(transX+44) + (transZ+52)*(transZ+52))) > 250){
			if(abs(transZ) > abs(transX)){
				transZ = (float)(transZ - velocity*velocity*velocity*cos(-RotatedY*3.14/180));
				if(sqrt(((transX+44)*(transX+44) + (transZ+52)*(transZ+52))) > 250){
					transX = (float)(transX - velocity*velocity*velocity*sin(-RotatedY*3.14/180));
				}
			}
			else if(abs(transZ) < abs(transX)){
				transX = (float)(transX - velocity*velocity*velocity*sin(-RotatedY*3.14/180));
				if(sqrt(((transX+44)*(transX+44) + (transZ+52)*(transZ+52))) > 250){
					transZ = (float)(transZ - velocity*velocity*velocity*cos(-RotatedY*3.14/180));
				}
			}
			else{
				transZ = (float)(transZ - velocity*velocity*velocity*cos(-RotatedY*3.14/180));
				transX = (float)(transX - velocity*velocity*velocity*sin(-RotatedY*3.14/180));
			}
		}*/

		if(velocity > 0){
			velocity -= 0.01 * (velocity/1.6);		
			if(velocity < 0){velocity = 0;}
		}
		else if(velocity < 0){
			velocity += 0.006 * (-velocity/1.1);		
			if(velocity > 0){velocity = 0;}
		}



		if(userRadius >= 200){
			//Position along track / Lap Detection
			float posAngle = acos(((transX + centerX)* (centerX) + ((transZ + centerZ) * (1+centerZ)))/(userRadius*sqrt(centerX*centerX + (1+centerZ)*(1+centerZ))));
			//cout << "angle along track = " << posAngle << endl;


			if(transX < 0 && transZ > 0 && posAngle < 1.1 && posAngle > 0.9){pt1 = 1;}
			else if(pt1 == 1 && transX > 0 && transZ < 0 && posAngle < 0.9 && posAngle > 0.7){pt2 = 1;}
			else if(pt1 == 1 && pt2 == 1 && transX < 0 && transZ < 0 && posAngle < 2.6 && posAngle > 2.4){pt3 = 1;}
			else if(pt1 == 1 && pt2 == 1 && pt3 == 1 && transX < 0 && transZ < 0 && posAngle < 2.2 && posAngle > 2.03){lap++;pt1 = 0;pt2 = 0;pt3 = 0;}		

		}
		//cout << "velocity = " << velocity << endl;
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFlush();
	
}

//Put objects into the fbo
void RenderIntoFrameBuffer(){fbo.Bind();GameDisplay();fbo.Unbind();}

//Give fbo texture id to an object
void UseFramebufferToDrawSomething(){	
	glBindTexture(GL_TEXTURE_2D, fbo.texture_handles[0]);
	glEnable(GL_TEXTURE_2D);
	//stadium.jb->fboID = fbo.texture_handles[0];
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

//Manage pausing and when to display the game / jumbotrons
void DisplayFunc(){
	glViewport(0, 0, window.size.x, window.size.y);
	if(!window.paused){
	GameDisplay();	
	if(stadiumSelect == 1){
		glViewport(0, 0, (GLsizei)(512.0), (GLsizei)(512.0));
		RenderIntoFrameBuffer();
		UseFramebufferToDrawSomething();
	}	
	glutSwapBuffers();
	}
	
}

void TimerFunc(int value){if (window.window_handle != -1){glutTimerFunc(window.interval, TimerFunc, value);glutPostRedisplay();}}

int main(int argc, char * argv[])
{
	glewInit();
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 512);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	//Initialize object's colors / values
	arena.texID = 18;
	egg.texID = 20;
	lotus.texID = 22;
	redbull.texID = 25;
	ferrari.texID = 26;
	renault.texID = 27;
	mercedes.texID = 28;
	mcLaren.texID = 29;
	forceIndia.texID = 30;
	vMobile.texID = 31;
	itemBox.texID = 32;
	speedBoost.texID = 33;
	rain2.color = vec3(0.4f, 0.4f, 1.0f);
	snow2.color = vec3(1.0f, 1.0f, 1.0f);
	fontfilename = "Pokemon.ttf";

	cpuX[0] = 200.2f;cpuZ[0] = -52.8f;cpuRotate[0] = 0.0f; //Redbull

	window.window_handle = glutCreateWindow("Racing Game");
	glutReshapeFunc(ReshapeFunc);
	glutCloseFunc(CloseFunc);
	
	glutMotionFunc(mouseMovement);
	glutPassiveMotionFunc(mouseMovement); //check for mouse movement
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutTimerFunc(window.interval, TimerFunc, 0);
	
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	cout << "1" << endl;
	
	//Initialize everything

	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}

	cout << "2" << endl;

	init_resources();
	glutDisplayFunc(DisplayFunc);

	cout << "3" << endl;

	if (!fbo.Initialize(glm::ivec2(512, 512), 1, true)){
		cerr << "Frame buffer failed to initialize." << endl;
		return 0;
	}

	cout << "4" << endl;

	if (!coin.Initialize()){return 0;}
	cout << "6" << endl;
	if (!stadium.Initialize()){return 0;}
	cout << "7" << endl;
	if (!skybox.Initialize(0, 5000, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}
	if (!skybox2.Initialize(1, 5025, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}
	if (!skybox3.Initialize(2, 5050, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}
	if (!skyboxUW.Initialize(3, 5075, "basic_skybox_shader.vert", "basic_skybox_shader.frag")){return 0;}

	if (!itemBox.Initialize("./models/itemBlock.obj", "./models/itemBlock.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
		cout << "item box failed to init" << endl;
	speedBoost.order = 1;
	if (!speedBoost.Initialize("./models/LaunchStar.obj", "./models/LaunchStar2.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag"))
		cout << "speed boost failed to init" << endl;

	cout << "8" << endl;
	arena.order = 1;mcLaren.order = 1;vMobile.order = 1;forceIndia.order = 1;
	renault.order = 1;redbull.order = 1;ferrari.order = 1;lotus.order = 1;mercedes.order = 1;
	
	if (!rain2.Initialize(3, 0.03f, "phong.vert", "phong.frag")){return 0;}
	cout << "14" << endl;
	if (!snow2.Initialize(3, 0.02f, "phong.vert", "phong.frag")){return 0;}
	cout << "15" << endl;
	if (!egg.Initialize(15, 1, "./textures/alienEgg.jpg", "basic_texture_shader.vert", "basic_texture_shader.frag")){return 0;}
	cout << "16" << endl;
	if (!sq4.Initialize(1, 5.0f, "basic_texture_shader.vert", "basic_texture_shader.frag")) {return 0;}
	cout << "17" << endl;

	glutMainLoop();
}