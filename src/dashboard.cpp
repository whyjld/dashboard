#include "dashboard.h"
#include <math.h>

RenderItem::RenderItem(const TexInfo& tex, GLsizei slice, Billboard* bb)
 : m_BB(bb)
 , m_TexInfo(tex)
 , m_ItemInfo(bb->CreateItems(slice))
 , m_Usage(new int(1))
{
}

RenderItem::RenderItem(const RenderItem& v)
 : m_BB(v.m_BB)
 , m_TexInfo(v.m_TexInfo)
 , m_ItemInfo(v.m_ItemInfo)
 , m_Usage(v.m_Usage)
{
	++(*m_Usage);
}

RenderItem::~RenderItem()
{
	Release();
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

RectItem::RectItem(const TexInfo& tex, Billboard* bb)
 : RenderItem(tex, 4, bb)
{
	SetAttribute(0.0f, 0.0f);
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
		m_BB = v.m_BB;
		m_Usage = v.m_Usage;
		m_TexInfo = v.m_TexInfo;
		m_ItemInfo = v.m_ItemInfo;
		++(*m_Usage);
	}
	return *this;
}
	
void RectItem::SetPosition(float x, float y)
{
	SetAttribute(x, y);
}

void RectItem::Draw()
{
	m_BB->Draw(m_ItemInfo, 4);
}

void RectItem::SetAttribute(float x, float y)
{
	float buffer[4 * 4];
	buffer[0 * 4 + 0] = x - m_TexInfo.Width / 2;
	buffer[0 * 4 + 1] = y + m_TexInfo.Height / 2;
	buffer[0 * 4 + 2] = m_TexInfo.Left;
	buffer[0 * 4 + 3] = m_TexInfo.Bottom;

	buffer[1 * 4 + 0] = x - m_TexInfo.Width / 2;
	buffer[1 * 4 + 1] = y - m_TexInfo.Height / 2;
	buffer[1 * 4 + 2] = m_TexInfo.Left;
	buffer[1 * 4 + 3] = m_TexInfo.Top;

	buffer[2 * 4 + 0] = x + m_TexInfo.Width / 2;
	buffer[2 * 4 + 1] = y + m_TexInfo.Height / 2;
	buffer[2 * 4 + 2] = m_TexInfo.Right;
	buffer[2 * 4 + 3] = m_TexInfo.Bottom;

	buffer[3 * 4 + 0] = x + m_TexInfo.Width / 2;
	buffer[3 * 4 + 1] = y - m_TexInfo.Height / 2;
	buffer[3 * 4 + 2] = m_TexInfo.Right;
	buffer[3 * 4 + 3] = m_TexInfo.Top;
	
	m_BB->SetItemsAttribute(m_ItemInfo, buffer, 4);	
}

ArcItem::ArcItem(const TexInfo& tex, GLsizei slice, Billboard* bb)
 : RenderItem(tex, slice * 2, bb)
 , m_X(0.0f)
 , m_Y(0.0f)
 , m_R(0.0f)
 , m_B(0.0f)
 , m_A(0.0f)
 , m_P(0.0f)
{
}

ArcItem::ArcItem(const ArcItem& v)
 : RenderItem(v)
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
		m_BB = v.m_BB;
		m_Usage = v.m_Usage;
		m_TexInfo = v.m_TexInfo;
		m_ItemInfo = v.m_ItemInfo;
		++(*m_Usage);
	}
	return *this;
}
	
void ArcItem::SetArc(float x, float y, float radius, float begin, float radian)
{
	m_X = x;
	m_Y = y; 
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
	
void ArcItem::Draw()
{
	m_BB->Draw(m_ItemInfo, m_ItemInfo.Count);
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
		is[0] = c * r + m_X;
		is[1] = s * r + m_Y;
		is[2] = m_TexInfo.Right;
		is[3] = v;
		
		r = m_R + m_TexInfo.Width / 2;
		os[0] = c * r + m_X;
		os[1] = s * r + m_Y;
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
 , m_Background(m_Textures.GetTexRect("background"), m_Billboard)
 , m_Center(m_Textures.GetTexRect("center"), m_Billboard)
 , m_Mph(m_Textures.GetTexRect("mph"), m_Billboard)
 , m_EngineSpeed(m_Textures.GetTexRect("enginespeed"), 64, m_Billboard)
 , m_GasOil(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)
 , m_Temp(m_Textures.GetTexRect("tempoil"), 32, m_Billboard)
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
			m_MphNums.push_back(RectItem(m_Textures.GetTexRect(name.c_str()), m_Billboard));
			m_MphNums[d * 10 + i].SetPosition(x, cy + 20.0f);
		}
		x += 50.0f;
	}
	
	m_EngineSpeed.SetArc(cx, cy, 250.0f, 234.0f, -288.0f);
	m_EngineSpeed.SetProgress(0.5f);
	
	m_GasOil.SetArc(cx, cy, 203.0f, 225.0f, -90.0f);
	m_GasOil.SetProgress(0.5f);
	
	m_Temp.SetArc(cx, cy, 205.0f, -45.0f, 90.0f);
	m_Temp.SetProgress(0.5f);
}

DashBoard::~DashBoard()
{
}

void DashBoard::Draw()
{
	m_Time = clock();
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
			default:
				break;
		};
	m_Billboard->End();
}

void DashBoard::NextStep()
{
	++m_Step;
	m_StepStart = m_Time;
}

void DashBoard::Step1()
{
	const auto sec = 1 * CLOCKS_PER_SEC;
	if(m_Time - m_StepStart > sec)
	{
		++m_Step;
	}
	float a = std::min(1.0f, float(m_Time - m_StepStart) / sec);
	glEnable(GL_BLEND);
	m_Billboard->SetAlpha(a);
	m_Center.Draw();
	m_Mph.Draw();
	m_MphNums[0].Draw();
	m_MphNums[10].Draw();		
}

void DashBoard::Step2()
{
	glEnable(GL_BLEND);
	m_Billboard->SetAlpha(1.0f);
	m_Center.Draw();
	m_Mph.Draw();
	m_MphNums[0].Draw();
	m_MphNums[10].Draw();
	m_EngineSpeed.Draw();
	m_GasOil.Draw();
	m_Temp.Draw();		
}

