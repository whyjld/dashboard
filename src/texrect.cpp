#include "texrect.h"
#include "tga.h"
#include <rapidjson/document.h>
#include <stdexcept>
#include <iostream>

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
	GLenum format;
	m_Texture = LoadTGA((pathName + texFile).c_str(), width, height, format);
	if(0 == m_Texture)
	{
		throw std::runtime_error("Can't load texture.");
	}
	
	
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
