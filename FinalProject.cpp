#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// *********************************************************
// Final Project
// CS 457: Computer Graphics Shaders
// 
// Author: Shauna Bybee
// 
// Note: Some of the code for the graphics program was
// provided by the instructor. My code is primarily found
//  in the following methods:
//   - Display()
//   - InitVAOs()
//	 - InitFBOs()
// *********************************************************

// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLSL Sample -- Joe Graphics" };
const char *GLUITITLE   = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

const int ESCAPE = 0x1b;

// timing

const int MS_PER_CYCLE = 30000;

// initial window size:

const int INIT_WINDOW_SIZE = { 800 };

// size of the box:

const float BOXSIZE = { 2.f };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };

// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };

// for glm - convert degrees to radians

const float D2R = M_PI / 180.f;

// non-constant global variables:

int			ActiveButton;			// current button that is down
GLuint		AxesList;				// list to hold the axes
int			AxesOn;					// != 0 means to draw the axes
GLuint		blueLinesFBO;
GLuint		blueLinesTexture;
GLSLProgram* BlurShader;
GLSLProgram* BoxShader;	
GLuint		boxVAO;					
GLuint		boxVBO;					
bool		buffer1;				// used in the final display rendering to render from various buffers
bool		buffer2;
bool		buffer3;
bool		ChangeFragments;
bool		ChangeVertices;
float		Color[3];
GLSLProgram* CombineShader;
int			DebugOn;				// != 0 means to print debugging info
int			DepthCueOn;				// != 0 means to use intensity depth cueing
float		Ds, Dt;
bool		finalBlur;				// determines whether a final blur pass will be applied
GLuint		fbo;					// Framebuffer object
bool		Freeze;
GLuint		glowingFBO;
GLSLProgram* GlowingLine;
GLuint		glowingRBO;
GLuint		glowingTexture;
GLuint		lineVAO;
GLuint		lineVBO;
glm::mat4	model;					// model matrix
GLuint		NoiseTexture;			// 3D Noise texture
GLuint		pingpongFBO[2];			// Ping pong framebuffer for Gaussian blur effect
GLuint		pingpongBuffer[2];		// Ping pong color buffer for Gaussian blur effect
glm::mat4	projection;				// projection matrix
GLuint		projectionLoc;
GLuint		quadVAO;
GLuint		quadVBO;
GLuint		rbo;					// Renderbuffer object 
GLuint		SphereList;				// object display list
GLuint		solidFBO;
GLSLProgram* SolidLine;
GLuint		solidRBO;
GLuint		solidTexture;
int			MainWindow;				// window id for main graphics window
GLuint		modelLoc;
float		S0, T0;
float		Scale;					// scaling factor
GLSLProgram* ScreenShader;
GLuint		textureColorbuffer;		// For rendering from the framebuffer
float		Time;
bool		valid;					// used to test whether a shader program is valid after creation
glm::mat4	view;					// view matrix
GLuint		viewLoc;
int			WhichColor;				// index into Colors[ ]
int			WhichProjection;		// ORTHO or PERSP
int			Xmouse, Ymouse;			// mouse values
float		Xrot, Yrot;				// rotation angles in degrees

float	White[ ] = { 1.,1.,1.,1. };


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoChangeMenu( int );
void	DoColorMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
void	DrawSolidLines( );
float	ElapsedSeconds( );
void	InitFBOs( );
void	InitGraphics( );
void	InitLists( );
void	InitVAOs( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	OsuSphere( float, int, int );
unsigned char* ReadTexture3D(char*, int*, int*, int*);
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );

float *	Array3( float, float, float );
float *	Array4( float, float, float, float );
float *	BlendArray3( float, float [3], float [3] );
float *	MulArray3( float, float [3] );

// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display structures that will not change:

	InitLists( );
	InitVAOs( );
	InitFBOs( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	int ms = glutGet( GLUT_ELAPSED_TIME );
	ms %= MS_PER_CYCLE;
	/*if (ms > MS_PER_CYCLE / 2.)
	{
		ms = MS_PER_CYCLE - ms;
	}*/
	Time = (float)ms / (float)( MS_PER_CYCLE - 1 );

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	// FIRST PASS - send to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, glowingFBO);
	glClearColor(0.f, 0.12f, 0.24f, 1.0f);
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );


	// Set up transparency/blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:

	if (WhichProjection == ORTHO)
		projection = glm::ortho(-3., 3., -3., 3., 0.1, 1000.);
	else
		projection = glm::perspective(D2R * 90., 1., 0.1, 1000.);


	// set the eye position, look-at position, and up-vector:

	glm::vec3 eye(0., 0., 3.);
	glm::vec3 look(0., 0., 0.);
	glm::vec3 up(0., 1., 0.);
	glm::mat4 view = glm::lookAt(eye, look, up);

	glm::mat4 model(1.);


	// rotate the scene:

	model = glm::rotate(model, D2R * Yrot, glm::vec3(0., 1., 0.));
	model = glm::rotate(model, D2R * Xrot, glm::vec3(1., 0., 0.));

	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	//glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	model = glm::scale(model, glm::vec3(Scale, Scale, Scale));


	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// *************************************************************
	// ******** STEP 1: Render the very blurry background ********** 
	// *************************************************************

	model = glm::rotate(model, glm::radians(180.f), glm::vec3(1., 0., 0.));
	model = glm::translate(model, glm::vec3(0., 0., 100.));

	// Set up the noise texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D, NoiseTexture);

	// Draw the solid lines
	SolidLine->Use();
	SolidLine->SetUniformVariable("radius", 0.5f);
	SolidLine->SetUniformVariable("uWaviness", 5.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 1.f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);

	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->Use();
	SolidLine->SetUniformVariable("radius", 0.05f);
	SolidLine->SetUniformVariable("uWaviness", 5.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 1.f, 1.f, 2.f);
	SolidLine->SetUniformVariable("clockwise", false);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);

	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.2f);
	SolidLine->SetUniformVariable("uWaviness", 4.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 2.1f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->SetUniformVariable("radius", 0.1f);
	SolidLine->SetUniformVariable("uWaviness", 6.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 1.f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", true);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->SetUniformVariable("radius", 0.1f);
	SolidLine->SetUniformVariable("uWaviness", 7.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 0.3f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->Use();
	SolidLine->SetUniformVariable("radius", 0.15f);
	SolidLine->SetUniformVariable("uWaviness", 3.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 2.f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);

	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->Use();
	SolidLine->SetUniformVariable("radius", 0.05f);
	SolidLine->SetUniformVariable("uWaviness", 4.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 5.f);
	SolidLine->SetUniformVariable("clockwise", false);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);

	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.2f);
	SolidLine->SetUniformVariable("uWaviness", 6.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 2.5f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->SetUniformVariable("radius", 0.15f);
	SolidLine->SetUniformVariable("uWaviness", 8.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 0.15f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->SetUniformVariable("radius", 0.1f);
	SolidLine->SetUniformVariable("uWaviness", 7.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 3.5f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", true);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	// Draw the glowing halo 
	GlowingLine->Use();
	GlowingLine->SetUniformVariable("radius", 4.f);
	GlowingLine->SetUniformVariable("zOffset", 0.f);
	GlowingLine->SetUniformVariable("uColor", 0.f, 0.38f, 0.75f);
	GlowingLine->SetUniformVariable("uTime", Time);
	GlowingLine->SetUniformVariable("noise3", 3);
	projectionLoc = GlowingLine->GetUniformLocation("projection");
	viewLoc = GlowingLine->GetUniformLocation("view");
	modelLoc = GlowingLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	GlowingLine->SetUniformVariable("radius", 1.f);
	GlowingLine->SetUniformVariable("zOffset", -0.05f);
	GlowingLine->SetUniformVariable("uColor", 1., 0.5, 0.5);
	glDrawArrays(GL_LINES, 0, 1000);
	GlowingLine->Use(0);
	

	// PING PONG BLUR SHADER TECHNIQUE
	// From: https://learnopengl.com/Advanced-Lighting/Bloom
	glActiveTexture(GL_TEXTURE0);

	bool horizontal = true, first_iteration = true;
	int amount = 50;
	BlurShader->Use();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		BlurShader->SetUniformVariable("horizontal", horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? glowingTexture : pingpongBuffer[!horizontal]);
		BlurShader->SetUniformVariable("screenTexture", 0);
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		horizontal = !horizontal;
		if (first_iteration)
		{
			first_iteration = false;
		}
	}
	BlurShader->Use(0);

	// Transfer it back to the glowingTexture for storage
	BlurShader->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, glowingFBO);
	BlurShader->SetUniformVariable("horizontal", true);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
	BlurShader->SetUniformVariable("screenTexture", 0);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 6);



	// *****************************************************************
	// ******** STEP 2: Render the middle layer with medium blur *******
	// *****************************************************************

	glBindFramebuffer(GL_FRAMEBUFFER, blueLinesFBO);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	// Set up transparency/blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Larger lines closer to the middle, smaller lines further out
	SolidLine->Use();
	SolidLine->SetUniformVariable("radius", 0.15f);
	SolidLine->SetUniformVariable("uWaviness", 2.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.f, 0.f, 1.2f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);

	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.1f);
	SolidLine->SetUniformVariable("uWaviness", 3.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -0.2f, -0.2f, 2.7f);
	SolidLine->SetUniformVariable("clockwise", false);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.1f);
	SolidLine->SetUniformVariable("uWaviness", 3.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.4f, 0.4f, 2.8f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", -0.39f, 0.92f, 1.f);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.07f);
	SolidLine->SetUniformVariable("uWaviness", 5.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -0.6f, 0.6f, 2.4f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.07f);
	SolidLine->SetUniformVariable("uWaviness", 6.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -0.7f, 0.7f, 3.f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.03f);
	SolidLine->SetUniformVariable("uWaviness", 5.2f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 1.1f, -1.1f, 3.f);
	SolidLine->SetUniformVariable("clockwise", false);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.03f);
	SolidLine->SetUniformVariable("uWaviness", 7.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -1.1f, 1.1f, 3.f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.02f);
	SolidLine->SetUniformVariable("uWaviness", 5.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 1.2f, -1.2f, 0.15f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.02f);
	SolidLine->SetUniformVariable("uWaviness", 7.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -1.0f, -1.0f, 3.5f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", true);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.02f);
	SolidLine->SetUniformVariable("uWaviness", 7.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -0.9f, -0.3f, 3.5f);
	SolidLine->SetUniformVariable("uColor", 0.39f, 0.92f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	// Blur it a bit
	horizontal = true;
	first_iteration = true;
	amount = 3;
	BlurShader->Use();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		BlurShader->SetUniformVariable("horizontal", horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? blueLinesTexture : pingpongBuffer[!horizontal]);
		BlurShader->SetUniformVariable("screenTexture", 0);
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		horizontal = !horizontal;
		if (first_iteration)
		{
			first_iteration = false;
		}
	}
	BlurShader->Use(0);

	// Store the result
	BlurShader->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, blueLinesFBO);
	BlurShader->SetUniformVariable("horizontal", true);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
	BlurShader->SetUniformVariable("screenTexture", 0);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	// *********************************************************
	// ******** STEP 3: Render the solid white lines ********** 
	// *********************************************************

	glBindFramebuffer(GL_FRAMEBUFFER, solidFBO);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	// Set up transparency/blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw the solid lines
	SolidLine->Use();
	SolidLine->SetUniformVariable("radius", 0.2f);
	SolidLine->SetUniformVariable("uWaviness", 5.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.1f, 0.1f, 1.f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", 1.f, 1.f, 1.f);

	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->SetUniformVariable("radius", 0.1f);
	SolidLine->SetUniformVariable("uWaviness", 4.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.2f, -0.2f, 1.8f);
	SolidLine->SetUniformVariable("uColor", 1.f, 1.f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->SetUniformVariable("radius", 0.1f);
	SolidLine->SetUniformVariable("uWaviness", 6.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -0.3f, 0.3f, 4.9f);
	SolidLine->SetUniformVariable("uColor", 1.f, 1.f, 1.f);
	SolidLine->SetUniformVariable("clockwise", true);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->SetUniformVariable("radius", 0.075f);
	SolidLine->SetUniformVariable("uWaviness", 4.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -0.4f, -0.4f, 0.5f);
	SolidLine->SetUniformVariable("uColor", 1.f, 1.f, 1.f);
	SolidLine->SetUniformVariable("clockwise", true);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);


	SolidLine->SetUniformVariable("radius", 0.05f);
	SolidLine->SetUniformVariable("uWaviness", 7.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", 0.2f, 0.2f, 0.3f);
	SolidLine->SetUniformVariable("uColor", 1.f, 1.f, 1.f);
	SolidLine->SetUniformVariable("clockwise", false);
	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);

	SolidLine->Use();
	SolidLine->SetUniformVariable("radius", 0.05f);
	SolidLine->SetUniformVariable("uWaviness", 3.f);
	SolidLine->SetUniformVariable("uTime", Time);
	SolidLine->SetUniformVariable("noise3", 3);
	SolidLine->SetUniformVariable("uOffset", -0.2f, 0.f, 2.6f);
	SolidLine->SetUniformVariable("clockwise", true);
	SolidLine->SetUniformVariable("uColor", 1.f, 1.f, 1.f);

	projectionLoc = SolidLine->GetUniformLocation("projection");
	viewLoc = SolidLine->GetUniformLocation("view");
	modelLoc = SolidLine->GetUniformLocation("model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 1000);



	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	CombineShader->Use();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, solidTexture);
	CombineShader->SetUniformVariable("sharpTex", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blueLinesTexture);
	CombineShader->SetUniformVariable("blueLinesTex", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, glowingTexture);
	CombineShader->SetUniformVariable("blurredTex", 2);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	CombineShader->Use(0);

	// Blur it a tiny bit
	horizontal = true;
	first_iteration = true;
	amount = 2;
	BlurShader->Use();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		BlurShader->SetUniformVariable("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? textureColorbuffer : pingpongBuffer[!horizontal]);
		BlurShader->SetUniformVariable("screenTexture", 0);
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		horizontal = !horizontal;
		if (first_iteration)
		{
			first_iteration = false;
		}
	}
	BlurShader->Use(0);


	// FINAL PASS - Render from the final color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.f, 0.12f, 0.24f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ScreenShader->Use();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	if (buffer1)
		glBindTexture(GL_TEXTURE_2D, glowingTexture);
	else if (buffer2)
		glBindTexture(GL_TEXTURE_2D, blueLinesTexture);
	else if (buffer3)
		glBindTexture(GL_TEXTURE_2D, solidTexture);
	else if (finalBlur)
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
	else
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

	ScreenShader->SetUniformVariable("screenTexture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	ScreenShader->Use(0);



	glutSwapBuffers( );
	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoChangeMenu( int id )
{
	ChangeVertices  = (id & 1 ) != 0;
	ChangeFragments = (id & 2 ) != 0;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


void
DrawSolidLines()
{


	glBindVertexArray(boxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	BoxShader->Use(0);


	//SolidLine->Use();
	//SolidLine->SetUniformVariable("radius", 0.5f);
	//SolidLine->SetUniformVariable("uWaviness", 8.f);
	//SolidLine->SetUniformVariable("uTime", Time);
	//SolidLine->SetUniformVariable("noise3", 3);
	//SolidLine->SetUniformVariable("uColor", 1.0, 0., 0.);
	//projectionLoc = SolidLine->GetUniformLocation("projection");
	//viewLoc = SolidLine->GetUniformLocation("view");
	//modelLoc = SolidLine->GetUniformLocation("model");
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//glBindVertexArray(lineVAO);
	//glDrawArrays(GL_LINES, 0, 200);
	//SolidLine->Use(0);
}

// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int changemenu = glutCreateMenu( DoChangeMenu );
	glutAddMenuEntry( "No Shader Changes",  0 );
	glutAddMenuEntry( "Vertex Shader Changes Only",   1 );
	glutAddMenuEntry( "Fragment Shader Changes Only",   2 );
	glutAddMenuEntry( "Vertex + Fragment Shader Changes",   1|2 );

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Shaders",       changemenu);
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif


	// Set up noise texture
	glGenTextures(1, &NoiseTexture);
	int nums, numt, nump;
	unsigned char* texture = ReadTexture3D("noise3d.064.tex", &nums, &numt, &nump);
	if (texture == NULL)
	{
		printf("Error creating noise texture.");
	}
	glBindTexture(GL_TEXTURE_3D, NoiseTexture);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, nums, numt, nump, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);


	// Create the shader for the box
	BoxShader = new GLSLProgram();
	valid = BoxShader->Create("box.vert", "box.frag");
	if (!valid)
		fprintf(stderr, "Box shader cannot be created!\n");
	else
		fprintf(stderr, "Box shader created.\n");
	BoxShader->SetVerbose(false);


	// Create the shader for the glowing line
	GlowingLine = new GLSLProgram();
	valid = GlowingLine->Create("glowingline.vert", "glowingline.geom", "glowingline.frag");
	if (!valid)
		fprintf(stderr, "GlowingLine shader cannot be created!\n");
	else
		fprintf(stderr, "GlowingLine shader created.\n");
	GlowingLine->SetVerbose(false);


	// Create the shader for the solid line
	SolidLine = new GLSLProgram();
	valid = SolidLine->Create("solidline.vert", "solidline.geom", "solidline.frag");
	if (!valid)
		fprintf(stderr, "GlowingLine shader cannot be created!\n");
	else
		fprintf(stderr, "GlowingLine shader created.\n");
	SolidLine->SetVerbose(false);


	// Shader for the quad that will render from the framebuffer
	ScreenShader = new GLSLProgram();
	valid = ScreenShader->Create("screenshader.vert", "screenshader.frag");
	if (!valid)
		fprintf(stderr, "Screen shader cannot be created!\n");
	else
		fprintf(stderr, "Screen shader created.\n");
	ScreenShader->SetVerbose(false);


	// Gaussian blur shader
	BlurShader = new GLSLProgram();
	valid = BlurShader->Create("blur.vert", "blur.frag");
	if (!valid)
		fprintf(stderr, "Blur shader cannot be created!\n");
	else
		fprintf(stderr, "Blur shader created.\n");
	BlurShader->SetVerbose(false);


	// Shader for combining layers
	CombineShader = new GLSLProgram();
	valid = CombineShader->Create("combine.vert", "combine.frag");
	if (!valid)
		fprintf(stderr, "Combine shader cannot be created!\n");
	else
		fprintf(stderr, "Combine shader created.\n");
	CombineShader->SetVerbose(false);

}



// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	glutSetWindow( MainWindow );

	// create the object:

	SphereList = glGenLists( 1 );
	float rgb[3] = { 1., 1., 1. };
	glNewList( SphereList, GL_COMPILE );
		glMaterialfv( GL_BACK,  GL_AMBIENT,    MulArray3( .4f, White ) );
		glMaterialfv( GL_BACK,  GL_DIFFUSE,    MulArray3( 1.f, White ) );
		glMaterialfv( GL_BACK,  GL_SPECULAR,   Array3( 0., 0., 0. ) );
		glMaterialf (  GL_BACK,  GL_SHININESS, 3. );
		glMaterialfv( GL_BACK,  GL_EMISSION,   Array3( 0., 0., 0. ) );

		glMaterialfv( GL_FRONT, GL_AMBIENT,    MulArray3( 1.f, rgb ) );
		glMaterialfv( GL_FRONT, GL_DIFFUSE,    MulArray3( 1.f, rgb ) );
		glMaterialfv( GL_FRONT, GL_SPECULAR,   MulArray3( .7f, White ) );
		glMaterialf ( GL_FRONT, GL_SHININESS,  8. );
		glMaterialfv( GL_FRONT, GL_EMISSION,   Array3( 0., 0., 0. ) );

		OsuSphere( 1.3f, 72, 72 );
	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


void
InitFBOs()
{
	// Framebuffer object for rendering the quad to the screen
	// From: https://learnopengl.com/Advanced-OpenGL/Framebuffers
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// Renderbuffer object for the depth buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: Framebuffer is not complete.");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Framebuffer for capturing the solid lines
	glGenFramebuffers(1, &solidFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, solidFBO);
	glGenTextures(1, &solidTexture);
	glBindTexture(GL_TEXTURE_2D, solidTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, solidTexture, 0);

	glGenRenderbuffers(1, &solidRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, solidRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, solidRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: Framebuffer is not complete.");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Framebuffer for capturing the glowing lines
	glGenFramebuffers(1, &glowingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, glowingFBO);
	glGenTextures(1, &glowingTexture);
	glBindTexture(GL_TEXTURE_2D, glowingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glowingTexture, 0);

	glGenRenderbuffers(1, &glowingRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, glowingRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glowingRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: Framebuffer is not complete.");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Ping pong framebuffer for blurring
	// From: https://learnopengl.com/Advanced-Lighting/Bloom
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F,
			glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
			0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
	}


	// Destination buffer for the blue line layer
	glGenFramebuffers(1, &blueLinesFBO);
	glGenTextures(1, &blueLinesTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, blueLinesFBO);
	glBindTexture(GL_TEXTURE_2D, blueLinesTexture);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA16F,
		glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
		0, GL_RGBA, GL_FLOAT, NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blueLinesTexture, 0);


}


void
InitVAOs()
{
	float boxVertices[] = {
		// From: https://learnopengl.com/Advanced-OpenGL/Framebuffers 
		// positions          // texture Coords		// normals
		// back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.f, 0.f, -1.f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.f, 0.f, -1.f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.f, 0.f, -1.f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.f, 0.f, -1.f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.f, 0.f, -1.f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.f, 0.f, -1.f,

		// front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.f, 0.f, 1.f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.f, 0.f, 1.f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.f, 0.f, 1.f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.f, 0.f, 1.f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.f, 0.f, 1.f,

		// left side
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.f, 0.f, 0.f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.f, 0.f, 0.f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.f, 0.f, 0.f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.f, 0.f, 0.f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.f, 0.f, 0.f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.f, 0.f, 0.f,

		// right side
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.f, 0.f, 0.f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.f, 0.f, 0.f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.f, 0.f, 0.f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.f, 0.f, 0.f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.f, 0.f, 0.f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.f, 0.f, 0.f,

		 // bottom
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.f, -1.f, 0.f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.f, -1.f, 0.f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.f, -1.f, 0.f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.f, -1.f, 0.f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.f, -1.f, 0.f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.f, -1.f, 0.f,

		// top
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.f, 1.f, 0.f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.f, 1.f, 0.f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.f, 1.f, 0.f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.f, 1.f, 0.f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.f, 1.f, 0.f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.f, 1.f, 0.f
	};


	float zPosition = -0.5;
	float lineVertices[3003];
	// Vertices for a line that goes backwards in the Z direction

	lineVertices[0] = 0.;
	lineVertices[1] = 0.;
	lineVertices[2] = 0.;
	for (int i = 3; i < 3003; i += 6)
	{
		lineVertices[i] = 0.;
		lineVertices[i + 1] = 0.;
		lineVertices[i + 2] = zPosition;
		lineVertices[i + 3] = 0.;
		lineVertices[i + 4] = 0.;
		lineVertices[i + 5] = zPosition;
		zPosition -= 0.5;
	}

	float quadVertices[] = { 
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// From: https://learnopengl.com/Advanced-OpenGL/Framebuffers 
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};
	
	// Box VAO
	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);
	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));


	// Line VAO
	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(lineVAO);
	glGenBuffers(1, &lineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



	// Screen quad VAO (for rendering from the Framebuffer)
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}

// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case '0':
			buffer1 = false;
			buffer2 = false;
			buffer3 = false;
			break;
		case '1':
			buffer1 = !buffer1;
			buffer2 = false;
			buffer3 = false;
			break;
		case '2':
			buffer1 = false;
			buffer2 = !buffer2;
			buffer3 = false;
			break;
		case '3':
			buffer1 = false;
			buffer2 = false;
			buffer3 = !buffer3;
			break;
		case 'b':
		case 'B':
			finalBlur = !finalBlur;
			break;

		case 'f':
		case 'F':
			Freeze = ! Freeze;
			if( Freeze )
				glutIdleFunc( NULL );
			else
				glutIdleFunc( Animate );
			break;

		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case 3:
			Scale += SCLFACT * 3.f;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case 4:
			Scale -= SCLFACT * 3.f;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// Method for reading in the 3D noise texture (or other 3D textures)
unsigned char*
ReadTexture3D(char* filename, int* width, int* height, int* depth)
{
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
		return NULL;
	int nums, numt, nump;
	fread(&nums, 4, 1, fp);
	fread(&numt, 4, 1, fp);
	fread(&nump, 4, 1, fp);
	*width = nums;
	*height = numt;
	*depth = nump;
	unsigned char* texture = new unsigned char[4 * nums * numt * nump];
	fread(texture, 4 * nums * numt * nump, 1, fp);
	fclose(fp);
	return texture;
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 0;
	buffer1 = false;
	buffer2 = false;
	buffer3 = false;
	ChangeFragments = ChangeVertices = false;
	DebugOn = 0;
	DepthCueOn = 0;
	finalBlur = false;
	Freeze = false;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}


float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}


float *
Array4( float a, float b, float c, float d )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = d;
	return array;
}



float *
BlendArray3( float factor, float array0[3], float array1[3] )
{
	static float array[4];

	array[0] = factor * array0[0]  +  ( 1.f - factor ) * array1[0];
	array[1] = factor * array0[1]  +  ( 1.f - factor ) * array1[1];
	array[2] = factor * array0[2]  +  ( 1.f - factor ) * array1[2];
	array[3] = 1.;
	return array;
}



float *
MulArray3( float factor, float array0[3] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

#include "osusphere.cpp"
#include "glslprogram.cpp"
