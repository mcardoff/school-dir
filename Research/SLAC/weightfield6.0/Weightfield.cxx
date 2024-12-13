#include "WFGUI.h"

int main(int argc, char **argv)
{	
	TApplication *theApp = new TApplication("App", &argc, argv);
	new WFGUI(gClient->GetRoot(), 1000, 1000, theApp);
	theApp->Run();

	delete theApp;
		
	return 0;
}

