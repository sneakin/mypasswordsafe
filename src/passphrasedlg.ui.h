/* $Header: /home/cvsroot/MyPasswordSafe/src/passphrasedlg.ui.h,v 1.4 2004/11/01 21:34:58 nolan Exp $
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


const QString PassPhraseDlg::getText() const
{
    return pwordBox->text();
}


void PassPhraseDlg::pwordBox_textChanged( const QString &text )
{
    m_passkey = text;
}


void PassPhraseDlg::hideCancel(bool yes)
{
	if(yes)
		cancelButton->hide();
	else
		cancelButton->show();
}
