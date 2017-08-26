#ifndef _GLXC_H_
#define _GLXC_H_

#include <GL/glew.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>

class OGLContext
{
public:
	OGLContext();
	OGLContext(OGLContext&) = delete;
	
	~OGLContext();
	
	void SwapBuffers() const;
	
	GLuint GetWidth() const
	{
		return m_Width;
	}
	
	GLuint GetHeight() const
	{
		return m_Height;
	}	
private:
	Window Createwindow(Display* display, GLXFBConfig& config);
	void Destroywindow(Window window, Colormap colormap, Display* display);

	static int ContextErrorHandler(Display *dpy, XErrorEvent *ev);
	static bool s_ContextErrorOccurred;

	Display*   m_Display;
	Window     m_Window;
	Colormap   m_Colormap;
	GLXContext m_Context;
	
	GLuint m_Width;
	GLuint m_Height;
};

#endif//_GLXC_H_

