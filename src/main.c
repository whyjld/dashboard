/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include <signal.h>

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include "EGL/egl.h"
#include "fslegl.h"



#define TRUE 1
#define FALSE !TRUE

EGLDisplay			egldisplay;
EGLConfig			eglconfig;
EGLSurface			eglsurface;
EGLContext			eglcontext;
EGLNativeWindowType eglNativeWindow;
EGLNativeDisplayType eglNativeDisplayType;
volatile sig_atomic_t quit = 0;

GLuint       g_hShaderProgram      = 0;
GLuint       g_hModelViewMatrixLoc = 0;
GLuint       g_hProjMatrixLoc      = 0;
GLuint       g_hVertexLoc          = 0;
GLuint       g_hColorLoc           = 1;


//--------------------------------------------------------------------------------------
// Name: g_strVertexShader / g_strFragmentShader
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
const char* g_strVertexShader = 
"uniform   mat4 g_matModelView;				\n"
"uniform   mat4 g_matProj;					\n"
"								\n"
"attribute vec4 g_vPosition;				\n"
"attribute vec3 g_vColor;					\n"
"								\n"
"varying   vec3 g_vVSColor;					\n"
"								\n"
"void main()						\n"
"{								\n"
"    vec4 vPositionES = g_matModelView * g_vPosition;	\n"
"    gl_Position  = g_matProj * vPositionES;		\n"
"    g_vVSColor = g_vColor;					\n"
"}								\n";


const char* g_strFragmentShader = 
"#ifdef GL_FRAGMENT_PRECISION_HIGH				\n"
"   precision highp float;					\n"
"#else							\n"
"   precision mediump float;				\n"
"#endif							\n"
"								\n"
"varying   vec3      g_vVSColor;				\n"
"								\n"
"void main()						\n"
"{								\n"
"    gl_FragColor = vec4( g_vVSColor, 1.0 );		\n"
"}								\n";


float matModelView[16] = {0};
float matProj[16] = {0};

// Define vertice for a 4-sided pyramid
float VertexPositions[] =
{
	0.0f,1.0f, 0.0f,     
	-1.0f,-1.0f, 0.0f,     
	+1.0f,-1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	-1.0f,1.0f, 0.0f,
	1.0f, -1.0, 0.0f		
};

void render(float w, float h)
{

	// Clear the colorbuffer and depth-buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Build a perspective projection matrix
	matProj[ 0] = cosf( 0.5f ) / sinf( 0.5f );
	matProj[ 5] = matProj[0] * (w/h) ;
	matProj[10] = -( 10.0f ) / ( 9.0f );
	matProj[11] = -1.0f;
	matProj[14] = -( 10.0f ) / ( 9.0f );

	glViewport(0, 0, w, h);

	// Rotate and translate the model view matrix
	matModelView[ 0] = 1.0;
	matModelView[ 2] = 0.0f;
	matModelView[ 5] = 1.0f;
	matModelView[ 8] = 0.0f;
	matModelView[10] = 0.0f;
	matModelView[14] = -4.0f;//translation Z
	matModelView[15] = 1.0f;

	glUniformMatrix4fv( g_hModelViewMatrixLoc, 1, 0, matModelView );
	glUniformMatrix4fv( g_hProjMatrixLoc,      1, 0, matProj );

	// Bind the vertex attributes
	glVertexAttribPointer( g_hVertexLoc, 3, GL_FLOAT, 0, 0, VertexPositions );
	glEnableVertexAttribArray( g_hVertexLoc );

	glDrawArrays( GL_TRIANGLES, 0, 3 );

	// Cleanup
	glDisableVertexAttribArray( g_hVertexLoc );
}

