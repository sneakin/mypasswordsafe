#ifndef STARTUPDLG_HPP
#define STARTUPDLG_HPP

#include "startupdlgbase.h"

class MyPasswordSafe;

class StartupDlg: public StartupDlgBase
{
	Q_OBJECT;
	
	public:
	enum Action { OpenDefault, Browse, CreateNew };
	
	StartupDlg(MyPasswordSafe *myps);
	
	Action getAction();
	void setAction(Action action);
};

#endif
