/****************************************************************************
 ** ui.h extension file, included from the uic-generated form implementation.
 **
 ** If you wish to add, delete or rename functions or slots use
 ** Qt Designer which will update this file, preserving your code. Create an
 ** init() function in place of a constructor, and a destroy() function in
 ** place of a destructor.
 *****************************************************************************/
/* $Header: /home/cvsroot/MyPasswordSafe/src/mypasswordsafe.ui.h,v 1.9 2004/06/23 02:24:20 nolan Exp $
 * Copyright (c) 2004, Semantic Gap Solutions
 * All rights reserved.
 *   
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *  -  Redistributions of source code must retain the
 *     above copyright notice, this list of conditions
 *     and the following disclaimer.
 *  -  Redistributions in binary form must reproduce the
 *     above copyright notice, this list of conditions and
 *     the following disclaimer in the documentation and/or
 *     other materials provided with the distribution.
 *  -  Neither the name of Semantic Gap Solutions nor the
 *     names of its contributors may be used to endorse or
 *     promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qclipboard.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qsettings.h>
#include <qlistview.h>
#include "passphrasedlg.h"
#include "pwordeditdlg.h"
#include "manualdlg.h"
#include "newpassphrasedlg.h"
#include "myutil.hpp"

using namespace std;

void MyPasswordSafe::init()
{
  m_shown = false;
  savingEnabled(false);

  for(int i = 0; i < 3; i++)
    pwordListView->setColumnWidthMode(i, QListView::Manual);
    
  m_config.setPath("SemanticGap.com", "MyPasswordSafe");
  m_config.beginGroup("/MyPasswordSafe");
  m_gen_pword_length = m_config.readNumEntry("/prefs/password_length", 8);
  
  m_default_safe = m_config.readEntry("/prefs/default_safe", "");
  if(m_default_safe.isEmpty())
    fileOpenDefaultAction->setEnabled(false);
  
  m_def_user = m_config.readEntry("/prefs/default_username", "");
  setClearClipboardOnExit(m_config.readBoolEntry("/prefs/clear_clipboard", true));
  setLockOnMinimize(m_config.readBoolEntry("/prefs/lock_on_minimize", true));
  
  m_max_tries = m_config.readNumEntry("/prefs/max_tries", 3);
  if(m_max_tries > 10)
    m_max_tries = 10;
  
  int w, h;
  w = m_config.readNumEntry("/MainWindow/width", 400);
  h = m_config.readNumEntry("/MainWindow/height", 320);
  resize(QSize(w, h));
    
  w = m_config.readNumEntry("/MainWindow/left", -1);
  h = m_config.readNumEntry("/MainWindow/top", -1);
  if(w != -1 && h != -1) {
    move(w, h);
  }
   
  w = m_config.readNumEntry("/MainWindow/name_width", 0);
  if(w)
    pwordListView->setColumnWidth(0, w);
  w = m_config.readNumEntry("/MainWindow/user_width", 0);
  if(w)
    pwordListView->setColumnWidth(1, w);
  w = m_config.readNumEntry("/MainWindow/notes_width", 0);
  if(w)
    pwordListView->setColumnWidth(2, w);
  
  m_config.endGroup();
  
  m_safe = NULL;
  // m_safe gets setup by the Startup dialog
}

void MyPasswordSafe::destroy()
{
  // ask to save file if needed
  if(clearClipboardOnExit())
	  copyToClipboard("");
  // save config settings   
  m_config.beginGroup("/MyPasswordSafe");
  
  m_config.writeEntry("/prefs/password_length", (int)m_gen_pword_length);
  m_config.writeEntry("/prefs/default_safe", m_default_safe);
  m_config.writeEntry("/prefs/default_username", m_def_user);
  m_config.writeEntry("/prefs/clear_clipboard", clearClipboardOnExit());
  m_config.writeEntry("/prefs/lock_on_minimize", lockOnMinimize());
  m_config.writeEntry("/prefs/max_tries", m_max_tries);
  
  QSize sz = size();
  m_config.writeEntry("/MainWindow/width", sz.width());
  m_config.writeEntry("/MainWindow/height", sz.height());

  QPoint position = pos();
  m_config.writeEntry("/MainWindow/left", position.x());
  m_config.writeEntry("/MainWindow/top", position.y());
 
  m_config.writeEntry("/MainWindow/name_width", pwordListView->columnWidth(0));
  m_config.writeEntry("/MainWindow/user_width", pwordListView->columnWidth(1));
  m_config.writeEntry("/MainWindow/notes_width", pwordListView->columnWidth(2));
  m_config.endGroup();
  
  if(m_safe)
    delete m_safe;
}


void MyPasswordSafe::closeEvent( QCloseEvent *e )
{
  if(closeSafe()) {
    e->accept();
  }
}


bool MyPasswordSafe::closeSafe()
{
  if(m_safe) {
    if(m_safe->changed()) {
      int result = QMessageBox::warning(this, tr("Save safe?"),
					tr("Do you want to save the safe before it is closed?"),
					QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
      switch(result) {
      case QMessageBox::Yes:
	if(!save())
		return false;
	break;
      case QMessageBox::Cancel:
	return false;
      default:
	break;
      }
    }
    delete m_safe;
    m_safe = NULL;
    savingEnabled(false);
  }
  return true;
}


void MyPasswordSafe::fileNew()
{
  NewPassPhraseDlg dlg;
  // NOTE:  the dialog doesn't trash the memory
  if(dlg.exec() == QDialog::Rejected) {
    statusBar()->message(tr("No pass-phrase entered"));
    return;
  }

  createNewSafe(EncryptedString(dlg.password().utf8()));
}


void MyPasswordSafe::fileExit()
{
  //if(closeSafe()) {
  close();
  //}
}


void MyPasswordSafe::fileMakeDefault()
{
  if(m_safe->changed()) {
    if(!save()) {
      statusBar()->message(tr("The safe must be saved before it can be made the default."));
      return;
    }
  }
  m_default_safe = m_safe->getPath();
  fileOpenDefaultAction->setEnabled(true);
}


void MyPasswordSafe::filePreferences()
{
  PreferencesDlg dlg;
  dlg.setDefUser(m_def_user);
  dlg.setDefaultSafe(m_default_safe);
  dlg.setGenPwordLength(m_gen_pword_length);
  dlg.setMaxTries(m_max_tries);
  if(dlg.exec() == QDialog::Accepted) {
    m_def_user = dlg.getDefUser();
    m_default_safe = dlg.getDefaultSafe();
    m_gen_pword_length = dlg.getGenPwordLength();
    m_max_tries = dlg.getMaxTries();
	
    if(m_default_safe.isEmpty() == false)
      fileOpenDefaultAction->setEnabled(true);
    else
      fileOpenDefaultAction->setEnabled(false);
  }
}


void MyPasswordSafe::fileOpen()
{
  QString filename, filter;

  if(browseForSafe(filename, filter, false)) {
    if(!filename.isEmpty()) {
      QString file_ext = filename.right(filename.findRev('.'));
      Safe::Error open_ret = Safe::Failed;

      // check the password
      PassPhraseDlg passphrase_dlg;
      SecuredString pword;
      int num_tries = 0;
      while(num_tries < m_max_tries) {
	if(passphrase_dlg.exec() == PassPhraseDlg::Accepted) {
	  pword.set(passphrase_dlg.getText().utf8());

	  open_ret = Safe::checkPassword((const char *)filename.utf8(), (const char *)filter.utf8(), pword);

	  if(open_ret == Safe::Success)
	    break;
	  else {
	    statusBar()->message(Safe::errorToString(open_ret));
	    return;
	  }

	  num_tries++;
	}
	else {
	  statusBar()->message(tr("No pass-phrase entered"));
	  return;
	}
      }

      if(open(filename, pword, filter)) {
	statusBar()->message(tr("Safe opened"));
      }
    }
    else {
      statusBar()->message(tr("No filename specified"));
    }
  }
  else {
    statusBar()->message(tr("Open file cancelled"));
  }
}


bool MyPasswordSafe::open(const char *filename, const EncryptedString &passkey)
{
  return open(filename, passkey, NULL);
}


bool MyPasswordSafe::open( const char *filename, const EncryptedString &passkey, const char *type )
{
  Safe::Error ret = Safe::Failed;
  DBGOUT("Checking password");
  ret = Safe::checkPassword((const char *)filename,
			    (const char *)type, passkey);

  if(ret == Safe::Success) {
    // this is when we ask the user if they want to save a
    // modified safe or not
    if(!closeSafe())
      return false;
    Safe *s = new Safe;
    ret = s->load((const char *)filename,
			      (const char *)type,
			      passkey,
			      (const char *)m_def_user);
    if(ret == Safe::Success) {
      DBGOUT(filename << " has " << s->size() << " entries");

      //    if(s != NULL) {
      if(m_safe != NULL)
	delete m_safe;
      m_safe = s;
      pwordListView->setSafe(m_safe);
      //m_safe->setListBox(pwordListView);
      setCaption(tr("MyPasswordSafe: ") + filename);
      savingEnabled(false);
      fileSaveAsAction->setEnabled(true);
      return true;
    }
    else {
      DBGOUT("Error = " << Safe::errorToString(ret));
      statusBar()->message(tr("Error opening file"));
      delete s;
    }
  }
  else if(ret == Safe::Failed) {
    // FIXME: could we distinguish what the error was?
    statusBar()->message(tr("Wrong pass phrase"));
  }
  else if(ret == Safe::BadFormat) {
    statusBar()->message(tr("Wrong filter choosen"));
  }
  else {
    statusBar()->message(tr("Error opening file"));
  }

  return false;
}


bool MyPasswordSafe::save()
{
  if(m_safe != NULL) {
	QString path(m_safe->getPath());
	if(path.isEmpty())
	  return saveAs();
	else {
	  Safe::Error error = m_safe->save((const char *)m_def_user);
	  if(error == Safe::Success) {
	    savingEnabled(false);
	    statusBar()->message(tr("Safe saved"));
	    return true;
	  }
	  else {
	    statusBar()->message(Safe::errorToString(error));
	  }
	}
  }
  return false;
}


bool MyPasswordSafe::saveAs()
{
  QString filename, filter;

  if(browseForSafe(filename, filter, true)) {
    //QString filter = file_dlg.selectedFilter();
    if(!filename.isEmpty()) {
      // NOTE: the passphrase doesn't get encrypted here
      SecuredString passkey;
      NewPassPhraseDlg dlg;
      if(dlg.exec() == QDialog::Accepted) {
	passkey.set(dlg.password().utf8());
	Safe::Error error = m_safe->save((const char *)filename.utf8(),
				   (const char *)filter.utf8(),
				   passkey,
				   (const char *)m_def_user);
	if(error == Safe::Success) {
	  setCaption(tr("MyPasswordSafe: ") + m_safe->getPath());
	  statusBar()->message(tr("Safe saved"));
	  savingEnabled(false);
	  return true;
	}
	else {
	  DBGOUT("Error: " << error);
	  statusBar()->message(tr("Error saving file"));
	}
      }
      else {
	statusBar()->message(tr("No passkey entered"));
      }
    }
  }
  else {
    statusBar()->message(tr("Save file cancelled"));
  }
  return false;
}



void MyPasswordSafe::pwordAdd()
/* Shows the password edit dialog for the user to
 * fill out the values that the new entry will have,
 * and then adds the item to safe and view.
 */
{
  PwordEditDlg dlg;
  dlg.setGenPwordLength(m_gen_pword_length);
  dlg.setUser(m_def_user);
  dlg.showDetails(false);
  
  if(dlg.exec() == QDialog::Accepted) {
    SafeItem i((const char *)dlg.getItemName().utf8(),
	       (const char *)dlg.getUser().utf8(),
	       SecuredString((const char *)dlg.getPassword().utf8()),
	       (const char *)dlg.getNotes().utf8());
    SafeItem *safe_item = m_safe->addItem(i);
    if(safe_item != NULL) {
      if(pwordListView->addItem(safe_item) != NULL) {
	savingEnabled(true);
	statusBar()->message(tr("Item added"));
	return;
      }
    }

    statusBar()->message(tr("Error adding item"));
  }
  else {
    statusBar()->message(tr("Add canceled"));
  }
}


