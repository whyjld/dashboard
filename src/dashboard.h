#ifndef _DASHBOARD_h_
#define _DASHBOARD_h_

#include "billboard.h"
#include "texrect.h"
#include "renderitem.h"
#include <vector>
#include <sys/time.h>

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
	void Step5();
	void Step6();
	
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
	std::vector<RectItem> m_ECONums;
	
	RectItem m_ECOGAUGE;
	RectItem m_MAINMENU;
	
	RectItem m_TripComputer;
	RectItem m_Assistance;
	RectItem m_EcoStatistics;
	RectItem m_SkinSettings;
	RectItem m_Service;
	
	ArcItem m_ECOBar;
	RectItem m_ECOLight;
	
	timeval m_StartTime;
	timeval m_CurrentTime;
	
	long m_StepStart;
	long m_Time;
	long m_Second;
	
	size_t m_Step;
};

#endif//_DASHBOARD_h_