#include "dashboard.h"
#include <iostream>
#include <math.h>

RenderItem::RenderItem(const TexInfo& tex, GLsizei slice, Billboard* bb)
 : m_BB(bb)
 , m_TexInfo(tex)
 , m_ItemInfo(bb->CreateItems(slice))
 , m_X(0.0f)
 , m_Y(0.0f)
 , m_Scale(1.0f)
 , m_Usage(new int(1))
{
}

RenderItem::RenderItem(const RenderItem& v)
 : m_BB(v.m_BB)
 , m_TexInfo(v.m_TexInfo)
 , m_ItemInfo(v.m_ItemInfo)
 , m_X(v.m_X)
 , m_Y(v.m_Y)
 , m_Scale(v.m_Scale)
 , m_Usage(v.m_Usage)
{
	++(*m_Usage);
}

RenderItem::~RenderItem()
{
	Release();
}

void RenderItem::SetPosition(float x, float y)
{
	m_X = x;
	m_Y = y;
}

void RenderItem::SetScale(float scale)
{
	m_Scale = scale;
}
	
void RenderItem::Draw()
{
	m_BB->SetTranslation(m_X, m_Y, m_Scale);
	m_BB->Draw(m_ItemInfo, m_ItemInfo.Count);
}

void RenderItem::Release()
{
	if(--(*m_Usage) == 0)
	{
		delete m_Usage;
		glDeleteVertexArrays(1, &m_ItemInfo.VAO);
		glDeleteBuffers(1, &m_ItemInfo.PT);
	}
}

void RenderItem::Dump(const RenderItem& v)
{
	m_BB = v.m_BB;
	m_Usage = v.m_Usage;
	m_TexInfo = v.m_TexInfo;
	m_ItemInfo = v.m_ItemInfo;
	m_X = v.m_X;
	m_Y = v.m_Y;
	m_Scale = v.m_Scale;
	++(*m_Usage);
}

RectItem::RectItem(const TexInfo& tex, uint32_t mode, Billboard* bb)
 : RenderItem(tex, 4, bb)
{
	SetAttribute(mode);
}

RectItem::RectItem(const RectItem& v)
 : RenderItem(v)
{
}

RectItem::~RectItem()
{
}

RectItem& RectItem::operator=(const RectItem& v)
{
	if(this != &v)
	{
		Release();
		Dump(v);
	}
	return *this;
}

void RectItem::SetAttribute(uint32_t mode)
{
	float left, right, bottom, top;
	
	if(0 == (mode & fmVertical))
	{
		top = m_TexInfo.Top;
		bottom = m_TexInfo.Bottom;
	}
	else
	{
		top = m_TexInfo.Bottom;
		bottom = m_TexInfo.Top;
	}
	
	if(0 == (mode & fmHorizonal))
	{
		left = m_TexInfo.Left;
		right = m_TexInfo.Right;
	}
	else
	{
		left = m_TexInfo.Right;
		right = m_TexInfo.Left;
	}
	
	float buffer[4 * 4];
	buffer[0 * 4 + 0] = -m_TexInfo.Width / 2;
	buffer[0 * 4 + 1] =  m_TexInfo.Height / 2;
	buffer[0 * 4 + 2] =  left;
	buffer[0 * 4 + 3] =  bottom;

	buffer[1 * 4 + 0] = -m_TexInfo.Width / 2;
	buffer[1 * 4 + 1] = -m_TexInfo.Height / 2;
	buffer[1 * 4 + 2] =  left;
	buffer[1 * 4 + 3] =  top;

	buffer[2 * 4 + 0] =  m_TexInfo.Width / 2;
	buffer[2 * 4 + 1] =  m_TexInfo.Height / 2;
	buffer[2 * 4 + 2] =  right;
	buffer[2 * 4 + 3] =  bottom;

	buffer[3 * 4 + 0] =  m_TexInfo.Width / 2;
	buffer[3 * 4 + 1] = -m_TexInfo.Height / 2;
	buffer[3 * 4 + 2] =  right;
	buffer[3 * 4 + 3] =  top;
	
	m_BB->SetItemsAttribute(m_ItemInfo, buffer, 4);	
}

