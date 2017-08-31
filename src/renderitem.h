#ifndef _RENDERITEM_h_
#define _RENDERITEM_h_

#include "billboard.h"
#include "texrect.h"

class RenderItem
{
public:
	virtual ~RenderItem();

	RenderItem& operator=(const RenderItem& v) = delete;
	
	const TexInfo& GetTexInfo() const
	{
		return m_TexInfo;
	}
	
	virtual float GetX() const
	{
		return m_X;
	}
	virtual float GetY() const
	{
		return m_Y;
	}
	virtual float GetScale() const
	{
		return m_Scale;
	}

	virtual void SetPosition(float x, float y);
	virtual void SetScale(float scale);

	virtual void Draw();
protected:
	RenderItem(const TexInfo& tex, GLsizei slice, Billboard* bb);
	RenderItem(const RenderItem& v);

	void Release();
	void Dump(const RenderItem& v);
		
	Billboard* m_BB;

	TexInfo m_TexInfo;
	ItemInfo m_ItemInfo;
	
	float m_X;
	float m_Y;
	float m_Scale;
	
	int* m_Usage;
};

enum FlipMode
{
	fmVertical  = 0x01,
	fmHorizonal = 0x02,
};

class RectItem : public RenderItem
{
public:
	RectItem(const TexInfo& tex, uint32_t mode, Billboard* bb);
	RectItem(const RectItem& v);
	~RectItem();
	
	RectItem& operator=(const RectItem& v);
private:
	void SetAttribute(uint32_t mode);
};

class ArcItem : public RenderItem
{
public:
	ArcItem(const TexInfo& tex, GLsizei slice, Billboard* bb);
	ArcItem(const ArcItem& v);
	~ArcItem();
	
	ArcItem& operator=(const ArcItem& v);
	
	float GetRadius() const
	{
		return m_R;
	}
	
	float GetBegin() const
	{
		return m_B;
	}
	
	float GetRadian() const
	{
		return m_A;
	}
	
	float GetProgress() const
	{
		return m_P;
	}

	void SetArc(float radius, float begin, float radian);
	void SetProgress(float p);
private:
	void SetAttribute();
	
	float m_R;
	float m_B;
	float m_A;
	float m_P;
};

#endif//_RENDERITEM_h_