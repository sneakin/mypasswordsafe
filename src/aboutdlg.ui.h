/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <qstring.h>
#include "config.h"

void AboutDlg::init()
{
	QString text(infoText->text());
	text.replace("{VERSION}", VERSION);
	text.replace("{DATE}", COMP_DATE);
	text.replace("{USER}", COMP_USER);
	text.replace("{HOST}", COMP_HOST);
	text.replace("{DATE}", COMP_DATE);
	infoText->setText(text);
}

void AboutDlg::setCurrentPage(int page)
{
	tabWidget2->setCurrentPage(page);
}

int AboutDlg::currentPage() const
{
	return tabWidget2->currentPageIndex();
}
