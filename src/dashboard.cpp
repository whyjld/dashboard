#include "dashboard.h"

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
	m_BB->Draw(m_ItemInfo, 1);
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



DashBoard::DashBoard(Billboard* bb)
 : m_Billboard(bb)
 , m_Textures("./", "texture1.json")
 , m_Background(m_Textures.GetTexRect("background"), m_Billboard)
 , m_Center(m_Textures.GetTexRect("center"), m_Billboard)
 , m_Mph(m_Textures.GetTexRect("mph"), m_Billboard)
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
}

