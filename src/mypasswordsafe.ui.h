/* $Header: /home/cvsroot/MyPasswordSafe/src/mypasswordsafe.ui.h,v 1.44 2006/09/25 07:04:31 nolan Exp $
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

#include <cstdlib>
#include <iostream>
#include <string>
#include <qapplication.h>
#include <q3filedialog.h>
// #include <kfiledialog.h>
#include <qstatusbar.h>
#include <qclipboard.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qsettings.h>
#include <q3listview.h>
#include <qdom.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QHideEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include "tools/idle/idle.h"
#include "passphrasedlg.h"
#include "pwordeditdlg.h"
#include "manualdlg.h"
#include "newpassphrasedlg.h"
#include "myutil.hpp"
#include "safedragobject.hpp"
#include "xmlserializer.hpp"
#include "clipboard.hpp"

using namespace std;

typedef Q3FileDialog MyFileDialog;

void MyPasswordSafe::init()
{
  m_clipboard = Clipboard::instance();
  connect(editClearClipboardAction, SIGNAL(activated()), m_clipboard, SLOT(clear()));
  connect(m_clipboard, SIGNAL(cleared()), this, SLOT(clipboardCleared()));

  m_do_lock = false;
  savingEnabled(false);

  readConfig();

  m_safe = NULL; // m_safe gets setup by the Startup dialog

  m_idle = new Idle;
  m_idle->start();
  connect(m_idle, SIGNAL(secondsIdle(int)), this, SLOT(slotSecondsIdle(int)));

  settingsGenerateAndShow->setOn(PwordEditDlg::generateAndShow());
  settingsGenerateAndFetch->setOn(PwordEditDlg::generateAndFetch());
  settingsGenerateOnNew->setOn(PwordEditDlg::generateOnNew());
}

void MyPasswordSafe::destroy()
{
  delete m_idle;

  // ask to save file if needed
  if(clearClipboardOnExit())
    m_clipboard->copy("");

  writeConfig();

  if(m_safe)
    delete m_safe;
}


void MyPasswordSafe::closeEvent( QCloseEvent *e )
{
  if(closeSafe()) {
    e->accept();
    emit quit();
  }
}


bool MyPasswordSafe::closeSafe()
{
  if(m_safe) {
    if(m_safe->hasChanged()) {
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
  NewPassPhraseDlg dlg(this);
  // NOTE:  the dialog doesn't trash the memory
  if(dlg.exec() == QDialog::Rejected) {
    statusBar()->message(tr("No pass-phrase entered"));
    return;
  }

  createNewSafe(EncryptedString(dlg.password().utf8()));
}


void MyPasswordSafe::fileExit()
{
  close();
}


void MyPasswordSafe::fileMakeDefault()
{
  if(m_safe->hasChanged()) {
    if(!save()) {
      statusBar()->message(tr("The safe must be saved before it can be made the default."));
      return;
    }
  }
  setDefaultSafe(m_safe->getPath());
  fileOpenDefaultAction->setEnabled(true);
}

unsigned int MyPasswordSafe::clearTimeOut()
{
  return Clipboard::timeOut() / 1000;
}

void MyPasswordSafe::setClearTimeOut(unsigned int timeout)
{
  Clipboard::setTimeOut(timeout * 1000);
}

void MyPasswordSafe::filePreferences()
{
  PreferencesDlg dlg(this);
  dlg.setDefUser(PwordEditDlg::default_user);
  dlg.setDefaultSafe(m_default_safe);
  dlg.setGenPwordLength(PwordEditDlg::gen_pword_length);
  dlg.setMaxTries(m_max_tries);
  dlg.setIdleTime(m_idle_timeout);
  dlg.setClearTime(clearTimeOut());

  if(dlg.exec() == QDialog::Accepted) {
    PwordEditDlg::default_user = dlg.getDefUser();
    setDefaultSafe(dlg.getDefaultSafe());
    PwordEditDlg::gen_pword_length = dlg.getGenPwordLength();
    m_max_tries = dlg.getMaxTries();
    m_idle_timeout = dlg.getIdleTime();
    setClearTimeOut(dlg.getClearTime());

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
      PassPhraseDlg passphrase_dlg(this);
      SecuredString pword;
      int num_tries = 0;
      while(num_tries < m_max_tries) {
	if(passphrase_dlg.exec() == PassPhraseDlg::Accepted) {
	  pword.set(passphrase_dlg.getText().utf8());

	  open_ret = Safe::checkPassword(filename, filter, pword);

	  if(open_ret == Safe::Success)
	    break;
	  else {
	    statusBar()->message(Safe::errorToString(open_ret));
	  }

	  num_tries++;
	}
	else {
	  statusBar()->message(tr("No pass-phrase entered"));
	  return;
	}
      }

      if(open(filename, pword, filter)) {
	statusBar()->message(tr("Opened %1 entries in %2 groups").arg(m_safe->totalNumEntries()).arg(m_safe->totalNumGroups()));
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
			      (const char *)PwordEditDlg::default_user);
    if(ret == Safe::Success) {
      DBGOUT(filename << " has " << s->count() << " entries");

      if(m_safe != NULL)
	delete m_safe;
      m_safe = s;
      pwordListView->setSafe(m_safe);
      setCaption(tr("MyPasswordSafe: ") + filename);
      savingEnabled(false);
      fileSaveAsAction->setEnabled(true);
      connect(m_safe, SIGNAL(changed()), this, SLOT(savingEnabled()));
      return true;
    }
    else {
      statusBar()->message(Safe::errorToString(ret));
      delete s;
    }
  }
  else if(ret == Safe::Failed) {
    statusBar()->message(tr("Wrong pass phrase"));
  }
  else {
    statusBar()->message(Safe::errorToString(ret));
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
      Safe::Error error = m_safe->save((const char *)PwordEditDlg::default_user);
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
      Safe::Error error = m_safe->save(filename,
				       filter,
				       PwordEditDlg::default_user);
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
  }
  else {
    statusBar()->message(tr("Save file cancelled"));
  }
  return false;
}


SafeGroup *MyPasswordSafe::getSelectedParent() const
{
    SafeItem *selection = pwordListView->getSelectedItem();
    SafeGroup *parent = m_safe;

    if(selection != NULL) {
      if(selection->rtti() == SafeEntry::RTTI) {
	parent = selection->parent();
      }
      else if(selection->rtti() == SafeGroup::RTTI) {
	parent = (SafeGroup *)selection;
      }
    }

    return parent;
}

bool MyPasswordSafe::createEditDialog(SafeEntry *entry)
{
  // return false if we're already editing this item
  if(entry != NULL) {
    for(PwordEditDlg *iter = m_children.first();
	iter;
	iter = m_children.next()) {
      if(iter->getItem() == entry) {
	iter->raise();
	iter->setActiveWindow();
	return false;
      }
    }
  }

  // create the dialog
  PwordEditDlg *dlg = new PwordEditDlg(this);
  dlg->setItem(entry, getSelectedParent());

  // connect the dialog to the close slot
  connect(dlg, SIGNAL(accepted()), this, SLOT(editDialogAccepted()));
  connect(dlg, SIGNAL(rejected()), this, SLOT(editDialogRejected()));

  // add the dialog to the list
  m_children.append(dlg);

  // show the dialog
  dlg->show();

  return true;
}

bool MyPasswordSafe::removeEditDialog(const PwordEditDlg *dlg)
{
  // find and remove the dialog
  return m_children.remove(dlg);
}

void MyPasswordSafe::pwordAdd()
/* Shows the password edit dialog for the user to
 * fill out the values that the new entry will have,
 * and then adds the item to safe and view.
 */
{
  if(createEditDialog(NULL) == false) {
    statusBar()->message(tr("Failed to create password entry dialog"));
  }
}

