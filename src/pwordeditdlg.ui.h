/* $Header: /home/cvsroot/MyPasswordSafe/src/pwordeditdlg.ui.h,v 1.8 2004/11/01 21:34:58 nolan Exp $
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
#include "mypasswordsafe.h"
#include "pwsafe/Util.h"

void PwordEditDlg::showPassword()
{
	if(passwordEdit->echoMode() == QLineEdit::Normal) {
		showButton->setText(tr("Show"));
		passwordEdit->setEchoMode(QLineEdit::Password);
	}
	else {
		showButton->setText(tr("Hide"));
		passwordEdit->setEchoMode(QLineEdit::Normal);
	}
}


void PwordEditDlg::genPassword()
{    
	string s(GetAlphaNumPassword(m_pword_length));
	passwordEdit->setText(s.c_str());
}


void PwordEditDlg::fetchPassword()
{
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
