#include <iostream>
#include <sstream>
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

long Now()
{
	timeval ct;
	gettimeofday(&ct, NULL);
	
	long sec  = ct.tv_sec;
	long usec = ct.tv_usec;

	return sec * 1000L + long(usec / 1000.0 + 0.5);
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

int main (int argc, char* argv[])
{
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);

	Billboard billboard(256);
	DashBoard dashboard(&billboard);

	int port = 0;	
	UDP udp;
	if(argc > 1)
	{
		port = atoi(argv[1]);
		if(port > 0 && !udp.Bind(port, &onReceive))
		{
		    std::cout << "Bind failed." << std::endl;
		}
	}
	else
	{
		std::cout << "Usage:" << argv[0] << " port" << std::endl;
	}
	
	sockaddr_in none;
	std::ostringstream oss;
	if(!(port > 0))
	{
	    oss << "OilMass" << 80;
		onReceive(none, oss.str().c_str(), oss.str().length());

	    oss << "WaterTemperature" << 90;
		onReceive(none, oss.str().c_str(), oss.str().length());
	}
		
	long start = Now();
	long last = start;
	int gear = 1;
	float gearRate[] =
	{
	    106.0f,
	    106.0f,
	    62.5f,
	    42.3f,
	    35.5f,
	    29.5f,
	    24.0f,
	    19.4f,
	};

	long action = 0;
	while (!quit)
	{
		dashboard.Draw();
		if(port > 0)
		{
			udp.Update();
		}
		else
		{
			long now = Now();
			if(now - last > 10000)
			{
			    oss.str("");
				last = now;
				if(action % 2 == 0)
				{
				    oss << "right";
				}
				else
				{
				    oss << "left";
				}
				++action;
				onReceive(none, oss.str().c_str(), oss.str().length());
			}

            oss.str("");			
			float speed = (sin((now - start) / 10000.0f) + 1.0f) * 60.0f + 20.0f;
			oss << "Speed:" << (int)speed;			
			onReceive(none, oss.str().c_str(), oss.str().length());
            
            float es = speed * gearRate[gear];
            if(gear > 1 && es < 1200.0f)
            {
                --gear;
                es = speed * gearRate[gear];
            }
            if(gear < 6 && es > 3400.0f)
            {
                ++gear;
                es = speed * gearRate[gear];
            }
            oss.str("");			
			oss << "EngineSpeed:" << (int)es;			
			onReceive(none, oss.str().c_str(), oss.str().length());
		}
	}
	return 0;
}