void MyPasswordSafe::pwordDelete()
{
  SafeListViewItem *list_item(pwordListView->getSelectedItem());
  if(list_item) {
    int result = QMessageBox::warning(this, tr("Are you sure?"),
				      tr("Are you sure you want to delete this password?"),
				      QMessageBox::Yes, QMessageBox::No);
    switch(result) {
    case QMessageBox::Yes:
      pwordListView->delItem(list_item);
      savingEnabled(true);
      statusBar()->message(tr("Password deleted"));
      break;
    default:
      statusBar()->message(tr("Delete cancelled"));
      break;
    }
  }
  else {
    statusBar()->message(tr("No item selected"));
  }
}


void MyPasswordSafe::pwordEdit()
{
  SafeListViewItem *item = pwordListView->getSelectedItem();
  if(item != NULL) {
    item->updateAccessTime();

    PwordEditDlg dlg;

    dlg.setGenPwordLength(m_gen_pword_length);
    dlg.setItemName(QString::fromUtf8(item->getName()));
    dlg.setUser(QString::fromUtf8(item->getUser()));
    // NOTE: password decrypted
    dlg.setPassword(QString::fromUtf8(item->getPassword().get().get()));
    dlg.setNotes(QString::fromUtf8(item->getNotes()));
    dlg.setAccessedOn(item->getAccessTime());
    dlg.setCreatedOn(item->getCreationTime());
    dlg.setModifiedOn(item->getModificationTime());
    dlg.setLifetime(item->getLifetime());
    dlg.setUUID(item->getUUID()); // FIXME: decouple

    if(dlg.exec() == QDialog::Accepted) {
      item->setName(dlg.getItemName());
      item->setUser(dlg.getUser());
      item->setPassword(EncryptedString((const char *)dlg.getPassword().utf8()));
      item->setNotes(dlg.getNotes());
      item->updateModTime();

      statusBar()->message(tr("Password updated"));
    }
    else {
      statusBar()->message(tr("Edit password cancelled"));
    }

    m_safe->setChanged(true); // FIXME: send this through the view
    savingEnabled(true);
  }
  else {
    statusBar()->message(tr("No item selected"));
  }
}


