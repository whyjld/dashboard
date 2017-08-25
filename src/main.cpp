#include <iostream>
#include <cmath>
#include <signal.h>

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include "billboard.h"
#include "texrect.h"

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
	
	while (!quit)
	{
		billboard.Begin();
		billboard.End();
	}
	return 0;
}

