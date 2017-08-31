#ifndef _DASHBOARD_h_
#define _DASHBOARD_h_

#include "billboard.h"
#include "texrect.h"
#include <vector>
#include <sys/time.h>

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

	void SetArc(float radius, float begin, float radian);
	void SetProgress(float p);
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
	long MicroSecond();
	void NextStep();
	void Step1();
	void Step2();
	void Step3();
	void Step4();
	
	Billboard* m_Billboard;
	TexRect m_Textures;

	RectItem m_Background;
	RectItem m_Center;
	RectItem m_Mph;
	std::vector<RectItem> m_MphNums;
	
	ArcItem m_EngineSpeed;
	
	ArcItem m_GasOil;
	ArcItem m_Temp;
	
	std::vector<RectItem> m_Ends;
	
	RectItem m_LeftOutter;
	RectItem m_RightOutter;
	
	RectItem m_LeftSide;
	RectItem m_RightSide;
	RectItem m_ECOSide;
	
	RectItem m_ECOGAUGE;
	RectItem m_MAINMENU;
	
	RectItem m_TripComputer;
	RectItem m_Assistance;
	RectItem m_EcoStatistics;
	RectItem m_SkinSettings;
	RectItem m_Service;
	
	timeval m_StartTime;
	timeval m_CurrentTime;
	
	long m_StepStart;
	long m_Time;
	long m_Second;
	
	size_t m_Step;
};

#endif//_DASHBOARD_h_