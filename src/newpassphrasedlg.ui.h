/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <qmessagebox.h>
#include "myutil.hpp"


void NewPassPhraseDlg::okClicked()
{
	// FIXME: warn about the strength?
  QString line_one = firstLine->text(),
    line_two = secondLine->text();
	if(line_one == line_two) {
		accept();
	}
}


QString NewPassPhraseDlg::password()
{
	if(firstLine->text() == secondLine->text())
		return firstLine->text();
	else
		return QString::null;
}


void NewPassPhraseDlg::lineChanged(const QString &text)
{
	if(firstLine->text() == secondLine->text() && text.length() > 0)
		okButton->setEnabled(true);
	else
		okButton->setEnabled(false);
}
