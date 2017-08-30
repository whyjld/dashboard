#ifndef _TGA_h_
#define _TGA_h_

#include "gli.h"
#include <fstream>

bool LoadTGA(const char* name, GLsizei& width, GLsizei& height, GLenum& format, uint8_t*& image);

#endif//_TGA_h_