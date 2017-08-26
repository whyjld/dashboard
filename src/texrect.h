#ifndef _TEX_RECT_h_
#define _TEX_RECT_h_

#include <map>
#include <string>
#include <GLES3/gl3.h>

struct TexInfo
{
	GLuint Tex;
	GLsizei Width, Height;
	float Left, Right, Bottom, Top;
};

class TexRect
{
public:
	TexRect(const char* path, const char* file);
	~TexRect();
	
	TexInfo GetTexRect(const char* name);
private:
	GLuint m_Texture;
	
	std::map<std::string, TexInfo> m_Rects;
};

#endif//_TEX_RECT_h_