ArcItem::ArcItem(const TexInfo& tex, GLsizei slice, Billboard* bb)
 : RenderItem(tex, slice * 2, bb)
 , m_R(0.0f)
 , m_B(0.0f)
 , m_A(0.0f)
 , m_P(0.0f)
{
}

ArcItem::ArcItem(const ArcItem& v)
 : RenderItem(v)
 , m_R(v.m_R)
 , m_B(v.m_B)
 , m_A(v.m_A)
 , m_P(v.m_P)
{
}

ArcItem::~ArcItem()
{
}
	
ArcItem& ArcItem::operator=(const ArcItem& v)
{
	if(this != &v)
	{
		Release();
		m_R = v.m_R;
		m_B = v.m_B;
		m_A = v.m_A;
		m_P = v.m_P;
		Dump(v);
	}
	return *this;
}
	
void ArcItem::SetArc(float radius, float begin, float radian)
{
	m_R = radius;
	m_B = begin / 180.0f * M_PI;
	m_A = radian / 180.0f * M_PI;
	
	SetAttribute();
}

void ArcItem::SetProgress(float p)
{
	m_P = p;
	
	SetAttribute();
}

void ArcItem::SetAttribute()
{
	std::vector<float> buffer(4 * m_ItemInfo.Count);
	
	float* is;
	float* os;
	if(m_A > 0)
	{
		is = &buffer[0];
		os = &buffer[4];
	}
	else
	{
		is = &buffer[4];
		os = &buffer[0];
	}
	
	GLsizei c = m_ItemInfo.Count / 2 - 1;
	float s = m_A * m_P / c;
	float t = (m_TexInfo.Top - m_TexInfo.Bottom) * m_P / c;
	for(GLsizei i = 0;i < c + 1;++i)
	{
		float a = m_B + s * i;
		float v = m_TexInfo.Bottom + t * i;
		float c = cos(a);
		float s = sin(a);
		
		float r = m_R - m_TexInfo.Width / 2;
		is[0] = c * r;
		is[1] = s * r;
		is[2] = m_TexInfo.Right;
		is[3] = v;
		
		r = m_R + m_TexInfo.Width / 2;
		os[0] = c * r;
		os[1] = s * r;
		os[2] = m_TexInfo.Left;
		os[3] = v;
	
		is += 4 * 2;
		os += 4 * 2;
	}
	
	m_BB->SetItemsAttribute(m_ItemInfo, &buffer[0], m_ItemInfo.Count);	
}

