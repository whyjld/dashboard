#include "dashboard.h"
#include <math.h>

RenderItem::RenderItem(const TexInfo& tex, GLsizei slice, Billboard* bb)
 : m_BB(bb)
 , m_TexInfo(tex)
 , m_ItemInfo(bb->CreateItems(slice))
 , m_X(0.0f)
 , m_Y(0.0f)
 , m_Usage(new int(1))
{
}

RenderItem::RenderItem(const RenderItem& v)
 : m_BB(v.m_BB)
 , m_TexInfo(v.m_TexInfo)
 , m_ItemInfo(v.m_ItemInfo)
 , m_X(v.m_X)
 , m_Y(v.m_Y)
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

void RectItem::Draw()
{
	m_BB->SetPosition(m_X, m_Y);
	m_BB->Draw(m_ItemInfo, 4);
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
	
void ArcItem::Draw()
{
	m_BB->SetPosition(m_X, m_Y);
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
	
	m_EngineSpeed.SetPosition(cx, cy);
	m_EngineSpeed.SetArc(250.0f, 234.0f, -288.0f);
	m_EngineSpeed.SetProgress(0.5f);
	
	m_GasOil.SetPosition(cx, cy);
	m_GasOil.SetArc(203.0f, 225.0f, -90.0f);
	m_GasOil.SetProgress(0.5f);
	
	m_Temp.SetPosition(cx, cy);
	m_Temp.SetArc(205.0f, -45.0f, 90.0f);
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
	
	const auto sec = 5 * CLOCKS_PER_SEC;
	float op = std::min(1.0f, float(m_Time - m_StepStart) / sec);
	float tp = std::min(0.8f, float(m_Time - m_StepStart) / sec);

	m_EngineSpeed.Draw();
	
	m_GasOil.SetProgress(op);
	m_GasOil.Draw();
	
	m_Temp.SetProgress(tp);
	m_Temp.Draw();		
}

