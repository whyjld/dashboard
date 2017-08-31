#include "renderitem.h"
#include <vector>
#include <math.h>

RenderItem::RenderItem(const TexInfo& tex, GLsizei slice, Billboard* bb)
 : m_BB(bb)
 , m_TexInfo(tex)
 , m_ItemInfo(bb->CreateItems(slice))
 , m_X(0.0f)
 , m_Y(0.0f)
 , m_Scale(1.0f)
 , m_Usage(new int(1))
{
}

RenderItem::RenderItem(const RenderItem& v)
 : m_BB(v.m_BB)
 , m_TexInfo(v.m_TexInfo)
 , m_ItemInfo(v.m_ItemInfo)
 , m_X(v.m_X)
 , m_Y(v.m_Y)
 , m_Scale(v.m_Scale)
 , m_Usage(v.m_Usage)
{
	++(*m_Usage);
}

RenderItem::~RenderItem()
{
	Release();
}

void RenderItem::SetPosition(float x, float y)
{
	m_X = x;
	m_Y = y;
}

void RenderItem::SetScale(float scale)
{
	m_Scale = scale;
}
	
void RenderItem::Draw()
{
	m_BB->SetTranslation(m_X, m_Y, m_Scale);
	m_BB->Draw(m_ItemInfo, m_ItemInfo.Count);
}

void RenderItem::Release()
{
	if(--(*m_Usage) == 0)
	{
		delete m_Usage;
		glDeleteVertexArrays(1, &m_ItemInfo.VAO);
		glDeleteBuffers(1, &m_ItemInfo.PT);
	}
}

void RenderItem::Dump(const RenderItem& v)
{
	m_BB = v.m_BB;
	m_Usage = v.m_Usage;
	m_TexInfo = v.m_TexInfo;
	m_ItemInfo = v.m_ItemInfo;
	m_X = v.m_X;
	m_Y = v.m_Y;
	m_Scale = v.m_Scale;
	++(*m_Usage);
}

RectItem::RectItem(const TexInfo& tex, uint32_t mode, Billboard* bb)
 : RenderItem(tex, 4, bb)
{
	SetAttribute(mode);
}

RectItem::RectItem(const RectItem& v)
 : RenderItem(v)
{
}

RectItem::~RectItem()
{
}

RectItem& RectItem::operator=(const RectItem& v)
{
	if(this != &v)
	{
		Release();
		Dump(v);
	}
	return *this;
}

void RectItem::SetAttribute(uint32_t mode)
{
	float left, right, bottom, top;
	
	if(0 == (mode & fmVertical))
	{
		top = m_TexInfo.Top;
		bottom = m_TexInfo.Bottom;
	}
	else
	{
		top = m_TexInfo.Bottom;
		bottom = m_TexInfo.Top;
	}
	
	if(0 == (mode & fmHorizonal))
	{
		left = m_TexInfo.Left;
		right = m_TexInfo.Right;
	}
	else
	{
		left = m_TexInfo.Right;
		right = m_TexInfo.Left;
	}
	
	float buffer[4 * 4];
	buffer[0 * 4 + 0] = -m_TexInfo.Width / 2;
	buffer[0 * 4 + 1] =  m_TexInfo.Height / 2;
	buffer[0 * 4 + 2] =  left;
	buffer[0 * 4 + 3] =  bottom;

	buffer[1 * 4 + 0] = -m_TexInfo.Width / 2;
	buffer[1 * 4 + 1] = -m_TexInfo.Height / 2;
	buffer[1 * 4 + 2] =  left;
	buffer[1 * 4 + 3] =  top;

	buffer[2 * 4 + 0] =  m_TexInfo.Width / 2;
	buffer[2 * 4 + 1] =  m_TexInfo.Height / 2;
	buffer[2 * 4 + 2] =  right;
	buffer[2 * 4 + 3] =  bottom;

	buffer[3 * 4 + 0] =  m_TexInfo.Width / 2;
	buffer[3 * 4 + 1] = -m_TexInfo.Height / 2;
	buffer[3 * 4 + 2] =  right;
	buffer[3 * 4 + 3] =  top;
	
	m_BB->SetItemsAttribute(m_ItemInfo, buffer, 4);	
}

ArcItem::ArcItem(const TexInfo& tex, GLsizei slice, Billboard* bb)
 : RenderItem(tex, slice * 2, bb)
 , m_R(0.0f)
 , m_B(0.0f)
 , m_A(0.0f)
 , m_P(0.0f)
{
}

ArcItem::ArcItem(const ArcItem& v)
 : RenderItem(v)
 , m_R(v.m_R)
 , m_B(v.m_B)
 , m_A(v.m_A)
 , m_P(v.m_P)
{
}

ArcItem::~ArcItem()
{
}
	
ArcItem& ArcItem::operator=(const ArcItem& v)
{
	if(this != &v)
	{
		Release();
		m_R = v.m_R;
		m_B = v.m_B;
		m_A = v.m_A;
		m_P = v.m_P;
		Dump(v);
	}
	return *this;
}
	
void ArcItem::SetArc(float radius, float begin, float radian)
{
	m_R = radius;
	m_B = begin / 180.0f * M_PI;
	m_A = radian / 180.0f * M_PI;
	
	SetAttribute();
}

void ArcItem::SetProgress(float p)
{
	m_P = p;
	
	SetAttribute();
}

void ArcItem::SetAttribute()
{
	std::vector<float> buffer(4 * m_ItemInfo.Count);
	
	float* is;
	float* os;
	if(m_A > 0)
	{
		is = &buffer[0];
		os = &buffer[4];
	}
	else
	{
		is = &buffer[4];
		os = &buffer[0];
	}
	
	GLsizei c = m_ItemInfo.Count / 2 - 1;
	float s = m_A * m_P / c;
	float t = (m_TexInfo.Top - m_TexInfo.Bottom) * m_P / c;
	for(GLsizei i = 0;i < c + 1;++i)
	{
		float a = m_B + s * i;
		float v = m_TexInfo.Bottom + t * i;
		float c = cos(a);
		float s = sin(a);
		
		float r = m_R - m_TexInfo.Width / 2;
		is[0] = c * r;
		is[1] = s * r;
		is[2] = m_TexInfo.Right;
		is[3] = v;
		
		r = m_R + m_TexInfo.Width / 2;
		os[0] = c * r;
		os[1] = s * r;
		os[2] = m_TexInfo.Left;
		os[3] = v;
	
		is += 4 * 2;
		os += 4 * 2;
	}
	
	m_BB->SetItemsAttribute(m_ItemInfo, &buffer[0], m_ItemInfo.Count);	
}