void MyPasswordSafe::pwordFetch()
{
  SafeListViewItem *item(pwordListView->getSelectedItem());
  if(item) {
    // NOTE: password decrypted
    SecuredString pword(item->getPassword().get());
	copyToClipboard(QString::fromUtf8(pword.get()));
    statusBar()->message(tr("Password copied to clipboard"));

    item->updateAccessTime();
  }
  else {
    statusBar()->message(tr("No item selected"));
  }
}


void MyPasswordSafe::pwordFetchUser()
{
  SafeListViewItem *item(pwordListView->getSelectedItem());
  if(item) {
	copyToClipboard(QString::fromUtf8(item->getUser()));
    statusBar()->message(tr("Username copied to clipboard"));

    item->updateAccessTime();
  }
  else {
    statusBar()->message(tr("No item selected"));
  }
}


void MyPasswordSafe::onPwordListDblClicked(QListViewItem *item)
{
  if(item->rtti() == SafeListViewItem::RTTI)
    pwordFetch();
}


void MyPasswordSafe::onPwordListRightClk( QListViewItem *, const QPoint &point, int)
{
  PopupMenu->popup(point);
}


void MyPasswordSafe::helpAbout()
{
  AboutDlg dlg;
  dlg.exec();
}


void MyPasswordSafe::helpManual()
{
  if(m_manual.isVisible()) {
    m_manual.hide();
  }
  else {
    m_manual.show();
  }
}