DashBoard::DashBoard(Billboard* bb)
 : m_Billboard(bb)
 , m_Textures("./", "texture1.json")
 , m_Background(m_Textures.GetTexRect("background"), 0, m_Billboard)
 , m_Center(m_Textures.GetTexRect("center"), 0, m_Billboard)
 , m_Mph(m_Textures.GetTexRect("mph"), 0, m_Billboard)
 , m_EngineSpeed(m_Textures.GetTexRect("enginespeed"), 64, m_Billboard)
 , m_GasOil(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)
 , m_Temp(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)
 , m_LeftOutter(m_Textures.GetTexRect("outter"), 0, m_Billboard)
 , m_RightOutter(m_Textures.GetTexRect("outter"), fmHorizonal, m_Billboard)
 , m_LeftSide(m_Textures.GetTexRect("side"), 0, m_Billboard)
 , m_RightSide(m_Textures.GetTexRect("side"), fmHorizonal, m_Billboard)
 , m_ECOSide(m_Textures.GetTexRect("eco"), 0, m_Billboard)
 , m_ECOGAUGE(m_Textures.GetTexRect("ECO GAUGE"), 0, m_Billboard)
 , m_MAINMENU(m_Textures.GetTexRect("MAIN MENU"), 0, m_Billboard)
 , m_TripComputer(m_Textures.GetTexRect("Trip Computer"), 0, m_Billboard)
 , m_Assistance(m_Textures.GetTexRect("Assistance"), 0, m_Billboard)
 , m_EcoStatistics(m_Textures.GetTexRect("Eco Statistics"), 0, m_Billboard)
 , m_SkinSettings(m_Textures.GetTexRect("Skin Settings"), 0, m_Billboard)
 , m_Service(m_Textures.GetTexRect("Service"), 0, m_Billboard)
 , m_Step(0)
{
	m_Billboard->SetLogicSize(1920, 720);

	float cx = 960.0f;
	float cy = 360.0f;
	m_Background.SetPosition(cx, cy);
	m_Center.SetPosition(cx, cy);
	m_Mph.SetPosition(cx, cy - 30.0f);

	float x = cx - 25.0f;
	for(int d = 0;d < 2;++d)
	{
		for(int i = 0;i < 10;++i)
		{
			std::string name = "mph";
			name += ('0' + i);
			m_MphNums.push_back(RectItem(m_Textures.GetTexRect(name.c_str()), 0, m_Billboard));
			m_MphNums[d * 10 + i].SetPosition(x, cy + 20.0f);
		}
		x += 50.0f;
	}
	
	for(int d = 0;d < 4;++d)
	{
		m_Ends.push_back(RectItem(m_Textures.GetTexRect("end"), d, m_Billboard));
	}
	m_Ends[0].SetPosition(cx - 175.0f, cy + 245.0f);
	m_Ends[1].SetPosition(cx + 175.0f, cy + 245.0f);
	m_Ends[2].SetPosition(cx - 175.0f, cy - 245.0f);
	m_Ends[3].SetPosition(cx + 175.0f, cy - 245.0f);
	
	m_EngineSpeed.SetPosition(cx, cy);
	m_EngineSpeed.SetArc(250.0f, 234.0f, -288.0f);
	m_EngineSpeed.SetProgress(0.5f);
	
	m_GasOil.SetPosition(cx, cy);
	m_GasOil.SetArc(203.0f, 225.0f, -90.0f);
	m_GasOil.SetProgress(0.5f);
	
	m_Temp.SetPosition(cx, cy);
	m_Temp.SetArc(205.0f, -45.0f, 90.0f);
	m_Temp.SetProgress(0.5f);
	
	m_LeftOutter.SetScale(0.60f);
	m_LeftOutter.SetPosition(700.0f, cy);
	m_RightOutter.SetScale(0.60f);
	m_RightOutter.SetPosition(1220.0f, cy);
	
	m_LeftSide.SetPosition(-m_LeftSide.GetTexInfo().Width / 2, cy);
	m_RightSide.SetPosition(1920.0f + m_LeftSide.GetTexInfo().Width / 2, cy);

	m_ECOGAUGE.SetPosition(1360.0f, 580.0f);
	m_MAINMENU.SetPosition(560.0f, 580.0f);
	m_TripComputer.SetPosition(420.0f, 520.0f);
	m_Assistance.SetPosition(350.0f, 440.0f);
	m_EcoStatistics.SetPosition(360.0f, 360.0f);
	m_SkinSettings.SetPosition(370.0f, 280.0f);
	m_Service.SetPosition(370.0f, 200.0f);

	gettimeofday(&m_StartTime, NULL);
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
		glDisable(GL_BLEND);
		m_Background.Draw();
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
	gettimeofday(&m_CurrentTime, NULL);
	
	long sec  = m_CurrentTime.tv_sec  - m_StartTime.tv_sec;
    long usec = m_CurrentTime.tv_usec - m_StartTime.tv_usec;

	return sec * 1000L + long(usec / 1000.0 + 0.5);
}

void DashBoard::NextStep()
{
	++m_Step;
	m_StepStart = m_Time;
}

void DashBoard::Step1()
{
	const long sec = 1000;
	float a = std::min(1.0f, float(m_Time - m_StepStart) / sec);
	glEnable(GL_BLEND);
	m_Billboard->SetAlpha(a);
	m_Center.Draw();
	m_Mph.Draw();
	m_MphNums[0].Draw();
	m_MphNums[10].Draw();		

	m_Ends[0].Draw();
	m_Ends[1].Draw();
	m_Ends[2].Draw();
	m_Ends[3].Draw();

	m_LeftOutter.Draw();
	m_RightOutter.Draw();

	if(m_Time - m_StepStart > sec)
	{
		NextStep();
	}
}

