#ifndef _KTX_h_
#define _KTX_h_
//------------------------------------------------------------------------------
#include "GLES2/gl2.h"
#include <cstdint>
//------------------------------------------------------------------------------
/**
	\brief KTX头长度
*/
const aeUINT c_KTXHeaderSize = 64;
//------------------------------------------------------------------------------
/**
	\brief KTX文件头
*/
struct aeKTXHeader
{
	aeUINT8  Identifier[12];
	aeUINT32 Endianness;
	aeUINT32 GLType;
	aeUINT32 GLTypeSize;
	aeUINT32 GLFormat;
	aeUINT32 GLInternalFormat;
	aeUINT32 GLBaseInternalFormat;
	aeUINT32 PixelWidth;
	aeUINT32 PixelHeight;
	aeUINT32 PixelDepth;
	aeUINT32 NumberOfArrayElements;
	aeUINT32 NumberOfFaces;
	aeUINT32 NumberOfMipmapLevels;
	aeUINT32 BytesOfKeyValueData;
} ;
//------------------------------------------------------------------------------
//检测当前编译环境数据长度是否正确，不过如果支持C99就不起作用了
typedef int InvalidKTXHeaderLength[sizeof(aeKTXHeader) == c_KTXHeaderSize];
//------------------------------------------------------------------------------
/**
	\brief KTX文件装入器
*/
class KTXLoader
{
public:
	static aeBOOL Load(std::istream& in, ImageFileImpl* texFile, const std::vector<aeTextureFormat>& formats)
	{
		try
		{
			KTXLoader(in, texFile, formats);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	static aeBOOL Loadable(const std::string& ext)
	{
		return ext == ".ktx";
	}
	static aeVOID getLoadable(std::vector<std::string>& loadable)
	{
		loadable.push_back(".ktx");
	}
protected:
	KTXLoader(std::istream& in, ImageFileImpl* texFile, const std::vector<aeTextureFormat>& formats)
	 : f_TexFile(texFile), f_NeedSwapEndian(false)
	{
		if(!Read(in, f_Header))
		{
			_THROW_ERR_("数据不足，无法读取KTX文件头");
		}
		CheckHeader();
		SaveTextureInfomation();
		std::vector<aeTextureFormat>::const_iterator i = formats.begin();
		for(;i != formats.end();++i)
		{
			if(*i == texFile->getFormat())
			{
				break;
			}
		}
		if(formats.end() == i)
		{
			_THROW_ERR_("不支持纹理格式[%x]", texFile->getFormat());
		}
		in.seekg(f_Header.BytesOfKeyValueData, std::ios::cur);
		for(aeUINT32 level = 0;level < f_Header.NumberOfMipmapLevels;++level)
		{
			aeUINT32 width  = max(1UL, f_Header.PixelWidth  >> level);
			aeUINT32 height = max(1UL, f_Header.PixelHeight >> level);
			aeUINT32 depth  = max(1UL, f_Header.PixelDepth  >> level);

			aeUINT32 size = 0;
			if(!Read(in, size))
			{
				_THROW_ERR_("数据不足，无法读取KTX文件第[%d]层数据大小", level);
			}

			if(f_NeedSwapEndian)
			{
				SwapEndian(size);
			}

			aeUINT32 roundSize = (size + 3UL) & (~(3UL));

			for(aeUINT32 face = 0;face < f_Header.NumberOfFaces;++face)
			{
				auto_buf<aeBYTE> buf(roundSize);
				if(!Read(in, buf.get(), buf.size()))
				{
					_THROW_ERR_("数据不足，无法读取KTX文件第[%d]层数据", level);
				}

				if(f_NeedSwapEndian)
				{
					if(2 == f_Header.GLTypeSize)
					{
						SwapEndian((aeUINT16*)buf.get(), size / 2);
					}
					else if(4 == f_Header.GLTypeSize)
					{
						SwapEndian((aeUINT32*)buf.get(), size / 4);
					}
				}

				f_TexFile->AddData(new TextureDataImpl(width, height, depth, roundSize, buf.get()));
				buf.release();
			}
		}
	}
	/**
		\brief 校验KTX文件头
	*/
	aeVOID CheckHeader()
	{
		const aeUINT32 ktxEndian     = 0x04030201;
		const aeUINT32 ktxEndianSwap = 0x01020304;
		aeBYTE id[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

		if(memcmp(f_Header.Identifier, id, sizeof(id)) != 0)
		{
			_THROW_ERR_("数据无效，没有KTX文件标记")
		}

		if(ktxEndianSwap == f_Header.Endianness)
		{
#define SWAP_ENDIAN(x) f_Header.x = SwapEndian(f_Header.x);
			SWAP_ENDIAN(GLTypeSize);
			if (f_Header.GLTypeSize != 1
				|| f_Header.GLTypeSize != 2
				|| f_Header.GLTypeSize != 4)
			{
				_ERR_MSG_("数据无效，不支持的KTX文件类型")
			}
			SWAP_ENDIAN(GLType);
			SWAP_ENDIAN(GLFormat);
			SWAP_ENDIAN(GLInternalFormat);
			SWAP_ENDIAN(GLBaseInternalFormat);
			SWAP_ENDIAN(PixelWidth);
			SWAP_ENDIAN(PixelHeight);
			SWAP_ENDIAN(PixelDepth);
			SWAP_ENDIAN(NumberOfArrayElements);
			SWAP_ENDIAN(NumberOfFaces);
			SWAP_ENDIAN(NumberOfMipmapLevels);
			SWAP_ENDIAN(BytesOfKeyValueData);

			f_NeedSwapEndian = true;
#undef SWAP_ENDIAN
		}
		else if (ktxEndian != f_Header.Endianness)
		{
			_THROW_ERR_("数据无效，无效的KTX文件存储方式")
		}
	}
	/**
		\brief 设置纹理信息
	*/
	aeVOID SaveTextureInfomation()
	{
		if(0 == f_Header.GLType * f_Header.GLFormat
		   && 0 != f_Header.GLType + f_Header.GLFormat)
		{
			_THROW_ERR_("数据无效，KTX纹理格式无效")
		}

		if(0 != f_Header.GLFormat)
		{
			f_TexFile->setFormat(ConvertToTextureFormat(f_Header.GLFormat));
		}
		else if(0 != f_Header.GLInternalFormat)
		{
			f_TexFile->setFormat(ConvertToTextureFormat(f_Header.GLInternalFormat));
		}
		else
		{
			_THROW_ERR_("数据无效，KTX纹理格式无效")
		}

		if ((0 == f_Header.PixelWidth)
			|| (f_Header.PixelDepth > 0 && 0 == f_Header.PixelHeight))
		{
			_THROW_ERR_("数据无效，KTX纹理尺寸无效")
		}

		if (f_Header.PixelDepth > 0)
		{
			if(1 != f_Header.NumberOfFaces)
			{
				_THROW_ERR_("数据无效，KTX纹理面数量无效")
			}
			f_TexFile->setTextureType(ttTexture3D);
		}
		else if(f_Header.PixelHeight > 0)
		{
			if(1 == f_Header.NumberOfFaces)
			{
				f_TexFile->setTextureType(ttTexture2D);
			}
			else if(6 == f_Header.NumberOfFaces)
			{
				f_TexFile->setTextureType(ttTextureCubeMap);
			}
			else
			{
				_THROW_ERR_("数据无效，KTX纹理面数量无效")
			}
		}
		else
		{
			if(1 != f_Header.NumberOfFaces)
			{
				_THROW_ERR_("数据无效，KTX纹理面数量无效")
			}
			f_TexFile->setTextureType(ttTexture1D);
		}

		if(0 == f_Header.NumberOfMipmapLevels)
		{
			f_Header.NumberOfMipmapLevels = 1;
		}

		aeUINT maxDim = std::max(std::max(f_Header.PixelWidth,
		                                  f_Header.PixelHeight),
						         f_Header.PixelDepth);
		if(maxDim < (1UL << (f_Header.NumberOfMipmapLevels - 1)))
		{
			_THROW_ERR_("数据无效，KTX纹理层数无效")
		}


	}
	/**
		\brief 转换纹理类型
		\param format GL纹理类型
		\return AE纹理类型
	*/
	static aeTextureFormat ConvertToTextureFormat(GLenum format)
	{
		switch(format)
		{
			case GL_RED:
			case GL_R8:
				return tfR8i;
			case GL_RG8:
				return tfRG8i;
			case GL_RGB:
				return tfRGB8i;
			case GL_RGBA:
				return tfRGBA8i;
			case GL_DEPTH_COMPONENT16:
				return tfD16i;
			case GL_DEPTH_COMPONENT24:
				return tfD24i;
			case GL_DEPTH24_STENCIL8:
				return tfD24S8i;
			case GL_DEPTH_COMPONENT32:
				return tfD32i;
			case GL_R16F:
				return tfR16f;
			case GL_RG16F:
				return tfRG16f;
			case GL_RGB16F:
				return tfRGB16f;
			case GL_RGBA16F:
				return tfRGBA16f;
			case GL_R32F:
				return tfR32f;
			case GL_RG32F:
				return tfRG32f;
			case GL_RGB32F:
				return tfRGB32f;
			case GL_RGBA32F:
				return tfRGBA32f;
			case GL_DEPTH_COMPONENT32F:
				return tfD32f;
			case GL_DEPTH32F_STENCIL8:
				return tfD32fS8i;
			case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
				return tfRGBDXT1;
			case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
				return tfRGBADXT3;
			case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
				return tfRGBADXT5;
			case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
				return tfsRGBDXT1;
			case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
				return tfsRGBADXT3;
			case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
				return tfsRGBADXT5;
			case GL_ETC1_RGB8_OES:
				return tfETC1;
			case GL_COMPRESSED_RGB8_ETC2:
				return tfRGB8ETC2;
			case GL_COMPRESSED_SRGB8_ETC2:
				return tfsRGB8ETC2;
			case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
				return tfRGB8A1ETC2;
			case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
				return tfsRGB8A1ETC2;
			case GL_COMPRESSED_RGBA8_ETC2_EAC:
				return tfRGBA8ETC2;
			case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
				return tfsRGB8A8ETC2;
			case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
				return tfRGB8PVRTC4;
			case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
				return tfRGB8PVRTC2;
			case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
				return tfRGBA8PVRTC4;
			case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
				return tfRGBA8PVRTC2;
			default:
				return tfInvalid;
		};
	}
	/**
		\brief KTX文件头
	*/
	aeKTXHeader f_Header;
	/**
		\brief 纹理文件
	*/
	ImageFileImpl* f_TexFile;
	/**
		\brief 是否需要交换字节顺序
	*/
	aeBOOL f_NeedSwapEndian;
};
//------------------------------------------------------------------------------
_AE_IMPLEMENT_END_
//------------------------------------------------------------------------------
#endif//_KTX_h_
