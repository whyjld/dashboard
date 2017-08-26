#include <iostream>
#include <vector>
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
	DashBoard dashboard(&billboard);
		
	while (!quit)
	{
		dashboard.Draw();
	}
	return 0;
}

