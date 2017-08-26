#ifndef _GLI_h_
#define _GLI_h_

#if defined(USE_OGL)
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glx.h>
#else
	#include "eglc.h"
#endif

#endif//_GLI_h_