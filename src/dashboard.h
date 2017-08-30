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

	virtual void SetPosition(float x, float y);

	virtual void Draw() = 0;
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
	
	int* m_Usage;
};

class RectItem : public RenderItem
{
public:
	RectItem(const TexInfo& tex, Billboard* bb);
	RectItem(const RectItem& v);
	~RectItem();
	
	RectItem& operator=(const RectItem& v);
	
	void Draw();
private:
	void SetAttribute();
};

class ArcItem : public RenderItem
{
public:
	ArcItem(const TexInfo& tex, GLsizei slice, Billboard* bb);
	ArcItem(const ArcItem& v);
	~ArcItem();
	
	ArcItem& operator=(const ArcItem& v);

	void SetArc(float radius, float begin, float radian);
	void SetProgress(float p);
	
	void Draw();
private:
	void SetAttribute();
	
	float m_R;
	float m_B;
	float m_A;
	float m_P;
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
	
	ArcItem m_EngineSpeed;
	
	ArcItem m_GasOil;
	ArcItem m_Temp;
	
	clock_t m_StepStart;
	clock_t m_Time;
	
	size_t m_Step;
};

#endif//_DASHBOARD_h_