int init(void)
{

	static const EGLint s_configAttribs[] =
	{
		EGL_RED_SIZE,		5,
		EGL_GREEN_SIZE, 	6,
		EGL_BLUE_SIZE,		5,
		EGL_ALPHA_SIZE, 	0,
		EGL_SAMPLES,		0,
		EGL_NONE
	};

	EGLint numconfigs;

	eglNativeDisplayType = fsl_getNativeDisplay();
	egldisplay = eglGetDisplay(eglNativeDisplayType);
	eglInitialize(egldisplay, NULL, NULL);
	assert(eglGetError() == EGL_SUCCESS);
	eglBindAPI(EGL_OPENGL_ES_API);

	eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
	assert(eglGetError() == EGL_SUCCESS);
	assert(numconfigs == 1);

	eglNativeWindow = fsl_createwindow(egldisplay, eglNativeDisplayType);	
	assert(eglNativeWindow);	

	eglsurface = eglCreateWindowSurface(egldisplay, eglconfig, eglNativeWindow, NULL);

	assert(eglGetError() == EGL_SUCCESS);
	EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	eglcontext = eglCreateContext( egldisplay, eglconfig, EGL_NO_CONTEXT, ContextAttribList );
	assert(eglGetError() == EGL_SUCCESS);
	eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);
	assert(eglGetError() == EGL_SUCCESS);

	{
		// Compile the shaders
		GLuint hVertexShader = glCreateShader( GL_VERTEX_SHADER );
		glShaderSource( hVertexShader, 1, &g_strVertexShader, NULL );
		glCompileShader( hVertexShader );

		// Check for compile success
		GLint nCompileResult = 0;
		glGetShaderiv( hVertexShader, GL_COMPILE_STATUS, &nCompileResult );
		if( 0 == nCompileResult )
		{
			char  strLog[1024];
			GLint nLength;
			glGetShaderInfoLog( hVertexShader, 1024, &nLength, strLog );
			return FALSE;
		}

		GLuint hFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( hFragmentShader, 1, &g_strFragmentShader, NULL );
		glCompileShader( hFragmentShader );

		// Check for compile success
		glGetShaderiv( hFragmentShader, GL_COMPILE_STATUS, &nCompileResult );
		if( 0 == nCompileResult )
		{
			char  strLog[1024];
			GLint nLength;
			glGetShaderInfoLog( hFragmentShader, 1024, &nLength, strLog );
			return FALSE;
		}

		// Attach the individual shaders to the common shader program
		g_hShaderProgram = glCreateProgram();
		glAttachShader( g_hShaderProgram, hVertexShader );
		glAttachShader( g_hShaderProgram, hFragmentShader );

		// Init attributes BEFORE linking
		glBindAttribLocation( g_hShaderProgram, g_hVertexLoc,   "g_vPosition" );

		// Link the vertex shader and fragment shader together
		glLinkProgram( g_hShaderProgram );

		// Check for link success
		GLint nLinkResult = 0;
		glGetProgramiv( g_hShaderProgram, GL_LINK_STATUS, &nLinkResult );
		if( 0 == nLinkResult )
		{
			char strLog[1024];
			GLint nLength;
			glGetProgramInfoLog( g_hShaderProgram, 1024, &nLength, strLog );
			return FALSE;
		}

		// Get uniform locations
		g_hModelViewMatrixLoc = glGetUniformLocation( g_hShaderProgram, "g_matModelView" );
		g_hProjMatrixLoc      = glGetUniformLocation( g_hShaderProgram, "g_matProj" );

		glDeleteShader( hVertexShader );
		glDeleteShader( hFragmentShader );

		// Set the shader program
		glUseProgram( g_hShaderProgram );

		// Clear the colorbuffer and depth-buffer
		glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );

		// Set some state
		glEnable( GL_DEPTH_TEST );

	}

	return 1;
}

void Cleanup()
{

}

void resize(int w, int h)
{
	// Build a perspective projection matrix
	matProj[ 0] = cosf( 0.5f ) / sinf( 0.5f );
	matProj[ 5] = matProj[0] * (w/h) ;
	matProj[10] = -( 10.0f ) / ( 9.0f );
	matProj[11] = -1.0f;
	matProj[14] = -( 10.0f ) / ( 9.0f );

	glViewport(0, 0, w, h);
}

void deinit(void)
{
	printf("Cleaning up...\n");
	eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	assert(eglGetError() == EGL_SUCCESS);
	eglDestroyContext(egldisplay, eglcontext);
	eglDestroySurface(egldisplay, eglsurface);	
	fsl_destroywindow(eglNativeWindow, eglNativeDisplayType);
	eglTerminate(egldisplay);
	assert(eglGetError() == EGL_SUCCESS);
	eglReleaseThread();	
}

void sighandler(int signal)
{
	printf("Caught signal %d, setting flaq to quit.\n", signal);
	quit = 1;
}

int main (void)
{
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);

	assert( init() );

	while (!quit)
	{
		EGLint width = 0;
		EGLint height = 0;
		eglQuerySurface(egldisplay, eglsurface, EGL_WIDTH, &width);
		eglQuerySurface(egldisplay, eglsurface, EGL_HEIGHT, &height);
		render(width,height);
		eglSwapBuffers(egldisplay, eglsurface);
	}
	deinit();
	return 0;
}

