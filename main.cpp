#include "controller.h"

int main() {

#if 1
	//#define BOT_OVER_CERR
#define CERR_OVER_BOT
	Controller app;
	app.init();
	app.run();

#endif



	return 0;
}
//todo: make it separate thread
//todo check SIGNALS 104 - DOESN'T WORK!!!
// Add user id to Controller  - but the problem is that INI file is the one for all
//

//todo: check Indicator and Signal updates
//todo: Add Value ctor from Json
//todo: Add Logging at large
//todo: Manage threads - it seems like a leak with no proper interruption
//todo: Make Thread Engine a member of Controller
//todo: add a case when there is a new data for some of the labels in strategy - probably check that when ruleSignaling
//todo: check comment at 104 SIGNAL
//todo: figure out where to put trading details - ie Slipage and others RULE
//todo: check data race

