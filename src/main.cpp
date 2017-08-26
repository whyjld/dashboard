#include <iostream>
#include <cmath>
#include <signal.h>

#include "dashboard.h"

volatile sig_atomic_t quit = 0;

void sighandler(int signal)
{
	std::cout << "Caught signal " << signal << ", setting flaq to quit.\n";
	quit = true;
}

int main (void)
{
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);

	Billboard billboard(256);
	TexRect   texrect("./", "texture1.json");
	
	RectItem item(texrect.GetTexRect("background"), &billboard);
	
	billboard.SetLogicSize(1920, 720);
	while (!quit)
	{
		billboard.Begin();
			item.Draw();		
		billboard.End();
	}
	return 0;
}

