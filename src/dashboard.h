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
	DashBoard(Billboard* bb);
	DashBoard(const DashBoard&) = delete;
	~DashBoard();
	
	void Draw();
private:
	long MicroSecond();
	void NextStep();

    void DisplayMph(long v);
    void DisplayInstrument();
	void Step1();
	void Step2();
	void Step3();
	void Step4();


	ShareMemory<Datas> m_DestDatas;
	Datas m_CurrDatas;
	float m_MphProg;
	
	Billboard* m_Billboard;
	TexRect m_Textures;
    	
	timeval m_StartTime;
	
	long m_StepStart;
	long m_Time;
	long m_Second;
	
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
	ArcItem m_GasOil;
	ArcItem m_Temp;
	std::vector<RectItem> m_MphUnits;

};
#endif//_DASHBOARD_h_
