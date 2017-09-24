#include "dashboard.h"
#include <iostream>
#include <cstring>
#include <cmath>

DashBoard::DashBoard(Billboard* bb)
 : m_DestDatas("DashBoard", true)
 , m_MphProg(0.0f)
 , m_Billboard(bb)
 , m_Textures("./", "texture1.json")
 , m_Step(0)
 , m_Background(m_Textures.GetTexRect("background"), 0, m_Billboard)
 , m_Center(m_Textures.GetTexRect("center"), 0, m_Billboard)
 , m_Enginespeed(m_Textures.GetTexRect("enginespeed"), 64, m_Billboard)
 , m_LeftOutter(m_Textures.GetTexRect("outter"), 64, m_Billboard)
 , m_RightOutter(m_Textures.GetTexRect("outter"), 64, m_Billboard)
 , m_LeftUpBlock(m_Textures.GetTexRect("outblock"), 32, m_Billboard)
 , m_LeftBottomBlock(m_Textures.GetTexRect("outblock"), 32, m_Billboard)
 , m_RightUpBlock(m_Textures.GetTexRect("outblock"), 32, m_Billboard)
 , m_RightBottomBlock(m_Textures.GetTexRect("outblock"), 32, m_Billboard)
 , m_GasOil(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)
 , m_Temp(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)

{
    memset(m_DestDatas.get(), 0, sizeof(Datas));
    memset(&m_CurrDatas,      0, sizeof(Datas));

	m_Billboard->SetLogicSize(1920, 720);

	m_Background.SetPosition(960.0f, 360.0f);
	m_Center.SetPosition(960.0f, 360.0f);
	m_Enginespeed.SetPosition(960.0f, 360.0f);
	m_Enginespeed.SetArc(250.0f, 234.0f, -288.0f);
	m_LeftOutter.SetPosition(960.0f, 360.0f);
	m_LeftOutter.SetArc(280.0f, 230.0f, -100.0f);
	m_RightOutter.SetPosition(960.0f, 360.0f);
	m_RightOutter.SetArc(280.0f, -50.0f, 100.0f);
	m_LeftUpBlock.SetPosition(960.0f, 360.0f);
	m_LeftUpBlock.SetArc(900.0f, 150.0f, 15.0f);
	m_LeftBottomBlock.SetPosition(960.0f, 360.0f);
	m_LeftBottomBlock.SetArc(900.0f, 210.0f, 15.0f);
	m_RightUpBlock.SetPosition(960.0f, 360.0f);
	m_RightUpBlock.SetArc(900.0f, 150.0f, -15.0f);
	m_RightBottomBlock.SetPosition(960.0f, 360.0f);
	m_RightBottomBlock.SetArc(900.0f, 210.0f, -15.0f);
	m_GasOil.SetPosition(960.0f, 360.0f);
	m_GasOil.SetArc(203.0f, 225.0f, -90.0f);
	m_Temp.SetPosition(960.0f, 360.0f);
	m_Temp.SetArc(203.0f, -45.0f, 90.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph0"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph1"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph2"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph3"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph4"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph5"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph6"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph7"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph8"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph9"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 360.0f);


	gettimeofday(&m_StartTime, NULL);
	m_Second = MicroSecond();
}

DashBoard::~DashBoard()
{
}

void DashBoard::Draw()
{
	m_Time = MicroSecond();
	if(m_Time > m_Second + 1000)
	{
		m_Second = m_Time;
		std::cout << "Frame time :" << m_Time << std::endl;
	}
	m_Billboard->Begin();
		switch(m_Step)
		{
			case 0:
				NextStep();
				break;
			case 1:
				Step1();
				break;
			case 2:
				Step2();
				break;
			case 3:
				Step3();
				break;
			case 4:
				Step4();
				break;

			default:
				break;
		};
	m_Billboard->End();
}

long DashBoard::MicroSecond()
{
	timeval ct;
	gettimeofday(&ct, NULL);
	
	long sec  = ct.tv_sec  - m_StartTime.tv_sec;
	long usec = ct.tv_usec - m_StartTime.tv_usec;

	return sec * 1000L + long(usec / 1000.0 + 0.5);
}

void DashBoard::NextStep()
{
	++m_Step;
	m_StepStart = m_Time;
}