Safe * MyPasswordSafe::getSafe()
{
  return m_safe;
}


int MyPasswordSafe::getGeneratedPwordLength()
{
  return m_gen_pword_length;
}


void MyPasswordSafe::savingEnabled( bool value)
{
  if(value) {
    fileSaveAction->setEnabled(true);
    //fileSaveAsAction->setEnabled(true);
  }
  else {
    fileSaveAction->setEnabled(false);
  }
}


const QString & MyPasswordSafe::getDefaultSafe()
{
  return m_default_safe;
}


bool MyPasswordSafe::browseForSafe( QString &filename, QString &filter, bool saving )
{
  QString all_safes(tr("All Safes (%1)").arg(Safe::getExtensions().c_str()));
  QString all_files(tr("All Files (*)"));

  QString types(QString("%1\n%2\n%3").arg(all_safes).arg(Safe::getTypes().c_str()).arg(all_files));

  QFileDialog file_dlg(QString::null, types, this, "file",
		       true);
  if(saving)
    file_dlg.setMode(QFileDialog::AnyFile);

  if(file_dlg.exec() == QDialog::Accepted) {
    filename = file_dlg.selectedFile();

    QString temp(file_dlg.selectedFilter());
    if(temp != all_safes && temp != all_files)
      filter = file_dlg.selectedFilter();
    return true;
  }

  return false;
}


