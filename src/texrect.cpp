#include <rapidjson/document.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <webp/decode.h>
#include "texrect.h"
#include "tga.h"

#define GET_VALUE(v, p, n, t, m) \
{ \
	if (!p.HasMember(#n)) \
	{ \
		throw std::runtime_error("Invalid texture file, " #n " node is missing."); \
	} \
	auto& n = p[#n]; \
	if (!n.t()) \
	{ \
		throw std::runtime_error("Invalid texture file, " #n "node has wrong type."); \
	} \
	v = n.m(); \
}

#define GET_NODE(p, n) \
if (!p.HasMember(#n)) \
{ \
	throw std::runtime_error("Invalid texture file, " #n " node is missing."); \
} \
auto& n = p[#n]; \
if (!n.IsObject()) \
{ \
	throw std::runtime_error("Invalid texture file, " #n " node has wrong type."); \
}

TexRect::TexRect(const char* path, const char* file)
 : m_Texture(0)
{
	std::string pathName = path;
	std::string text;
	{
		std::ifstream inf((pathName + file).c_str());
		if (!inf)
		{
			throw std::runtime_error("Can't load texture file.");
		}

		for (std::string line; std::getline(inf, line);)
		{
			text += line;
			text += "\r\n";
		}
	}

	rapidjson::Document doc;
	doc.Parse(text.c_str());
	if (!doc.IsObject())
	{
		throw std::runtime_error("Invalid texture file.");
	}
	std::string texFile;
	GET_VALUE(texFile, doc, file, IsString, GetString);
	
	GLsizei width, height;
	LoadTexture(pathName + texFile, width, height);
	
	GET_NODE(doc, frames);
	for (rapidjson::Value::ConstMemberIterator i = frames.MemberBegin();i != frames.MemberEnd(); ++i)
	{
		auto& f = i->value;
		
		int tx, ty, tw, th;		
		GET_VALUE(tx, f, x, IsInt, GetInt);
		GET_VALUE(ty, f, y, IsInt, GetInt);
		GET_VALUE(tw, f, w, IsInt, GetInt);
		GET_VALUE(th, f, h, IsInt, GetInt);

		TexInfo info = 
		{
			m_Texture,
			tw,
			th,
			float(tx - 1) / (width - 1),
			float(tx - 1 + tw) / (width - 1),
			float(ty - 1) / (height - 1),
			float(ty - 1 + th) / (height - 1),
		};
		
		m_Rects[i->name.GetString()] = info;
	}
}

TexRect::~TexRect()
{
	glDeleteTextures(1, &m_Texture);
}

TexInfo TexRect::GetTexRect(const char* name)
{
	TexInfo ret = {0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f};
	auto i = m_Rects.find(name);
	if(i != m_Rects.end())
	{
		ret = i->second;
	}
	return ret;
}

void TexRect::LoadTexture(const std::string& file, GLsizei& width, GLsizei& height)
{
	GLenum format;

	uint8_t* image = nullptr;
	
	enum TexType
	{
		ttNone,
		ttTGA,
		ttWebP,
	};
	TexType texType = ttNone;
	if(file.find(".tga") != std::string::npos)
	{
		if(!LoadTGA(file.c_str(), width, height, format, image))
		{
			throw std::runtime_error("Can't load tga texture.");
		}
		texType = ttTGA;
	}
	else if(file.find(".webp") != std::string::npos)
	{
		std::ifstream inf(file.c_str(), std::ios::binary | std::ios::in);
		if(!inf)
		{
			throw std::runtime_error("Can't open webp texture.");
		}
		inf.seekg(0, std::ios::end);
		std::vector<uint8_t> buf(inf.tellg());
		inf.seekg(0, std::ios::beg);
		inf.read((char*)&buf[0], buf.size());
		if((size_t)inf.gcount() != buf.size())
		{
			throw std::runtime_error("Can't load webp texture");
		}
		int w, h;
		image = WebPDecodeRGBA(&buf[0], buf.size(), &w, &h);
		width = w;
		height = h;
		format = GL_RGBA;
		
		texType = ttWebP;
	}
	
	if(nullptr == image || ttNone == texType)
	{
		throw std::runtime_error("Invalid texture file.");
	}
	glGenTextures(1, &m_Texture);
	if(0 == m_Texture)
	{
		throw std::runtime_error("Can't create gl texture.");
	}

	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	bool mipmap = (0 == (width & (width - 1)) && 0 == (height & (height - 1)));
	GLenum minf;
	GLenum wrap;
	if(mipmap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		minf = GL_LINEAR_MIPMAP_LINEAR;
		wrap = GL_REPEAT;
	}
	else
	{
		minf = GL_LINEAR;
		wrap = GL_CLAMP_TO_EDGE;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	switch(texType)
	{
		case ttTGA:
			delete[] image;
			break;
		case ttWebP:
			WebPFree(image);
			break;
		case ttNone:
		default:
			break;
	};
}
