#ifndef _DASHBOARD_h_
#define _DASHBOARD_h_

#include "billboard.h"
#include "texrect.h"
#include "renderitem.h"
#include "shareMemory.h"
#include "datas.h"
#include <vector>
#include <sys/time.h>

class DashBoard
{
public:
    enum Direction
    {
        dStop,
        dForward,
        dBackward,
    };
    
    enum
    {
        StepCount = 6,
    };

	DashBoard(Billboard* bb);
	DashBoard(const DashBoard&) = delete;
	~DashBoard();
	
	void Draw();
private:
    long StepTime(long end);
	long MicroSecond();
	void PreviousStep();
	void NextStep();
	void Stop();

    void DisplayMph(long v);
    void DisplayInstrument();
	bool Step1();
	bool Step2();
	bool Step3();
	bool Step4();
	bool Step5();
	bool Step6();


	ShareMemory<Datas> m_DestDatas;
	Datas m_CurrDatas;
	float m_MphProg;
	
	Billboard* m_Billboard;
	TexRect m_Textures;
    	
	timeval m_StartTime;
	
	long m_StepStart;
	long m_Time;
	long m_Second;
	
	Direction m_Dir;
	size_t m_Step;

	RectItem m_Background;
	RectItem m_Center;
	ArcItem m_Enginespeed;
	ArcItem m_LeftOutter;
	ArcItem m_RightOutter;
	ArcItem m_LeftUpBlock;
	ArcItem m_LeftBottomBlock;
	ArcItem m_RightUpBlock;
	ArcItem m_RightBottomBlock;
	ArcItem m_LeftUpBar;
	ArcItem m_LeftBottomBar;
	ArcItem m_RightUpBar;
	ArcItem m_RightBottomBar;
	RectItem m_LeftSide;
	RectItem m_RightSide;
	RectItem m_EcoSide;
	ArcItem m_GasOil;
	ArcItem m_Temp;
	RectItem m_TripComputer;
	RectItem m_Assistance;
	RectItem m_EcoStatistics;
	RectItem m_SkinSettings;
	RectItem m_Service;
	RectItem m_Light;
	RectItem m_EcoLight;
	ArcItem m_EcoBar;
	RectItem m_OneLeaf;
	ArcItem m_LeftFunc;
	ArcItem m_RightFunc;
	std::vector<RectItem> m_MphUnits;

};
#endif//_DASHBOARD_h_
