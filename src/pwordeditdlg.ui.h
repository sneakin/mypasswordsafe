/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
/* $Header: /home/cvsroot/MyPasswordSafe/src/pwordeditdlg.ui.h,v 1.2 2004/05/04 22:48:44 nolan Exp $
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

#include <qapplication.h>
#include <qclipboard.h>
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
    QApplication::clipboard()->setText(passwordEdit->text());
}


QString PwordEditDlg::getItemName()
{
    return nameEdit->text();
}


QString PwordEditDlg::getUser()
{
    return userEdit->text();
}


QString PwordEditDlg::getPassword()
{
    return passwordEdit->text();
}

QString PwordEditDlg::getNotes()
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
