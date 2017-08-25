#ifndef _TGA_h_
#define _TGA_h_

#include <GLES3/gl3.h>
#include <fstream>

GLuint LoadTGA(const char* name, GLsizei& width, GLsizei& height, GLenum& format);

#endif//_TGA_h_