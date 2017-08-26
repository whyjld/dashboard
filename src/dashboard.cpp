#include "dashboard.h"

RectItem::RectItem(const TexInfo& tex, Billboard* bb)
 : m_BB(bb)
 , m_TexInfo(tex)
 , m_ItemInfo(bb->CreateItems(4))
{
	SetAttribute(0.0f, 0.0f);
}

RectItem::~RectItem()
{
	glDeleteVertexArrays(1, &m_ItemInfo.VAO);
	glDeleteBuffers(1, &m_ItemInfo.PT);
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
	buffer[0 * 4 + 0] = x;
	buffer[0 * 4 + 1] = y + m_TexInfo.Height;
	buffer[0 * 4 + 2] = m_TexInfo.Left;
	buffer[0 * 4 + 3] = m_TexInfo.Top;

	buffer[1 * 4 + 0] = x;
	buffer[1 * 4 + 1] = y;
	buffer[1 * 4 + 2] = m_TexInfo.Left;
	buffer[1 * 4 + 3] = m_TexInfo.Bottom;

	buffer[2 * 4 + 0] = x + m_TexInfo.Width;
	buffer[2 * 4 + 1] = y + m_TexInfo.Height;
	buffer[2 * 4 + 2] = m_TexInfo.Right;
	buffer[2 * 4 + 3] = m_TexInfo.Top;

	buffer[3 * 4 + 0] = x + m_TexInfo.Width;
	buffer[3 * 4 + 1] = y;
	buffer[2 * 4 + 2] = m_TexInfo.Right;
	buffer[2 * 4 + 3] = m_TexInfo.Bottom;
	
	m_BB->SetItemsAttribute(m_ItemInfo, buffer, 4);	
}
