#include "SystemClass.h"

int main(int argc, char* argv[])
{
	endpoint_t ep;
	char name[256];
	int priv_f;

	// Initialize Service
	sef_startup();

	// Fetch our endpoint
	sys_whoami(&ep, name, 256, &priv_f);

	// Enable IO-sensitive operations for ourselves
	sys_enable_iop(ep);

	// Initialize program
	SystemClass* System;
	System = newSystem();
	if(SystemInitialize(System, true))
		printf("\n\t System Successfully initialized! \n");

	// Run program
	SystemRun(System);

	// Shutdown
	SystemShutdown(System);
	deleteSystem(System);

	return 0;
}
