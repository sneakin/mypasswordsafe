/* $Header: /home/cvsroot/MyPasswordSafe/src/startupdlgbase.ui.h,v 1.6 2004/11/01 23:42:28 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
 * http://www.semanticgap.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
#include <assert.h>
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
    newPassPhraseBox->setFocus();
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
    else { // open default selected
      if(m_myps->getDefaultSafe().isEmpty() != true) {
	setFilename(m_myps->getDefaultSafe());
	setFilter(QString::null);
      }
      else {
	actionChanged(1); // set the action to browse
	m_myps->setDefaultSafe(getFilename());
      }
    }
    passPhraseStack->raiseWidget(0);
    passPhraseBox->setFocus();
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
