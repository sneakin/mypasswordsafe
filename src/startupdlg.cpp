#include "startupdlg.hpp"
#include "mypasswordsafe.h"

StartupDlg::StartupDlg(MyPasswordSafe *myps)
	: StartupDlgBase(NULL)
{
	setMyPasswordSafe(myps);
}

StartupDlg::Action StartupDlg::getAction()
{
	return (Action)getActionBoxItem();
}

void StartupDlg::setAction(Action action)
{
	setActionBoxItem(action);
	actionChanged((int)action, false);
}
