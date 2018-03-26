#include <iostream>
#include <vector>
#include <cmath>
#include <signal.h>

#include "dashboard.h"
#include "udp.h"

volatile sig_atomic_t quit = 0;

void sighandler(int signal)
{
	std::cout << "Caught signal " << signal << ", setting flaq to quit.\n";
	quit = true;
}

void onReceive(const sockaddr_in& sa, const char* buf, size_t len)
{
	static ShareMemory<Datas> datas("DashBoard");
	
	if(strncmp(buf, "up", len) == 0)
	{
	}
	else if(strncmp(buf, "down", len) == 0)
	{
	}
	else if(strncmp(buf, "left", len) == 0)
	{
	    datas->View = -1;
	}
	else if(strncmp(buf, "right", len) == 0)
	{
	    datas->View = 1;
	}
	else if(buf == strstr(buf, "OilMass:"))
	{
	    datas->OilMass = atoi(buf + 8);
	}
	else if(buf == strstr(buf, "Speed:"))
	{
	    datas->Speed = atoi(buf + 6);
	}
	else if(buf == strstr(buf, "EngineSpeed:"))
	{
	    datas->EngineSpeed = atoi(buf + 12);
	}
	else if(buf == strstr(buf, "WaterTemperature:"))
	{
	    datas->WaterTemperature = atoi(buf + 17);
	}
	
	std::cout << buf << std::endl;	
}

int main (void)
{
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);

	Billboard billboard(256);
	DashBoard dashboard(&billboard);
	
	UDP udp;
	if(!udp.Bind(1088, &onReceive))
	{
	    std::cout << "Bind failed." << std::endl;
	}
		
	while (!quit)
	{
		dashboard.Draw();
		udp.Update();
	}
	return 0;
}

