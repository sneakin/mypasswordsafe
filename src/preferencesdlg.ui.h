/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
/* $Header: /home/cvsroot/MyPasswordSafe/src/preferencesdlg.ui.h,v 1.1.1.1 2004/05/04 22:47:07 nolan Exp $
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
	
#include "safe.hpp"

void PreferencesDlg::onBrowse()
{
    QString filename = QFileDialog::getOpenFileName(
	    QString::null, SafeSerializer::getTypes().c_str(), this, "file open", "Open a password safe" );
    if(!filename.isEmpty()) {
	defaultSafeBox->setText(filename);
    }
}


QString PreferencesDlg::getDefUser()
{
    return defUserBox->text();
}


QString PreferencesDlg::getDefaultSafe()
{
    return defaultSafeBox->text();
}



void PreferencesDlg::setDefUser( const QString &user )
{
    defUserBox->setText(user);
}


void PreferencesDlg::setDefaultSafe( const QString &safe )
{
    defaultSafeBox->setText(safe);
}


int PreferencesDlg::getGenPwordLength()
{
    return pwordLengthBox->value();
}


void PreferencesDlg::setGenPwordLength( int length )
{
    pwordLengthBox->setValue(length);
}

int PreferencesDlg::getMaxTries()
{
	return maxTriesBox->value();
}

void PreferencesDlg::setMaxTries(int tries)
{
	maxTriesBox->setValue(tries);
}



void PreferencesDlg::onHelp()
{
	QWhatsThis::enterWhatsThisMode();
}