void DashBoard::Step2()
{
	glEnable(GL_BLEND);
	m_Billboard->SetAlpha(1.0f);
	m_Center.Draw();
	m_Mph.Draw();
	m_MphNums[0].Draw();
	m_MphNums[10].Draw();

	m_Ends[0].Draw();
	m_Ends[1].Draw();
	m_Ends[2].Draw();
	m_Ends[3].Draw();

	m_EngineSpeed.Draw();

	{
		const long sec = 2000;
		float p = std::min(1.0f, float(m_Time - m_StepStart) / sec);

		m_LeftOutter.SetScale(0.60f + p * 0.40f);
		m_LeftOutter.SetPosition(700.0f - p * 480.0f, 360.0f);

		m_RightOutter.SetScale(0.60f + p * 0.40f);
		m_RightOutter.SetPosition(1220.0f + p * 480.0f, 360.0f);

		m_LeftOutter.Draw();
		m_RightOutter.Draw();
	}
	
	{
		const long sec = 2000;
		float op = std::min(1.0f, float(m_Time - m_StepStart) / sec);
		float tp = std::min(0.8f, float(m_Time - m_StepStart) / sec);
	
		m_GasOil.SetProgress(op);
		m_Temp.SetProgress(tp);
	
		m_GasOil.Draw();
		m_Temp.Draw();
	}
	
	if(m_Time - m_StepStart > 2000)
	{
		NextStep();
	}
}

void DashBoard::Step3()
{
	glEnable(GL_BLEND);
	m_Billboard->SetAlpha(1.0f);
	m_Center.Draw();
	m_Mph.Draw();
	m_MphNums[0].Draw();
	m_MphNums[10].Draw();

	m_LeftOutter.Draw();
	m_RightOutter.Draw();
	
	m_EngineSpeed.Draw();

	m_Ends[0].Draw();
	m_Ends[1].Draw();
	m_Ends[2].Draw();
	m_Ends[3].Draw();
	
	m_GasOil.SetProgress(1.0);
	m_GasOil.Draw();
	
	m_Temp.SetProgress(0.8);
	m_Temp.Draw();
	
	const long ss = 2000;
	float sp = std::min(1.0f, float(m_Time - m_StepStart) / ss);

	float y = 360.0f;
	float ls = -m_LeftSide.GetTexInfo().Width / 2;
	float rs = 1920.0f + m_LeftSide.GetTexInfo().Width / 2;
	float dis = m_LeftSide.GetTexInfo().Width + 180.0f;
	m_LeftSide.SetPosition(ls + dis * sp, y);
	m_RightSide.SetPosition(rs - dis * sp, y);
	
	m_LeftSide.Draw();
	m_RightSide.Draw();
	
	if(m_Time - m_StepStart > ss)
	{
		NextStep();
		m_ECOSide.SetPosition(m_RightSide.GetX(), m_RightSide.GetY());
	}
}

void DashBoard::Step4()
{
	glEnable(GL_BLEND);
	m_Billboard->SetAlpha(1.0f);
	m_Center.Draw();
	m_Mph.Draw();
	m_MphNums[0].Draw();
	m_MphNums[10].Draw();

	m_LeftOutter.Draw();
	m_RightOutter.Draw();
	
	m_EngineSpeed.Draw();

	m_Ends[0].Draw();
	m_Ends[1].Draw();
	m_Ends[2].Draw();
	m_Ends[3].Draw();
	
	m_GasOil.SetProgress(1.0);
	m_GasOil.Draw();
	
	m_Temp.SetProgress(0.8);
	m_Temp.Draw();
		
	m_LeftSide.Draw();

	const long ss = 1000;
	float ep = std::min(1.0f, float(m_Time - m_StepStart) / ss);
	m_Billboard->SetAlpha(1.0f - ep);
	m_RightSide.Draw();
	m_Billboard->SetAlpha(ep);
	m_ECOSide.Draw();

	m_ECOGAUGE.Draw();
	m_MAINMENU.Draw();
	m_TripComputer.Draw();
	m_Assistance.Draw();
	m_EcoStatistics.Draw();
	m_SkinSettings.Draw();
	m_Service.Draw();
}