void MyPasswordSafe::editDialogAccepted()
{
  const PwordEditDlg *dlg = dynamic_cast<const PwordEditDlg *>(sender());
  DBGOUT("editDialogClosed: " << dlg->result());

  if(dlg->isNew()) {
    pwordListView->itemAdded(dlg->getItem(), true);
    statusBar()->message(tr("Item added"));
  }
  else {
    pwordListView->itemChanged(dlg->getItem());
    statusBar()->message(tr("Password updated"));
  }

  savingEnabled(true);
  removeEditDialog(dlg);
}

void MyPasswordSafe::editDialogRejected()
{
  const PwordEditDlg *dlg = dynamic_cast<const PwordEditDlg *>(sender());

  if(dlg->isNew()) {
    statusBar()->message(tr("Add canceled"));
  }
  else {
    statusBar()->message(tr("Edit password cancelled"));
    savingEnabled(true); // updated the access time
  }

  removeEditDialog(dlg);
}


void MyPasswordSafe::pwordDelete()
{
  SafeItem *item(pwordListView->getSelectedItem());
  if(item) {
    int result = QMessageBox::warning(this, tr("Are you sure?"),
				      tr("Are you sure you want to delete this password?"),
				      QMessageBox::Yes, QMessageBox::No);
    switch(result) {
    case QMessageBox::Yes:
      deleteItem(item);
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


void MyPasswordSafe::deleteItem(SafeItem *item)
{
  DBGOUT("deleteItem");
  m_safe->setChanged(true);
  pwordListView->itemDeleted(item);
  delete item;
  savingEnabled(true);
}

void MyPasswordSafe::pwordEdit()
{
  SafeItem *item = pwordListView->getSelectedItem();
  if(item != NULL) {
    if(item->rtti() == SafeEntry::RTTI) {
      SafeEntry *entry = (SafeEntry *)item;

      if(createEditDialog(entry) == false) {
	statusBar()->message(tr("You are already editing this item."));
      }
    }
  }
  else {
    statusBar()->message(tr("No item selected"));
  }
}

void MyPasswordSafe::pwordFetch()
{
  SafeItem *item = pwordListView->getSelectedItem();
  if(item && item->rtti() == SafeEntry::RTTI) {
    SafeEntry *entry = (SafeEntry *)item;
    entry->updateAccessTime();

    // NOTE: password decrypted
    SecuredString pword(entry->password().get());
    m_clipboard->copy(QString::fromUtf8(pword.get()), true);

    statusBar()->message(tr("Password copied to clipboard"));
  }
  else {
    statusBar()->message(tr("No item selected"));
  }
}


void MyPasswordSafe::pwordFetchUser()
{
  SafeItem *item = pwordListView->getSelectedItem();
  if(item && item->rtti() == SafeEntry::RTTI) {
    SafeEntry *entry = (SafeEntry *)item;
    entry->updateAccessTime();

    m_clipboard->copy(entry->user(), true);
    statusBar()->message(tr("Username copied to clipboard"));
  }
  else {
    statusBar()->message(tr("No item selected"));
  }
}


void MyPasswordSafe::onPwordListDblClicked(Q3ListViewItem *item)
{
  if(item->rtti() == SafeListViewEntry::RTTI)
    pwordFetch();
}


void MyPasswordSafe::onPwordListRightClk( Q3ListViewItem *, const QPoint &point, int)
{
  PopupMenu->popup(point);
}


void MyPasswordSafe::helpAbout(int page)
{
  AboutDlg dlg(this);
  dlg.setCurrentPage(page);
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


Safe *MyPasswordSafe::getSafe()
{
  return m_safe;
}


int MyPasswordSafe::getGeneratedPwordLength()
{
  return PwordEditDlg::gen_pword_length;
}

void MyPasswordSafe::savingEnabled()
{
  savingEnabled(true);
}

void MyPasswordSafe::savingEnabled( bool value)
{
  if(value) {
    fileSaveAction->setEnabled(true);
    fileSaveAsAction->setEnabled(true);
  }
  else {
    fileSaveAction->setEnabled(false);
  }
}

void MyPasswordSafe::setDefaultSafe(const QString &path)
{
  m_default_safe = path;
}

const QString & MyPasswordSafe::getDefaultSafe()
{
  return m_default_safe;
}


bool MyPasswordSafe::browseForSafe( QString &filename, QString &filter, bool saving )
{
  QString all_safes(tr("All Safes (%1)").arg(Safe::getExtensions()));
  QString all_files(tr("All Files (*)"));

  QString types(QString("%1\n%2\n%3").arg(all_safes).arg(Safe::getTypes()).arg(all_files));
  QString f;
  bool ret = false;

  do {
    if(saving) {
      f = MyFileDialog::getSaveFileName(QString::null,
				       types,
				       this,
				       // "save file dialog",
				       tr("Enter a file to save to")); //,
				       // &filter);
    }
    else {
      f = MyFileDialog::getOpenFileName(QString::null,
				       types,
				       this,
				       // "open file dialog",
				       tr("Choose a file to open")); // ,
				       // &filter);
    }
  
    if(!f.isEmpty()) {
      if(saving) {
	QFileInfo info(f);
	if(info.exists()) {
	  if(QMessageBox::warning(this, tr("File exists"),
				  tr("Are you sure you want to overwrite \"%1\"?").arg(f),
				  QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
	    continue;
	  }
	}
      }

      filename = f;
      if(filter == all_safes)
	filter = QString::null;

      ret = true;
    }
    else {
      break;
    }
  } while(ret == false);

  return ret;
}


void MyPasswordSafe::createNewSafe(const EncryptedString &passphrase)
{
  if(closeSafe()) {
    m_safe = new Safe();
    m_safe->setPassPhrase(passphrase);
    pwordListView->setSafe(m_safe);
    m_safe->setChanged(false);
    setCaption(tr("MyPasswordSafe: <untitled>"));
    savingEnabled(false);
    fileSaveAsAction->setEnabled(false);
    statusBar()->message(tr("Created new safe"));
  }
}


void MyPasswordSafe::fileOpenDefault()
{
  PassPhraseDlg dlg(this);
  if(dlg.exec() == PassPhraseDlg::Accepted) {
    if(open((const char *)getDefaultSafe(), (const char *)dlg.getText()))
      statusBar()->message(tr("Opened %1 entries in %2 groups").arg(m_safe->totalNumEntries()).arg(m_safe->totalNumGroups()));
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
  NewPassPhraseDlg dlg(this);
  if(dlg.exec() == NewPassPhraseDlg::Accepted) {
    // set the new password
    m_safe->setPassPhrase(EncryptedString((const char *)dlg.password().utf8()));
    savingEnabled(true);
    statusBar()->message(tr("Pass-phrase changed"));
  }
  else {
    statusBar()->message(tr("Canceled"));
  }
}

void MyPasswordSafe::hideChildren()
{
    for(PwordEditDlg *iter = m_children.first();
	iter;
	iter = m_children.next()) {
      iter->hide();
    }
}

void MyPasswordSafe::showChildren()
{
    for(PwordEditDlg *iter = m_children.first();
	iter;
	iter = m_children.next()) {
      iter->show();
    }
}

void MyPasswordSafe::lock()
{
  PassPhraseDlg dlg(this);
  dlg.hideCancel(true);

  hide();
  hideChildren();
  //closeChild(); // NOTE: hiding would cause a crash on show
  // FIXME: hiding the edit dialog should work with non-modal dialogs

  m_clipboard->clear();

  do {
    dlg.exec(); // will only accept
  } while(m_safe->getPassPhrase() != EncryptedString(dlg.getText().utf8()));
  
  showNormal();
  showChildren();

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

  // get the selected item
  SafeItem *item = pwordListView->getSelectedItem();
  SafeGroup *parent = m_safe;

  // if there's an item selected
  if(item != NULL) {
    //    get its group
    if(item->rtti() == SafeEntry::RTTI) {
      parent = item->parent();
    }
    else if(item->rtti() == SafeGroup::RTTI) {
      parent = (SafeGroup *)item;
    }
    else {
      DBGOUT("Selected item not a valid item type");
      statusBar()->message(tr("The selected item is invalid"));
      return;
    }
  }

  //    create the group
  SafeGroup *group = new SafeGroup(parent, group_name);
  if(group == NULL) {
    statusBar()->message(tr("Unable to create the group"));
  }
  else {
    m_safe->setChanged(true);
    pwordListView->itemAdded(group, true);

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

void MyPasswordSafe::setGenerateAndShow(bool yes) 
{
  PwordEditDlg::setGenerateAndShow(yes);
}

void MyPasswordSafe::setGenerateAndFetch(bool yes) 
{
  PwordEditDlg::setGenerateAndFetch(yes);
}

void MyPasswordSafe::setGenerateOnNew(bool yes) 
{
  PwordEditDlg::setGenerateOnNew(yes);
}

#ifdef Q_WS_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

bool MyPasswordSafe::isMinimized() const
{
  //DBGOUT("isMinimized: " << QMainWindow::isMinimized());

  Display *display = x11Display();
  Atom atom;
  static const int prop_buffer_lengh = 1024 * 1024;
  Atom type_returned = 0;
  int format_returned = 0;
  unsigned long n_clients;
  unsigned long bytes_after_return = 0;
  unsigned char *data = NULL;

  atom = XInternAtom (display, "WM_STATE", False);
  XGetWindowProperty (display, (Window)winId(),
		      atom, 0, prop_buffer_lengh, False, atom,
		      &type_returned, &format_returned, &n_clients,
		      &bytes_after_return, &data);

  /*#ifdef DEBUG
  for(int i = 0; i < n_clients; i++) {
    DBGOUT("data[" << i << "] = " << data[i] << "\t" << (data[i] == IconicState));
  }
  #endif*/
  return (n_clients > 0 && data[0] == IconicState);
}
#else
bool MyPasswordSafe::isMinimized() const
{
  return Q3MainWindow::isMinimized();
}
#endif

void MyPasswordSafe::hideEvent(QHideEvent *)
{
  DBGOUT("Hide event: " << isMinimized());

  // Fire a timer to see if we need to lock due
  // to a minimization after 5 seconds
  if(lockOnMinimize()) {
    QTimer::singleShot(5*1000, this, SLOT(checkMinimization()));
  }
}

void MyPasswordSafe::showEvent(QShowEvent *)
{
  DBGOUT("Show event: " << isMinimized());

  if(m_do_lock && lockOnMinimize()) {
    m_do_lock = false;
    lock();
  }
}

static const char *MyPS_Column_Fields[] = {
  "/MainWindow/name_width",
  "/MainWindow/user_width",
  "/MainWindow/notes_width",
  "/MainWindow/modtime_width",
  "/MainWindow/accessed_width",
  "/MainWindow/created_width",
  "/MainWindow/lifetime_width",
  NULL
};

void MyPasswordSafe::readConfig()
{
  m_config.setPath("SemanticGap.com", "MyPasswordSafe");
  m_config.beginGroup("/MyPasswordSafe");

  m_first_time = m_config.readBoolEntry("/prefs/first_time", true);
  PwordEditDlg::gen_pword_length = m_config.readNumEntry("/prefs/password_length", 8);
  
  m_default_safe = m_config.readEntry("/prefs/default_safe", "");
  if(m_default_safe.isEmpty())
    fileOpenDefaultAction->setEnabled(false);
  
  PwordEditDlg::default_user = m_config.readEntry("/prefs/default_username", "");
  setClearClipboardOnExit(m_config.readBoolEntry("/prefs/clear_clipboard", true));
  setLockOnMinimize(m_config.readBoolEntry("/prefs/lock_on_minimize", true));
  PwordEditDlg::setGenerateAndShow(m_config.readBoolEntry("/prefs/generate/show", true));
  PwordEditDlg::setGenerateAndFetch(m_config.readBoolEntry("/prefs/generate/fetch", true));
  PwordEditDlg::setGenerateOnNew(m_config.readBoolEntry("/prefs/generate/on_new", true));
  
  m_max_tries = m_config.readNumEntry("/prefs/max_tries", 3);
  if(m_max_tries > 10)
    m_max_tries = 10;
  
  m_idle_timeout = m_config.readNumEntry("/prefs/idle_timeout", 2); // 2 minute default
  setClearTimeOut(m_config.readNumEntry("/prefs/clear_timeout", 60)); // 1 minute default

  int w, h;
  w = m_config.readNumEntry("/MainWindow/width", 400);
  h = m_config.readNumEntry("/MainWindow/height", 320);
  resize(QSize(w, h));
    
  w = m_config.readNumEntry("/MainWindow/left", -1);
  h = m_config.readNumEntry("/MainWindow/top", -1);
  if(w != -1 && h != -1) {
    move(w, h);
  }

  for(int i = 0; MyPS_Column_Fields[i] != NULL; i++) {
    readColumnWidth(i, MyPS_Column_Fields[i]);
  }
  
  m_config.endGroup();  
}

void MyPasswordSafe::readColumnWidth(int col, const char *name)
{
  int w = m_config.readNumEntry(name, -1);
  if(w > -1)
    pwordListView->setColumnWidth(col, w);
}

void MyPasswordSafe::writeColumnWidth(int col, const char *name)
{
  m_config.writeEntry(name, pwordListView->columnWidth(col));
}

void MyPasswordSafe::writeConfig()
{
  // save config settings   
  m_config.beginGroup("/MyPasswordSafe");

  m_config.writeEntry("/prefs/first_time", false);
  m_config.writeEntry("/prefs/password_length", (int)PwordEditDlg::gen_pword_length);
  m_config.writeEntry("/prefs/default_safe", m_default_safe);
  m_config.writeEntry("/prefs/default_username", PwordEditDlg::default_user);
  m_config.writeEntry("/prefs/generate/show", PwordEditDlg::generateAndShow());
  m_config.writeEntry("/prefs/generate/fetch", PwordEditDlg::generateAndFetch());
  m_config.writeEntry("/prefs/generate/on_new", PwordEditDlg::generateOnNew());
  m_config.writeEntry("/prefs/clear_clipboard", clearClipboardOnExit());
  m_config.writeEntry("/prefs/lock_on_minimize", lockOnMinimize());
  m_config.writeEntry("/prefs/max_tries", m_max_tries);
  m_config.writeEntry("/prefs/idle_timeout", m_idle_timeout);
  m_config.writeEntry("/prefs/clear_timeout", (int)clearTimeOut());

  QSize sz = size();
  m_config.writeEntry("/MainWindow/width", sz.width());
  m_config.writeEntry("/MainWindow/height", sz.height());

  QPoint position = pos();
  m_config.writeEntry("/MainWindow/left", position.x());
  m_config.writeEntry("/MainWindow/top", position.y());

  for(int i = 0; MyPS_Column_Fields[i] != NULL; i++) {
    writeColumnWidth(i, MyPS_Column_Fields[i]);
  }

  m_config.endGroup();
}


void MyPasswordSafe::dragObjectDropped(QMimeSource *drag, SafeListViewItem *target)
{
  DBGOUT("dragObjectDropped");
  QDomDocument doc("safe");
  if(SafeDragObject::decode(drag, doc)) {
    DBGOUT("Dragged data: " << endl << doc.toString());

    // find the group to add the dragged data to
    SafeListViewGroup *lvi_parent = NULL;
    SafeGroup *safe_parent = m_safe;
    if(target) {
      if(target->rtti() == SafeListViewGroup::RTTI)
	lvi_parent = (SafeListViewGroup *)target;
      else
	lvi_parent = (SafeListViewGroup *)target->parent();

      if(lvi_parent)
	safe_parent = lvi_parent->group();
    }

    // add the items contained in the data
    QDomNode n = doc.firstChild();
    for(; !n.isNull(); n = n.nextSibling()) {
      if(n.isElement()) {
	QDomElement elem = n.toElement();
	QString tag_name = elem.tagName();
	if(tag_name == "item") {
	  SafeEntry *entry = new SafeEntry(safe_parent);
	  if(XmlSerializer::safeEntryFromXml(elem, entry)) {
	    pwordListView->itemAdded(entry);
	  }
	}
	else if(tag_name == "group") {
	  SafeGroup *group = new SafeGroup(safe_parent);
	  if(XmlSerializer::safeGroupFromXml(elem, group))
	    pwordListView->itemAdded(group);
	}
      }
    }

    m_safe->setChanged(true);
  }
}

bool MyPasswordSafe::firstTime() const
{
  return m_first_time;
}


void MyPasswordSafe::clipboardCleared()
{
  statusBar()->message(tr("Clipboard cleared"));
}

void MyPasswordSafe::helpAbout()
{
  helpAbout(0);
}

void MyPasswordSafe::slotSecondsIdle(int secs)
{
  if(m_idle_timeout > 0) { // zero means off
    int timeout = m_idle_timeout * 60;
    int count_down = timeout - secs;

    if(count_down == 30
       || count_down == 20
       || count_down <= 10) {
      statusBar()->message(tr("Locking in %1 seconds.").arg(count_down), 2000);
    }

    if(count_down <= 0 && isVisible()) {
      DBGOUT("Locking due to idle");
      lock();
    }
  }
}

void MyPasswordSafe::checkMinimization()
{
  DBGOUT("isMinimized: " << (isMinimized() == Q3MainWindow::isMinimized()));
  if(lockOnMinimize()) {
    m_do_lock = isMinimized();
  }
}
