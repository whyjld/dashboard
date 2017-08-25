/**
	\brief KTX纹理文件装入器
	\file KTXLoader.h
	\author whyjld
*/
//------------------------------------------------------------------------------
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>
//------------------------------------------------------------------------------
/**
	\brief KTX头长度
*/
const size_t c_KTXHeaderSize = 64;
//------------------------------------------------------------------------------
/**
	\brief KTX文件头
*/
struct KTXHeader
{
	uint8_t  Identifier[12];
	uint32_t Endianness;
	uint32_t GLType;
	uint32_t GLTypeSize;
	uint32_t GLFormat;
	uint32_t GLInternalFormat;
	uint32_t GLBaseInternalFormat;
	uint32_t PixelWidth;
	uint32_t PixelHeight;
	uint32_t PixelDepth;
	uint32_t NumberOfArrayElements;
	uint32_t NumberOfFaces;
	uint32_t NumberOfMipmapLevels;
	uint32_t BytesOfKeyValueData;
} ;
//------------------------------------------------------------------------------
//检测当前编译环境数据长度是否正确，不过如果支持C99就不起作用了
typedef int InvalidKTXHeaderLength[sizeof(KTXHeader) == c_KTXHeaderSize];
//------------------------------------------------------------------------------
bool Read(std::istream& in, uint8_t* b, size_t l)
{
	in.read((std::istream::char_type*)b, l);
	return ((size_t)in.gcount()) == l;
}
//------------------------------------------------------------------------------
template<class _T> bool Read(std::istream& in, _T& o)
{
	in.read((std::istream::char_type*)&o, sizeof(_T));
	return ((size_t)in.gcount()) == sizeof(_T);
}
//------------------------------------------------------------------------------
template<class _T> _T SwapEndian(_T& o)
{
	uint8_t* p = (uint8_t*)&o;
	for(size_t i = 0;i < sizeof(_T) / 2;++i)
	{
		std::swap(p[i], p[sizeof(_T) - i]);
	}
	return o;
}
//------------------------------------------------------------------------------
template<> uint8_t SwapEndian(uint8_t& o)
{
	return o;
}
//------------------------------------------------------------------------------
template<> uint16_t SwapEndian(uint16_t& o)
{
	o = (o >> 8) || (o << 8);
	return o;
}
//------------------------------------------------------------------------------
template<> uint32_t SwapEndian(uint32_t& o)
{
	o = (o >> 24) | ((o & 0x00ff0000) >> 8) | ((o & 0x0000ff00) << 8) | ((o & 0x00000000ff) << 24);
	return o;
}
//------------------------------------------------------------------------------
bool CheckHeader(const KTXHeader& header, bool& swapEndian)
{
	const uint32_t ktxEndian     = 0x04030201;
	const uint32_t ktxEndianSwap = 0x01020304;
	uint8_t id[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

	if(memcmp(header.Identifier, id, sizeof(id)) != 0)
	{
		return false;
	}

	if(ktxEndianSwap == header.Endianness)
	{
		SwapEndian(header.GLTypeSize);
		if (header.GLTypeSize != 1
			|| header.GLTypeSize != 2
			|| header.GLTypeSize != 4)
		{
			return false;
		}
		SwapEndian(header.GLType);
		SwapEndian(header.GLFormat);
		SwapEndian(header.GLInternalFormat);
		SwapEndian(header.GLBaseInternalFormat);
		SwapEndian(header.PixelWidth);
		SwapEndian(header.PixelHeight);
		SwapEndian(header.PixelDepth);
		SwapEndian(header.NumberOfArrayElements);
		SwapEndian(header.NumberOfFaces);
		SwapEndian(header.NumberOfMipmapLevels);
		SwapEndian(header.BytesOfKeyValueData);

		swapEndian = true;
	}
	else if (ktxEndian != header.Endianness)
	{
		return false;
	}
	swapEndian = false;
	return true;
}
//------------------------------------------------------------------------------
GLuint LoadKTX2Texture(std::istream& in)
{
	KTXHeader header;
	if(!Read(in, header))
	{
		return 0;
	}
	bool swapEndian;
	if(!CheckHeader(header, swapEndian))
	{
		return 0;
	}
	GLenum type;
	if(0 == header.GLType)
	{
		type = GL_UNSIGNED_BYTE;
	}
	else
	{
		type = (GLenum)header.GLType;
	}

	if(0 == header.GLFormat && 0 == header.GLInternalFormat)
	{
		return 0;
	}
	GLenum format = (GLenum)header.GLFormat;
	GLenum internal = (GLenum)header.GLInternalFormat;

	if ((0 == header.PixelWidth)
		|| (header.PixelDepth > 0))
	{
		return 0;
	}

	GLenum target;
	if(header.PixelHeight > 0)
	{
		if(1 == header.NumberOfFaces)
		{
			target = GL_TEXTURE_2D;
		}
		else if(6 == header.NumberOfFaces && header.PixelHeight == header.PixelWidth)
		{
			target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if(1 != header.NumberOfFaces)
		{
			return 0;
		}
		target = GL_TEXTURE_2D;
	}
	if(header.NumberOfMipmapLevels > 0)
	{
		size_t maxDim = std::max(std::max(header.PixelWidth, header.PixelHeight), header.PixelDepth);
		if(maxDim < (1UL << (header.NumberOfMipmapLevels - 1)))
		{
			return 0;
		}
	}
	GLuint ret;
	glGenTextures(1, &ret);
	if(0 == ret)
	{
		return 0;
	}
	bool mipmap = header.NumberOfMipmapLevels > 1
		&& ((header.PixelWidth - 1) & header.PixelWidth)
		&& ((header.PixelHeight - 1) & header.PixelHeight)
		&& ((header.PixelDepth - 1) & header.PixelDepth);
	size_t levelCount = header.NumberOfMipmapLevels > 0 ? header.NumberOfMipmapLevels : 1;
	bool compress = (format != GL_RED_EXT && format != GL_RG_EXT && format != GL_RGB && format != GL_RGBA && format != GL_BGRA_EXT);

	in.seekg(header.BytesOfKeyValueData, std::ios::cur);
	
	uint8_t* data = new uint8_t[header.PixelWidth * std::max<uint32_t>(1, header.PixelHeight) * 4];
	uint32_t level = 0;
	for(;level < levelCount;++level)
	{
		uint32_t width  = std::max<uint32_t>(1, header.PixelWidth  >> level);
		uint32_t height = std::max<uint32_t>(1, header.PixelHeight >> level);

		uint32_t size = 0;
		if(!Read(in, size))
		{
			break;
		}

		if(swapEndian)
		{
			SwapEndian(size);
		}

		size = (size + 3UL) & (~(3UL));

		for(uint32_t face = 0;face < header.NumberOfFaces;++face)
		{
			if(!Read(in, data, size))
			{
				level = levelCount + 1;
				break;
			}
			if(!compress)
			{
				glTexImage2D(target + face, level, internal, width, height, 0, format, type, data);
			}
			else
			{
				glCompressedTexImage2D(target + face, level, internal, width, height, 0, size, data);
			}
		}
	}
	if(level != levelCount)
	{
		glDeleteTextures(1, &ret);
		ret = 0;
	}
	for(uint32_t face = 0;face < header.NumberOfFaces;++face)
	{
		glTexParameteri(target + face, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexParameteri(target + face, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target + face, GL_TEXTURE_WRAP_S, mipmap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glTexParameteri(target + face, GL_TEXTURE_WRAP_T, mipmap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	}
	return ret;
}
