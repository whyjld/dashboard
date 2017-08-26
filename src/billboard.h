#ifndef _BILLBOARD_h_
#define _BILLBOARD_h_

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>

/* GL_VIV_direct_texture */
#ifndef GL_VIV_direct_texture
#define GL_VIV_YV12                     0x8FC0
#define GL_VIV_NV12                     0x8FC1
#define GL_VIV_YUY2                     0x8FC2
#define GL_VIV_UYVY                     0x8FC3
#define GL_VIV_NV21                     0x8FC4
#endif

typedef void (GL_APIENTRY *PFNGLTEXDIRECTVIV) (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid ** Pixels);
typedef void (GL_APIENTRY *PFNGLTEXDIRECTINVALIDATEVIV) (GLenum Target);

extern PFNGLTEXDIRECTVIV glTexDirectVIV;
extern PFNGLTEXDIRECTINVALIDATEVIV glTexDirectInvalidateVIV;

struct ItemInfo
{
	GLsizei Count;
	GLuint PT;
	GLuint VAO;
};

class Billboard
{
public:
	Billboard(GLsizei count);
	~Billboard();
	
	ItemInfo CreateItems(GLsizei count) const;
	void SetItemsAttribute(const ItemInfo& items, const float* pt, size_t count) const;

	void SetLogicSize(GLsizei width, GLsizei height);

	void Begin() const;
	void SetAlpha(float alpha);
	void Draw(const ItemInfo& items, size_t count);
	void End() const;
private:
	void InitEGL();
	void CleanUpEGL();
	void InitIndices();
	void InitShaders();

	GLuint CreateVertices(GLsizei count) const;
	GLuint CreateVAO(GLuint pt) const;

	GLuint CreateProgram(const char* vss, const char* pss);

	GLsizei m_ItemCount;

	EGLNativeDisplayType m_NativeDisplay;
	EGLNativeWindowType  m_NativeWindow;
	EGLDisplay			 m_Display;
	EGLSurface			 m_Surface;
	EGLContext			 m_Context;
	
	GLuint m_Width;
	GLuint m_Height;

	GLuint m_Indices;

	GLuint m_PTLocation;
	GLuint m_AlphaLocation;
	GLuint m_VPLocation;

	GLuint m_Program;
};

#endif//_BILLBOARD_h_