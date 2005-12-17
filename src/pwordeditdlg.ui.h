/* $Header: /home/cvsroot/MyPasswordSafe/src/pwordeditdlg.ui.h,v 1.14 2005/12/17 11:47:13 nolan Exp $
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

#include <qapplication.h>
#include <qclipboard.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include "mypasswordsafe.h"
#include "safe.hpp"
#include "pwsafe/Util.h"

int PwordEditDlg::gen_pword_length = 8;
QString PwordEditDlg::default_user;

void PwordEditDlg::init()
{
	setItem(NULL);
}

SafeEntry *PwordEditDlg::getItem() const
{
	return m_item;
}

void PwordEditDlg::setItem(SafeEntry *item, SafeGroup *future_group)
{
	m_item = item;
	updateItem(future_group);
}

void PwordEditDlg::updateItem(SafeGroup *future_group)
{
	if(m_item == NULL) {
		m_future_group = future_group;

		setNotes(QString::null);
		setUser(default_user);

		// automatically generate the password
		genPassword(false);

		showDetails(false);
	}
	else {
		m_future_group = NULL;

		setItemName(m_item->name());
		setUser(m_item->user());
		// NOTE: password decrypted
		setPassword(QString::fromUtf8(m_item->password().get().get()));
		setNotes(m_item->notes());
		setAccessedOn(m_item->accessTime());
		setCreatedOn(m_item->creationTime());
		setModifiedOn(m_item->modifiedTime());
		setLifetime(m_item->lifetime());
		setUUID(m_item->uuid().toString());

		m_item->updateAccessTime();

		showDetails(true);
		hidePassword();
	}
}

void PwordEditDlg::accept()
{
	DBGOUT("PwordEditDlg::accept");

	// if the username or password have changed,
	if(!isNew()
	   && ((m_item->password() != EncryptedString(passwordEdit->text().utf8())
		&& m_item->password().length() > 0)
	       || (m_item->user() != userEdit->text() && m_item->user().length() > 0))) {
		// prompt the user to see if they are sure they
		// want to edit the entry
		switch(QMessageBox::warning(this, tr("Entry Changed"),
					    tr("Are you sure that you want to edit this password?"),
					    QMessageBox::Yes,
					    QMessageBox::No,
					    QMessageBox::Cancel)) {
		case QMessageBox::No:
			reject();
		case QMessageBox::Cancel:
			return;
		}
	}

	// create a new item if we don't have one
	if(isNew()) {
		m_item = new SafeEntry(m_future_group,
				       getItemName(),
				       getUser(),
				       EncryptedString(getPassword().utf8()),
				       getNotes());
		assert(m_item != NULL);
		m_item->safe()->setChanged(true);
	}
	else {
		// otherwise update the item if it's set
		m_item->setName(getItemName());
		m_item->setUser(getUser());
		m_item->setPassword(EncryptedString((const char *)getPassword().utf8()));
		m_item->setNotes(getNotes());
		m_item->updateModTime();

		m_item->safe()->setChanged(true);
	}

	accepted();
	QDialog::accept();
}

void PwordEditDlg::reject()
{
	rejected();
	QDialog::reject();
}

void PwordEditDlg::showPassword()
{
	DBGOUT("showPassword");
	showButton->setText(tr("Hide"));
	passwordEdit->setEchoMode(QLineEdit::Normal);
}

void PwordEditDlg::hidePassword()
{
	DBGOUT("hidePassword");
	showButton->setText(tr("Show"));
	passwordEdit->setEchoMode(QLineEdit::Password);
}

void PwordEditDlg::togglePassword()
{
	DBGOUT("togglePassword " << passwordEdit->echoMode());
	if(passwordEdit->echoMode() == QLineEdit::Password) {
		showPassword();
	}
	else {
		hidePassword();
	}
}


void PwordEditDlg::genPassword()
{
	genPassword(true);
}

void PwordEditDlg::genPassword(bool fetch)
{
	string s(GetAlphaNumPassword(gen_pword_length));
	passwordEdit->setText(s.c_str());
	
	// FIXME: make these optional
	showPassword();

	if(fetch) {
		fetchPassword();
	}
}


void PwordEditDlg::fetchPassword()
{
	// FIXME: move the clipboard stuff into a singleton. Then copying,
	// clearing, the timer, and even the preference variable can be
	// in one place.
	MyPasswordSafe *myps = dynamic_cast<MyPasswordSafe *>(parent());
	myps->startClearTimer();

	copyToClipboard(passwordEdit->text());
}


QString PwordEditDlg::getItemName() const
{
	return nameEdit->text();
}


QString PwordEditDlg::getUser() const
{
	return userEdit->text();
}


QString PwordEditDlg::getPassword() const
{
	return passwordEdit->text();
}

QString PwordEditDlg::getNotes() const
{
	return notesEdit->text();
}


void PwordEditDlg::setItemName( const QString &text )
{
	nameEdit->setText(text);
}


void PwordEditDlg::setUser( const QString &text )
{
	userEdit->setText(text);
}


void PwordEditDlg::setPassword( const QString &text )
{
	passwordEdit->setText(text);
}


void PwordEditDlg::setNotes( const QString &text )
{
	notesEdit->setText(text);
}


void PwordEditDlg::setGenPwordLength( int value )
{
	gen_pword_length = value;
}

void PwordEditDlg::setCreatedOn(const QDateTime &time)
{
	createdOnLabel->setText(tr("Created on: %1").arg(time.toString()));
}

void PwordEditDlg::setAccessedOn(const QDateTime &time)
{
	accessedOnLabel->setText(tr("Accessed on: %1").arg(time.toString()));
}

void PwordEditDlg::setModifiedOn(const QDateTime &time)
{
	modifiedOnLabel->setText(tr("Modified on: %1").arg(time.toString()));
}

void PwordEditDlg::setLifetime(const QTime &time)
{
	lifetimeLabel->setText(tr("Lifetime: %1").arg(time.toString()));
}

void PwordEditDlg::setUUID(const QString &uuid)
{
	uuidLabel->setText(tr("UUID: %1").arg(uuid));
}

void PwordEditDlg::showDetails(bool yes)
{
	QWidget *tab(tabWidget->page(1));
	tabWidget->setTabEnabled(tab, yes);
}

bool PwordEditDlg::detailsShown() const
{
	QWidget *tab(tabWidget->page(1));
	return tabWidget->isTabEnabled(tab);
}

bool PwordEditDlg::isNew() const
{
	return (m_future_group != NULL);
}
