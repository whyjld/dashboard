#include "texrect.h"
#include <stdexcept>

TexRect::TexRect(const char* texname)
 : m_Texture(0)
{
	glGenTextures(1, &m_Texture);
}

TexRect::~TexRect()
{
	glDeleteTextures(1, &m_Texture);
}

TexInfo TexRect::GetTexRect(const char* name)
{
	TexInfo ret = {0, 0.0f, 0.0f, 0.0f, 0.0f};
	auto i = m_Rects.find(name);
	if(i != m_Rects.end())
	{
		ret = i.second;
	}
	return ret;
}
