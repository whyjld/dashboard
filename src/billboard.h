#ifndef _BILLBOARD_h_
#define _BILLBOARD_h_

#include "gli.h"

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
	void SetItemsAttribute(const ItemInfo& items, const float* pt, GLsizei count) const;

	void SetLogicSize(GLsizei width, GLsizei height);

	void Begin() const;
	void SetAlpha(float alpha);
	void SetPosition(float x, float y);
	void Draw(const ItemInfo& items, GLsizei count);
	void End() const;
private:
	void InitGL();
	void CleanUpGL();
	void InitIndices();
	void InitShaders();

	GLuint CreateVertices(GLsizei count) const;
	GLuint CreateVAO(GLuint pt) const;

	GLuint CreateProgram(const char* vss, const char* pss);
	
	OGLContext m_Context;

	GLsizei m_ItemCount;

	GLuint m_Indices;

	GLuint m_PTLocation;
	GLuint m_AlphaLocation;
	GLuint m_PosLocation;
	GLuint m_VPLocation;

	GLuint m_Program;
};

#endif//_BILLBOARD_h_