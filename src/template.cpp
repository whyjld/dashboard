#include "dashboard.h"
#include <iostream>
#include <cstring>
#include <cmath>

%classname%::%classname%(Billboard* bb)
 : m_DestDatas("DashBoard", true)
 , m_MphProg(0.0f)
 , m_Billboard(bb)
 , m_Textures("./", "texture1.json")
 , m_Dir(dStop)
 , m_Step(0)
%constructline%
{
    memset(m_DestDatas.get(), 0, sizeof(Datas));
    memset(&m_CurrDatas,      0, sizeof(Datas));

	m_Billboard->SetLogicSize(1920, 720);

%constructor%

	gettimeofday(&m_StartTime, NULL);
	m_Second = MicroSecond();
}

%classname%::~%classname%()
{
}

void %classname%::Draw()
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
%steps%
			default:
				break;
		};
	}
	m_Billboard->End();
}

long %classname%::StepTime(long end)
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

long %classname%::MicroSecond()
{
	timeval ct;
	gettimeofday(&ct, NULL);
	
	long sec  = ct.tv_sec  - m_StartTime.tv_sec;
	long usec = ct.tv_usec - m_StartTime.tv_usec;

	return sec * 1000L + long(usec / 1000.0 + 0.5);
}

void %classname%::PreviousStep()
{
    if(m_Step > 1)
    {
        --m_Step;
        m_Dir = dBackward;
        m_StepStart = m_Time;
	}
	else
	{
		m_Dir = dForward;
	}
	std::cout << "Step:" << m_Step << std::endl;
}

void %classname%::NextStep()
{
    if(m_Step < StepCount)
    {
        ++m_Step;
        m_Dir = dForward;
        m_StepStart = m_Time;
	}
	std::cout << "Step:" << m_Step << std::endl;
}

void %classname%::Stop()
{
	m_Dir = dStop;
	std::cout << "Step stop." << std::endl;
}

void %classname%::DisplayMph(long v)
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

void %classname%::DisplayInstrument()
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

%implement%
