/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/* The startup dialog lets the user do one of three things. The user
 * can open the default safe, browse for one to open, or create a
 * new safe.
 * There's a drop down box that lets the user choose what they want
 * to do. The default is to open the default safe. If a safe has been
 * specified on the command line, the default will be set to browse
 * and the safe to be opened is the one specified. Both Open default
 * and Browse will only show one line to get the pass-phrase and the
 * filename that's going to be opened. The Create New option will
 * display two lines to get a new pass-phrase, and will only allow the
 * Ok button to be clicked if both lines match.
 * Selecting Browse will automatically open up a file dialog to
 * allow the user to select the safe they want to open.
 * Canceling the startup dialog quits the program.
 */
#include <qmessagebox.h>
#include "config.h"
#include "mypasswordsafe.h"
#include "encryptedstring.hpp"
#include "myutil.hpp"

void StartupDlgBase::init()
{
	QString version = versionLabel->text();
	versionLabel->setText(version.arg(COMP_DATE).arg(COMP_HOST));
}

void StartupDlgBase::okClicked()
{
  switch(getActionBoxItem()) {
  case 0:
  case 1:
    // FIXME: use the right filter to open the file
    assert(m_myps != NULL);
    if(m_myps->open(m_filename, EncryptedString(getPassPhrase().utf8())) == false) {
      QMessageBox::information(this, tr("MyPasswordSafe"),
			       tr("Unable to open %1").arg(m_filename),
			       QMessageBox::Ok);
      return;
    }
    break;
  case 2:
    // check to see if the passwords match
    if(getPassPhrase() == QString::null) {
      QMessageBox::information(this, tr("Create new safe"),
			       tr("Please double check your pass-phrase."),
			       QMessageBox::Ok);
      return;
    }
    else {
      m_myps->createNewSafe(EncryptedString(getPassPhrase().utf8()));
    }
    break;
  default:
    QMessageBox::information(this, tr("Error"),
			     tr("An unknown action was selected."),
			     QMessageBox::Yes);
    return;
  }
  
  accept();
}


void StartupDlgBase::actionChanged( int action, bool browse)
{
	DBGOUT("action" << action);
	if(action == 2) { // create new selected
		passPhraseStack->raiseWidget(1);
	}
	else {
		assert(m_myps != NULL);
		if(action == 1) { // browse selected
		  if(browse) {
		    // display open safe dialog
		    QString filename, filter;
		    if(m_myps->browseForSafe(filename, filter, false) == false) { // canceled
		      return;
		    }
		    // update the file name
		    setFilename(filename);
		    setFilter(filter);
		  }
		  else {
		    setFilename(getFilename());
		  }
		}
		else {
			if(m_myps->getDefaultSafe().isEmpty() != true) {
				setFilename(m_myps->getDefaultSafe());
				setFilter(QString::null);
			}
			else {
				actionChanged(1); // set the action to browse
			}
		}
		passPhraseStack->raiseWidget(0);
	}
}


void StartupDlgBase::setFilename( const QString &filename )
{
	m_filename = filename;
	openLabel->setText(tr("What's the pass-phrase for %1?").arg(m_filename));
}


const QString &StartupDlgBase::getFilter()
{
	return m_filter;
}


void StartupDlgBase::setFilter(const QString &filter)
{
	m_filter = filter;
}

const QString &StartupDlgBase::getFilename()
{
	return m_filename;
}


QString StartupDlgBase::getPassPhrase()
{
	if(getActionBoxItem() == 2) { // create new selected
		if(newPassPhraseBox->text() == verifyBox->text())
			return newPassPhraseBox->text();
		else
			return QString::null;
	}
	else {
		return passPhraseBox->text();
	}
}


int StartupDlgBase::getActionBoxItem()
{
	return actionBox->currentItem();
}


void StartupDlgBase::setActionBoxItem(int item)
{
	actionBox->setCurrentItem(item);
}

void StartupDlgBase::setMyPasswordSafe(MyPasswordSafe *myps)
{
	m_myps = myps;
	if(getActionBoxItem() == 0) {
		setFilename(m_myps->getDefaultSafe());
	}
}


void StartupDlgBase::actionChanged( int action )
{
	actionChanged(action, true);
}
