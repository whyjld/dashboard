#ifndef _DASHBOARD_h_
#define _DASHBOARD_h_

#include "billboard.h"
#include "texrect.h"

class RectItem
{
public:
	RectItem(const TexInfo& tex, Billboard* bb);
	RectItem(const RectItem&) = delete;
	~RectItem();
	
	void SetPosition(float x, float y);
	
	void Draw();
private:
	void SetAttribute(float x, float y);
	
	Billboard* m_BB;

	TexInfo m_TexInfo;
	ItemInfo m_ItemInfo;
};

#endif//_DASHBOARD_h_