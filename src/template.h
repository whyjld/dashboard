#ifndef _%headername%_h_
#define _%headername%_h_

#include "billboard.h"
#include "texrect.h"
#include "renderitem.h"
#include "shareMemory.h"
#include "datas.h"
#include <vector>
#include <sys/time.h>

class %classname%
{
public:
    enum Direction
    {
        dStop,
        dForward,
        dBackward,
    };
	%classname%(Billboard* bb);
	%classname%(const %classname%&) = delete;
	~%classname%();
	
	void Draw();
private:
    long StepTime(long end);
	long MicroSecond();
	void PreviousStep();
	void NextStep();

    void DisplayMph(long v);
    void DisplayInstrument();
%defines%

	ShareMemory<Datas> m_DestDatas;
	Datas m_CurrDatas;
	float m_MphProg;
	
	Billboard* m_Billboard;
	TexRect m_Textures;
    	
	timeval m_StartTime;
	
	long m_StepStart;
	long m_Time;
	long m_Second;
	
	Direction m_Dir;
	size_t m_Step;

%items%
};
#endif//_%headername%_h_
