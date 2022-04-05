#include "ui.h"

int main() {

//#define BOT_OVER_CERR
#define CERR_OVER_BOT
	user_interface::UI ui;
	ui.run();

	return 0;
}


//todo: check Indicator and Signal updates
//todo: Add Value ctor from Json
//todo: Add Logging at large
//todo: Manage threads - it seems like a leak with no proper interruption
//todo: add a case when there is a new data for some of the labels in strategy - probably check that when ruleSignaling
//todo: check comment at 104 SIGNAL
//todo: figure out where to put trading details - ie Slipage and others RULE
//todo: check data race

