#ifndef _DASHBOARD_h_
#define _DASHBOARD_h_

#include "billboard.h"
#include "texrect.h"
#include <vector>
#include <ctime>

class RenderItem
{
public:
	virtual ~RenderItem();

	RenderItem& operator=(const RenderItem& v) = delete;

	virtual void Draw() = 0;
protected:
	RenderItem(const TexInfo& tex, GLsizei slice, Billboard* bb);
	RenderItem(const RenderItem& v);

	void Release();
		
	Billboard* m_BB;

	TexInfo m_TexInfo;
	ItemInfo m_ItemInfo;
	
	int* m_Usage;
};

class RectItem : public RenderItem
{
public:
	RectItem(const TexInfo& tex, Billboard* bb);
	RectItem(const RectItem& v);
	~RectItem();
	
	RectItem& operator=(const RectItem& v);
	
	void SetPosition(float x, float y);
	
	void Draw();
private:
	void SetAttribute(float x, float y);
};

class ArcItem : public RenderItem
{
public:
	ArcItem(const TexInfo& tex, GLsizei slice, Billboard* bb);
	ArcItem(const ArcItem& v);
	~ArcItem();
	
	ArcItem& operator=(const ArcItem& v);
	
	void SetPosition(float x, float y);
	
	void Draw();
private:
	void SetAttribute(float x, float y);
	
	Billboard* m_BB;

	TexInfo m_TexInfo;
	ItemInfo m_ItemInfo;
	
	int* m_Usage;
};

class DashBoard
{
public:
	DashBoard(Billboard* bb);
	DashBoard(const DashBoard&) = delete;
	~DashBoard();
	
	void Draw();
private:
	void NextStep();
	void Step1();
	void Step2();
	
	Billboard* m_Billboard;
	TexRect m_Textures;

	RectItem m_Background;
	RectItem m_Center;
	RectItem m_Mph;
	std::vector<RectItem> m_MphNums;
	
	clock_t m_StepStart;
	clock_t m_Time;
	
	size_t m_Step;
};

#endif//_DASHBOARD_h_