void DashBoard::DisplayMph(long v)
{
    if(v > 99)
    {
        auto x = m_MphUnits[0].GetX();
        auto y = m_MphUnits[0].GetY();
        
        long d = (v / 10) % 10;
        auto& t = m_MphUnits[d];
        t.Draw();

        d = v % 10;
        auto& u = m_MphUnits[d];
        u.SetPosition(x + 40, y);
        u.Draw();
        u.SetPosition(x, y);
        
        d = (v / 100) % 10;
        auto& h = m_MphUnits[d];
        h.SetPosition(x - 40, y);
        h.Draw();
        h.SetPosition(x, y);
        
        
    }
    else if(v > 9)
    {
        auto x = m_MphUnits[0].GetX();
        auto y = m_MphUnits[0].GetY();
        
        long d = v % 10;
        auto& u = m_MphUnits[d];
        u.SetPosition(x + 20, y);
        u.Draw();
        u.SetPosition(x, y);
        
        d = (v / 10) % 10;
        auto& t = m_MphUnits[d];
        t.SetPosition(x - 20, y);
        t.Draw();
        t.SetPosition(x, y);
    }
    else if(v < 10)
    {
        long d = v % 10;
        m_MphUnits[d].Draw();
    }
}

void DashBoard::DisplayInstrument()
{
    //mph
    long cv = m_CurrDatas.Speed;
    long dv = m_DestDatas->Speed;

    if(cv != dv)
    {
        m_MphProg += 0.1f;
        if(m_MphProg > 1.0f)
        {
            m_CurrDatas.Speed = m_DestDatas->Speed;
            m_MphProg = 0.0f;

            DisplayMph(dv);
        }
        else
        {
            float alpha = m_Billboard->GetAlpha();
            m_Billboard->SetAlpha(alpha * (1.0f - m_MphProg));
            DisplayMph(cv);
            m_Billboard->SetAlpha(alpha * m_MphProg);
            DisplayMph(dv);
            m_Billboard->SetAlpha(alpha);
        }
    }
    else
    {
        DisplayMph(cv);
    }
    
    //Engine speed
    m_Enginespeed.SetProgress(std::min(1.0f, m_DestDatas->EngineSpeed / 8000.0f));
    m_Enginespeed.Draw();
    //GasOil
    m_GasOil.SetProgress(std::min(1.0f, m_DestDatas->OilMass / 100.0f));
    m_GasOil.Draw();
    //Temperature
    m_Temp.SetProgress(std::min(1.0f, m_DestDatas->WaterTemperature / 99.0f));
    m_Temp.Draw();
}

void DashBoard::Step1()
{
	const long ms = m_Time - m_StepStart;
	if(ms >= 0 && ms < 1000)
	{
		float alpha = ms * 0.001f;
		m_Billboard->SetAlpha(alpha);
		float x = 960.0f;
		float y = 360.0f;
		m_Background.SetPosition(x, y);
		m_Center.SetPosition(x, y);
		m_LeftOutter.SetPosition(x, y);
		m_RightOutter.SetPosition(x, y);
		m_Background.Draw();
		m_Center.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	DisplayInstrument();
	if(ms >= 1000)
	{
		NextStep();
	}
}
void DashBoard::Step2()
{
	const long ms = m_Time - m_StepStart;
	if(ms >= 0 && ms < 3000)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Background.Draw();
		m_Center.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	DisplayInstrument();
	if(ms >= 3000)
	{
		NextStep();
	}
}
void DashBoard::Step3()
{
	const long ms = m_Time - m_StepStart;
	if(ms >= 0 && ms < 2000)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Background.Draw();
		m_Center.Draw();
	}
	if(ms >= 0 && ms < 1000)
	{
		float radius = ms * 0.52f + 280.0f;
		m_LeftOutter.SetArc(radius, m_LeftOutter.GetBegin(), m_LeftOutter.GetRadian());
		m_RightOutter.SetArc(radius, m_RightOutter.GetBegin(), m_RightOutter.GetRadian());
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float start = ms * 0.03f + 100.0f;
		m_LeftUpBlock.SetArc(m_LeftUpBlock.GetRadius(), start, m_LeftUpBlock.GetRadian());
		m_LeftUpBlock.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float start = ms * -0.03f + 245.0f;
		m_LeftBottomBlock.SetArc(m_LeftBottomBlock.GetRadius(), start, m_LeftBottomBlock.GetRadian());
		m_LeftBottomBlock.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float start = ms * -0.03f + 80.0f;
		m_RightUpBlock.SetArc(m_RightUpBlock.GetRadius(), start, m_RightUpBlock.GetRadian());
		m_RightUpBlock.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float start = ms * 0.03f + 295.0f;
		m_RightBottomBlock.SetArc(m_RightBottomBlock.GetRadius(), start, m_RightBottomBlock.GetRadian());
		m_RightBottomBlock.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	DisplayInstrument();
	if(ms >= 2000)
	{
		NextStep();
	}
}
void DashBoard::Step4()
{
	const long ms = m_Time - m_StepStart;
	if(ms >= 0 && ms < 3000)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Background.Draw();
		m_Center.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
		m_LeftUpBlock.Draw();
		m_LeftBottomBlock.Draw();
		m_RightUpBlock.Draw();
		m_RightBottomBlock.Draw();
	}
	DisplayInstrument();
	if(ms >= 3000)
	{
		NextStep();
	}
}

