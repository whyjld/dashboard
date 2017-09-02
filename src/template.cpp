#include "dashboard.h"
#include <iostream>
#include <cstring>
#include <cmath>

%classname%::%classname%(Billboard* bb)
 : m_Datas("DashBoard", true)
 , m_Billboard(bb)
 , m_Textures("./", "texture1.json")
 , m_Step(0)
%constructline%
{
    memset(m_Datas.get(), sizeof(Datas), 0);

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
		switch(m_Step)
		{
			case 0:
				NextStep();
				break;
%steps%
			default:
				break;
		};
	m_Billboard->End();
}

long %classname%::MicroSecond()
{
	timeval ct;
	gettimeofday(&ct, NULL);
	
	long sec  = ct.tv_sec  - m_StartTime.tv_sec;
	long usec = ct.tv_usec - m_StartTime.tv_usec;

	return sec * 1000L + long(usec / 1000.0 + 0.5);
}

void %classname%::NextStep()
{
	++m_Step;
	m_StepStart = m_Time;
}

void %classname%::DisplayInstrument()
{
    //mph
    long v = m_Datas->Speed;
    long d = v % 10;
    m_MphUnits[d].Draw();
    d = (v / 10) % 10;
    m_MphTens[d].Draw();
    d = (v / 100) % 10;
    m_MphHunds[d].Draw();
}

%implement%