void MyPasswordSafe::createNewSafe(const EncryptedString &passphrase)
{
  if(closeSafe()) {
    m_safe = new Safe();
    m_safe->setPassPhrase(passphrase);
    //m_safe->setListBox(pwordListView);
    pwordListView->setSafe(m_safe);
    setCaption(tr("MyPasswordSafe: <untitled>"));
    savingEnabled(false);
    fileSaveAsAction->setEnabled(false);
    statusBar()->message(tr("Created new safe"));
  }
}


void MyPasswordSafe::fileOpenDefault()
{
  PassPhraseDlg dlg;
  if(dlg.exec() == PassPhraseDlg::Accepted) {
    if(open((const char *)m_default_safe, (const char *)dlg.getText()))
      statusBar()->message(tr("Default safe opened"));
    else
      statusBar()->message(tr("Unable to open the default safe"));
  }
  else {
    statusBar()->message(tr("No pass-phrase entered"));
  }
}


void MyPasswordSafe::fileChangePassPhrase()
{
  // prompt the user for a new password
  NewPassPhraseDlg dlg;
  if(dlg.exec() == NewPassPhraseDlg::Accepted) {
    // set the new password
    m_safe->setPassPhrase(EncryptedString((const char *)dlg.password().utf8()));
    statusBar()->message(tr("Pass-phrase changed"));
  }
	
  statusBar()->message(tr("Canceled"));
}


void MyPasswordSafe::lock()
{
  PassPhraseDlg dlg;
  dlg.hideCancel(true);
	
  do {
    dlg.exec(); // will only accept
  } while(m_safe->getPassPhrase() != EncryptedString(dlg.getText().utf8()));
	
  statusBar()->message(tr("MyPasswordSafe unlocked"));
}


void MyPasswordSafe::createGroup()
{
  // prompt the user for the groups name
  bool ok = false;
  QString group_name = QInputDialog::getText(tr("MyPasswordSafe"),
					     tr("What would you like to name the group?"),
					     QLineEdit::Normal,
					     QString::null, &ok, this);

  // if the user cancels, set the status bar's message and return
  if(ok == false) {
    statusBar()->message(tr("Create group canceled"));
    return;
  }

  // escape any slashes the user may have entered
  group_name.replace('\\', "\\\\");
  group_name.replace('/', "\\/");

  // get the selected item
  QListViewItem *item = pwordListView->selectedItem();
	
  // if there's an item selected
  if(item != NULL) {
    //    get its group
    QString parent_group;
    if(item->rtti() == SafeListViewItem::RTTI) {
      parent_group = static_cast<SafeListViewItem *>(item)->getGroup();
    }
    else if(item->rtti() == SafeListViewGroup::RTTI) {
      parent_group = static_cast<SafeListViewGroup *>(item)->fullname();
    }
    else {
      DBGOUT("Selected item not a valid item type");
      statusBar()->message(tr("The selected item is invalid"));
      return;
    }
		
    //    append the new group name to the item's group
    if(!parent_group.isEmpty())
      group_name = parent_group + '/' + group_name;
  }
	
  //    add the group to the view
  if(pwordListView->addGroup(group_name) == NULL) {
    statusBar()->message(tr("Unable to create the group"));
  }
  else {
    savingEnabled(true);
    statusBar()->message(tr("Created the group \"%1\"").arg(group_name));
  }
}


bool MyPasswordSafe::clearClipboardOnExit() const
{
  return clearClipboardOnExitAction->isOn();
}


void MyPasswordSafe::setClearClipboardOnExit(bool yes)
{
  clearClipboardOnExitAction->setOn(yes);
}


bool MyPasswordSafe::lockOnMinimize() const
{
  return lockOnMinimizeAction->isOn();
}


void MyPasswordSafe::setLockOnMinimize(bool yes)
{
  lockOnMinimizeAction->setOn(yes);
}


void MyPasswordSafe::showEvent(QShowEvent *)
{
  if(m_shown && lockOnMinimize()) {
    lock();
  }
  
  // prevent MyPS from asking for the pass-phrase when it first starts
  m_shown = true;
}
