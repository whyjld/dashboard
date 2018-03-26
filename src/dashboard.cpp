#include "dashboard.h"
#include <iostream>
#include <cstring>
#include <cmath>

DashBoard::DashBoard(Billboard* bb)
 : m_DestDatas("DashBoard", true)
 , m_MphProg(0.0f)
 , m_Billboard(bb)
 , m_Textures("./", "texture1.json")
 , m_Dir(dStop)
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
 , m_LeftUpBar(m_Textures.GetTexRect("blockbar"), 64, m_Billboard)
 , m_LeftBottomBar(m_Textures.GetTexRect("blockbar"), 64, m_Billboard)
 , m_RightUpBar(m_Textures.GetTexRect("blockbar"), 64, m_Billboard)
 , m_RightBottomBar(m_Textures.GetTexRect("blockbar"), 64, m_Billboard)
 , m_LeftSide(m_Textures.GetTexRect("side"), 0, m_Billboard)
 , m_RightSide(m_Textures.GetTexRect("side"), 2, m_Billboard)
 , m_EcoSide(m_Textures.GetTexRect("eco"), 0, m_Billboard)
 , m_GasOil(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)
 , m_Temp(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)
 , m_TripComputer(m_Textures.GetTexRect("Trip Computer"), 0, m_Billboard)
 , m_Assistance(m_Textures.GetTexRect("Assistance"), 0, m_Billboard)
 , m_EcoStatistics(m_Textures.GetTexRect("Eco Statistics"), 0, m_Billboard)
 , m_SkinSettings(m_Textures.GetTexRect("Skin Settings"), 0, m_Billboard)
 , m_Service(m_Textures.GetTexRect("Service"), 0, m_Billboard)
 , m_Light(m_Textures.GetTexRect("light"), 0, m_Billboard)
 , m_EcoLight(m_Textures.GetTexRect("light"), 0, m_Billboard)
 , m_EcoBar(m_Textures.GetTexRect("ecobar"), 64, m_Billboard)
 , m_OneLeaf(m_Textures.GetTexRect("1leaf"), 0, m_Billboard)
 , m_LeftFunc(m_Textures.GetTexRect("funcbar"), 64, m_Billboard)
 , m_RightFunc(m_Textures.GetTexRect("funcbar"), 64, m_Billboard)

