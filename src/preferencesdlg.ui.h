/* $Header: /home/cvsroot/MyPasswordSafe/src/preferencesdlg.ui.h,v 1.7 2005/11/23 13:21:28 nolan Exp $
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
	
#include "safe.hpp"

void PreferencesDlg::onBrowse()
{
    QString filename = QFileDialog::getOpenFileName(
	    QString::null, Safe::getTypes(),
	    this, "file open", "Open a password safe" );
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

int PreferencesDlg::getIdleTime()
{
	return idleTimeBox->value();
}

void PreferencesDlg::setIdleTime(int minutes)
{
	idleTimeBox->setValue(minutes);
}

int PreferencesDlg::getClearTime()
{
	return clearTimeBox->value();
}

void PreferencesDlg::setClearTime(int seconds)
{
	clearTimeBox->setValue(seconds);
}

void PreferencesDlg::onHelp()
{
	QWhatsThis::enterWhatsThisMode();
}
