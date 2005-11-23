/* $Header: /home/cvsroot/MyPasswordSafe/src/pwordeditdlg.ui.h,v 1.9 2005/11/23 13:21:29 nolan Exp $
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
#include "pwsafe/Util.h"

void PwordEditDlg::accept()
{
	// if the username or password have changed,
	if((m_orig_pword != passwordEdit->text() && m_orig_pword.length() > 0) ||
	   (m_orig_user != userEdit->text() && m_orig_user.length() > 0)) {
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

	QDialog::accept();
}

void PwordEditDlg::showPassword()
{
	showButton->setText(tr("Hide"));
	passwordEdit->setEchoMode(QLineEdit::Normal);
}

void PwordEditDlg::hidePassword()
{
	showButton->setText(tr("Show"));
	passwordEdit->setEchoMode(QLineEdit::Password);
}

void PwordEditDlg::togglePassword()
{
	if(passwordEdit->echoMode() == QLineEdit::Normal) {
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
	string s(GetAlphaNumPassword(m_pword_length));
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
	m_orig_user = text;
	userEdit->setText(text);
}


void PwordEditDlg::setPassword( const QString &text )
{
	m_orig_pword = text;
	passwordEdit->setText(text);
}


void PwordEditDlg::setNotes( const QString &text )
{
	notesEdit->setText(text);
}


void PwordEditDlg::setGenPwordLength( int value )
{
	m_pword_length = value;
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