{
    memset(m_DestDatas.get(), 0, sizeof(Datas));
    memset(&m_CurrDatas,      0, sizeof(Datas));

	m_Billboard->SetLogicSize(1920, 720);

	m_Background.SetPosition(960.0f, 358.0f);
	m_Center.SetPosition(960.0f, 358.0f);
	m_Enginespeed.SetPosition(960.0f, 358.0f);
	m_Enginespeed.SetArc(250.0f, 234.0f, -288.0f);
	m_LeftOutter.SetPosition(960.0f, 358.0f);
	m_LeftOutter.SetArc(280.0f, 230.0f, -100.0f);
	m_RightOutter.SetPosition(960.0f, 358.0f);
	m_RightOutter.SetArc(280.0f, -50.0f, 100.0f);
	m_LeftUpBlock.SetPosition(960.0f, 358.0f);
	m_LeftUpBlock.SetArc(900.0f, 150.0f, 15.0f);
	m_LeftBottomBlock.SetPosition(960.0f, 358.0f);
	m_LeftBottomBlock.SetArc(900.0f, 210.0f, 15.0f);
	m_RightUpBlock.SetPosition(960.0f, 358.0f);
	m_RightUpBlock.SetArc(900.0f, 150.0f, -15.0f);
	m_RightBottomBlock.SetPosition(960.0f, 358.0f);
	m_RightBottomBlock.SetArc(900.0f, 210.0f, -15.0f);
	m_LeftUpBar.SetPosition(960.0f, 358.0f);
	m_LeftUpBar.SetArc(925.0f, 150.0f, 45.0f);
	m_LeftBottomBar.SetPosition(960.0f, 358.0f);
	m_LeftBottomBar.SetArc(925.0f, 210.0f, 45.0f);
	m_RightUpBar.SetPosition(960.0f, 358.0f);
	m_RightUpBar.SetArc(925.0f, 150.0f, -45.0f);
	m_RightBottomBar.SetPosition(960.0f, 358.0f);
	m_RightBottomBar.SetArc(925.0f, 210.0f, -45.0f);
	m_LeftSide.SetPosition(460.0f, 358.0f);
	m_RightSide.SetPosition(1460.0f, 358.0f);
	m_EcoSide.SetPosition(1460.0f, 358.0f);
	m_GasOil.SetPosition(960.0f, 358.0f);
	m_GasOil.SetArc(203.0f, 225.0f, -90.0f);
	m_Temp.SetPosition(960.0f, 358.0f);
	m_Temp.SetArc(203.0f, -45.0f, 90.0f);
	m_TripComputer.SetPosition(460.0f, 520.0f);
	m_Assistance.SetPosition(400.0f, 440.0f);
	m_EcoStatistics.SetPosition(400.0f, 358.0f);
	m_SkinSettings.SetPosition(420.0f, 280.0f);
	m_Service.SetPosition(420.0f, 200.0f);
	m_Light.SetPosition(710.0f, 600.0f);
	m_EcoLight.SetPosition(710.0f, 600.0f);
	m_EcoBar.SetPosition(1492.0f, 365.0f);
	m_EcoBar.SetArc(116.0f, 247.5f, -315.0f);
	m_OneLeaf.SetPosition(1384.0f, 265.0f);
	m_LeftFunc.SetPosition(960.0f, 358.0f);
	m_LeftFunc.SetArc(740.0f, 230.0f, -100.0f);
	m_RightFunc.SetPosition(960.0f, 358.0f);
	m_RightFunc.SetArc(740.0f, -50.0f, 100.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph0"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph1"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph2"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph3"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph4"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph5"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph6"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph7"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph8"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);
	m_MphUnits.push_back(RectItem(m_Textures.GetTexRect("mph9"), 0, m_Billboard));
	m_MphUnits[m_MphUnits.size() - 1].SetPosition(960.0f, 358.0f);


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
	for(bool rendered = false;!rendered;)
	{
		switch(m_Step)
		{
			case 0:
				NextStep();
				break;
			case 1:
rendered = 				Step1();
				break;
			case 2:
rendered = 				Step2();
				break;
			case 3:
rendered = 				Step3();
				break;
			case 4:
rendered = 				Step4();
				break;
			case 5:
rendered = 				Step5();
				break;
			case 6:
rendered = 				Step6();
				break;

			default:
				break;
		};
	}
	m_Billboard->End();
}

long DashBoard::StepTime(long end)
{
    if(m_Dir == dForward)
    {
        return m_Time - m_StepStart;
    }
    else
    {
        return end - (m_Time - m_StepStart + 1);
    }
}

long DashBoard::MicroSecond()
{
	timeval ct;
	gettimeofday(&ct, NULL);
	
	long sec  = ct.tv_sec  - m_StartTime.tv_sec;
	long usec = ct.tv_usec - m_StartTime.tv_usec;

	return sec * 1000L + long(usec / 1000.0 + 0.5);
}

void DashBoard::PreviousStep()
{
    if(m_Step > 1)
    {
        --m_Step;
        m_Dir = dBackward;
        m_StepStart = m_Time;
	}
	std::cout << "Step:" << m_Step << std::endl;
}

void DashBoard::NextStep()
{
    if(m_Step < StepCount)
    {
        ++m_Step;
        m_Dir = dForward;
        m_StepStart = m_Time;
	}
	std::cout << "Step:" << m_Step << std::endl;
}

void DashBoard::Stop()
{
	m_Dir = dStop;
	std::cout << "Step stop." << std::endl;
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
    if(m_Step < 5)
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
}

bool DashBoard::Step1()
{
	const long ms = StepTime(1000);
	if(ms >= 0 && ms < 1000)
	{
		float alpha = ms * 0.001f;
		m_Billboard->SetAlpha(alpha);
		m_Background.Draw();
		m_Center.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}

    if(ms < 0)
    {
        PreviousStep();
        return false;
    }
    else if(ms < 1000)
    {
        DisplayInstrument();
        return true;
    }
    else
    {
        NextStep();
        return false;
    }
}
bool DashBoard::Step2()
{
	if(m_Dir != dStop)
	{
		Stop();
	}
	if(true)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Background.Draw();
		m_Center.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}

    if(m_DestDatas->View < 0)
    {
        m_DestDatas->View = 0;
        PreviousStep();
        return false;
    }
    else if(m_DestDatas->View > 0)
    {
        m_DestDatas->View = 0;
        NextStep();
        return false;
    }
    else
    {
        DisplayInstrument();
        return true;
    }
}
bool DashBoard::Step3()
{
	const long ms = StepTime(4500);
	if(ms >= 0 && ms < 4500)
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
	if(ms >= 2500 && ms < 3000)
	{
		float x = ms * 0.5f + -1030.0f;
		m_LeftSide.SetPosition(x, m_LeftSide.GetY());
		m_LeftSide.Draw();
	}
	if(ms >= 2500 && ms < 3000)
	{
		float x = ms * -0.5f + 2950.0f;
		m_RightSide.SetPosition(x, m_RightSide.GetY());
		m_RightSide.Draw();
	}
	if(ms >= 1000 && ms < 2500)
	{
		float start = ms * 0.03f + 65.0f;
		m_LeftUpBar.SetArc(m_LeftUpBar.GetRadius(), start, m_LeftUpBar.GetRadian());
		m_LeftUpBar.Draw();
	}
	if(ms >= 1000 && ms < 2500)
	{
		float start = ms * -0.03f + 250.0f;
		m_LeftBottomBar.SetArc(m_LeftBottomBar.GetRadius(), start, m_LeftBottomBar.GetRadian());
		m_LeftBottomBar.Draw();
	}
	if(ms >= 1000 && ms < 2500)
	{
		float start = ms * -0.03f + 115.0f;
		m_RightUpBar.SetArc(m_RightUpBar.GetRadius(), start, m_RightUpBar.GetRadian());
		m_RightUpBar.Draw();
	}
	if(ms >= 1000 && ms < 2500)
	{
		float start = ms * 0.03f + 290.0f;
		m_RightBottomBar.SetArc(m_RightBottomBar.GetRadius(), start, m_RightBottomBar.GetRadian());
		m_RightBottomBar.Draw();
	}
	if(ms >= 2000 && ms < 2500)
	{
		m_LeftUpBlock.Draw();
		m_LeftBottomBlock.Draw();
		m_RightUpBlock.Draw();
		m_RightBottomBlock.Draw();
	}
	if(ms >= 1000 && ms < 2500)
	{
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	if(ms >= 2500 && ms < 4500)
	{
		m_LeftUpBar.Draw();
		m_LeftBottomBar.Draw();
		m_RightUpBar.Draw();
		m_RightBottomBar.Draw();
		m_LeftUpBlock.Draw();
		m_LeftBottomBlock.Draw();
		m_RightUpBlock.Draw();
		m_RightBottomBlock.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	if(ms >= 3000 && ms < 3500)
	{
		float alpha = ms * 0.002f + -6.0f;
		m_Billboard->SetAlpha(alpha);
		m_TripComputer.Draw();
		m_Assistance.Draw();
		m_EcoStatistics.Draw();
		m_SkinSettings.Draw();
		m_Service.Draw();
		m_EcoSide.Draw();
		m_EcoBar.Draw();
		m_OneLeaf.Draw();
	}
	if(ms >= 3000 && ms < 3500)
	{
		m_Billboard->SetAlpha(1.0f);
		m_LeftSide.Draw();
	}
	if(ms >= 3000 && ms < 3500)
	{
		float alpha = ms * -0.002f + 7.0f;
		m_Billboard->SetAlpha(alpha);
		m_RightSide.Draw();
	}
	if(ms >= 3500 && ms < 4500)
	{
		m_Billboard->SetAlpha(1.0f);
		m_LeftSide.Draw();
		m_EcoSide.Draw();
		m_EcoBar.Draw();
		m_OneLeaf.Draw();
		m_TripComputer.Draw();
		m_Assistance.Draw();
		m_EcoStatistics.Draw();
		m_SkinSettings.Draw();
		m_Service.Draw();
	}
	if(ms >= 3500 && ms < 3600)
	{
		float alpha = ms * 0.01f + -34.1f;
		m_Billboard->SetAlpha(alpha);
		float x = ms * -0.8f + 3510.0f;
		m_Light.SetPosition(x, m_Light.GetY());
		m_Light.Draw();
	}
	if(ms >= 3500 && ms < 3900)
	{
		float alpha = 2.0f;
		m_Billboard->SetAlpha(alpha);
		float x = ms * -0.8f + 3510.0f;
		m_Light.SetPosition(x, m_Light.GetY());
		m_Light.Draw();
	}
	if(ms >= 3900 && ms < 4000)
	{
		float alpha = ms * -0.01f + 41.0f;
		m_Billboard->SetAlpha(alpha);
		float x = ms * -0.8f + 3510.0f;
		m_Light.SetPosition(x, m_Light.GetY());
		m_Light.Draw();
	}
	if(ms >= 3500 && ms < 3600)
	{
		float alpha = ms * 0.01f + -34.1f;
		m_Billboard->SetAlpha(alpha);
		float x = ms * -0.8f + 3510.0f;
		m_EcoLight.SetPosition(x, m_EcoLight.GetY());
		m_EcoLight.Draw();
	}
	if(ms >= 3500 && ms < 3900)
	{
		float alpha = 2.0f;
		m_Billboard->SetAlpha(alpha);
		float x = ms * -0.8f + 3510.0f;
		m_EcoLight.SetPosition(x, m_EcoLight.GetY());
		m_EcoLight.Draw();
	}

    if(ms < 0)
    {
        PreviousStep();
        return false;
    }
    else if(ms < 4500)
    {
        DisplayInstrument();
        return true;
    }
    else
    {
        NextStep();
        return false;
    }
}
bool DashBoard::Step4()
{
	if(m_Dir != dStop)
	{
		Stop();
	}
	if(true)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Background.Draw();
		m_Center.Draw();
		m_LeftSide.Draw();
		m_EcoSide.Draw();
		m_LeftUpBar.Draw();
		m_LeftBottomBar.Draw();
		m_RightUpBar.Draw();
		m_RightBottomBar.Draw();
		m_LeftUpBlock.Draw();
		m_LeftBottomBlock.Draw();
		m_RightUpBlock.Draw();
		m_RightBottomBlock.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
		m_TripComputer.Draw();
		m_Assistance.Draw();
		m_EcoStatistics.Draw();
		m_SkinSettings.Draw();
		m_Service.Draw();
		m_EcoBar.Draw();
		m_OneLeaf.Draw();
	}

    if(m_DestDatas->View < 0)
    {
        m_DestDatas->View = 0;
        PreviousStep();
        return false;
    }
    else if(m_DestDatas->View > 0)
    {
        m_DestDatas->View = 0;
        NextStep();
        return false;
    }
    else
    {
        DisplayInstrument();
        return true;
    }
}
bool DashBoard::Step5()
{
	const long ms = StepTime(2000);
	if(ms >= 0 && ms < 2000)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Background.Draw();
		m_LeftUpBar.Draw();
		m_LeftBottomBar.Draw();
		m_RightUpBar.Draw();
		m_RightBottomBar.Draw();
		m_LeftUpBlock.Draw();
		m_LeftBottomBlock.Draw();
		m_RightUpBlock.Draw();
		m_RightBottomBlock.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	if(ms >= 0 && ms < 1000)
	{
		float alpha = ms * -0.001f + 1.0f;
		m_Billboard->SetAlpha(alpha);
		m_TripComputer.Draw();
		m_Assistance.Draw();
		m_EcoStatistics.Draw();
		m_SkinSettings.Draw();
		m_Service.Draw();
		m_EcoBar.Draw();
		m_OneLeaf.Draw();
	}
	if(ms >= 0 && ms < 1000)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Center.Draw();
		m_LeftSide.Draw();
		m_EcoSide.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float alpha = ms * -0.001f + 2.0f;
		m_Billboard->SetAlpha(alpha);
		m_Center.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		m_Billboard->SetAlpha(1.0f);
		float x = ms * -0.5f + 970.0f;
		m_LeftSide.SetPosition(x, m_LeftSide.GetY());
		m_LeftSide.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float x = ms * 0.5f + 950.0f;
		m_EcoSide.SetPosition(x, m_EcoSide.GetY());
		m_EcoSide.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float start = ms * -0.1f + 430.0;
		m_LeftFunc.SetArc(m_LeftFunc.GetRadius(), start, m_LeftFunc.GetRadian());
		m_LeftFunc.Draw();
	}
	if(ms >= 1000 && ms < 2000)
	{
		float start = ms * 0.1f + -250.0f;
		m_RightFunc.SetArc(m_RightFunc.GetRadius(), start, m_RightFunc.GetRadian());
		m_RightFunc.Draw();
	}

    if(ms < 0)
    {
        PreviousStep();
        return false;
    }
    else if(ms < 2000)
    {
        DisplayInstrument();
        return true;
    }
    else
    {
        NextStep();
        return false;
    }
}
bool DashBoard::Step6()
{
	if(m_Dir != dStop)
	{
		Stop();
	}
	if(true)
	{
		m_Billboard->SetAlpha(1.0f);
		m_Background.Draw();
		m_LeftUpBar.Draw();
		m_LeftBottomBar.Draw();
		m_RightUpBar.Draw();
		m_RightBottomBar.Draw();
		m_LeftUpBlock.Draw();
		m_LeftBottomBlock.Draw();
		m_RightUpBlock.Draw();
		m_RightBottomBlock.Draw();
		m_LeftOutter.Draw();
		m_RightOutter.Draw();
		m_LeftFunc.Draw();
		m_RightFunc.Draw();
	}

    if(m_DestDatas->View < 0)
    {
        m_DestDatas->View = 0;
        PreviousStep();
        return false;
    }
    else if(m_DestDatas->View > 0)
    {
        m_DestDatas->View = 0;
        NextStep();
        return false;
    }
    else
    {
        DisplayInstrument();
        return true;
    }
}

